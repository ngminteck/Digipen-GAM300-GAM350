/*______________________________________________________________________________

@file     HelperFn.h
@author   Alfi Ramli  (alfi.ramli, alfi.ramli@digipen.edu)

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of
Technology is prohibit-ed.
_____________________________________________________________________________*/

#ifndef HELPER_FN_H
#define HELPER_FN_H


std::string GetDirFromPath(const std::string& input); //uhh dont really need this though
std::string GetExtensionFromPath(const std::string& input);
std::string GetFilenameFromPath(const std::string& input);
void SeperateBy(const std::string& seperator, std::string& input);

std::string GetCurrentDir();
std::vector<std::string> GetFilesInDir(const std::string& dir, const std::string& ext, const std::string& prefix = ASC_WILDCARD, const char *contain = nullptr);
bool compare_str(const std::string& a, const std::string& b);
std::size_t find_str(const std::string& a, const std::string& b);
bool DeleteFileInDir(const std::string& dir, const std::string& filename);

std::string GetLastOpenedFile();
void SetLastOpenedFile(const std::string &fileName);

float Time();
//////////////////////////* mainly for grid *//////////////////////////
static const float SIN60 = sin(ASC_DEG2RAD(60));
static const float SIN30 = sinf(ASC_DEG2RAD(30));
static const float COS30 = cosf(ASC_DEG2RAD(30));

//helpers
inline Vec3 vec223(Vec2 const& vec, float h = 0.f) { return Vec3{ vec.x, h, vec.y }; }
inline Vec2 vec322(Vec3 const& vec) { return Vec2{ vec.x, vec.z }; }

inline void rotby30(Vec3& vec)
{
  float x = vec.x * COS30 - vec.z * SIN30;
  float z = vec.x * SIN30 + vec.z * COS30;
  vec.x = x;
  vec.z = z;
}

inline void rotbyneg_30(Vec3& vec)
{
  float x = vec.x * COS30 + vec.z * SIN30;
  float z = vec.x * -SIN30 + vec.z * COS30;
  vec.x = x;
  vec.z = z;
}

//angle takes in radian
inline Vec2 rot2dvec(Vec2 const& vec, float rad)
{
  float c = cosf(rad);
  float s = sinf(rad);
  return Vec2(c * vec.x - s * vec.y , s * vec.x + c * vec.y );
}
//////////////////////////* mainly for grid *//////////////////////////

//for when we are lazy
#define ASC_VarGetSet(type, varname) \
private: \
type m_##varname; \
public: \
inline void Set##varname(const type& param) { m_##varname = param; } \
inline const type& Get##varname() const { return m_##varname; }



// Replaces the macro ZeroMemory
template <typename T, size_t length = sizeof(T)>
void ZeroMem(T &location)
{
  memset(reinterpret_cast<void *>(&location), 0, length);
}


template <typename M, typename V>
void MapToVec_key(const  M & m, V & v) {
  for (typename M::const_iterator it = m.begin(); it != m.end(); ++it) 
  {
    v.push_back(it->first);
  }
}


//for easy creation of enum type 
#define TYPECREATE(type_name) \
StrToType.insert({std::string(#type_name), type_name}); \
TypeToStr.insert({type_name , #type_name}); \
TypeList.push_back(#type_name);

/*
We do not need this anymore...I didn't know we can template initialize static variables
*/
//#define TYPEDEFINE(TP, Derived) \
//std::vector<const char*> TypeHelper<TP, Derived>::TypeList; \
//std::unordered_map<std::string, TP> TypeHelper<TP, Derived>::StrToType; \
//std::unordered_map<TP, const char *> TypeHelper<TP, Derived>::TypeToStr; 

/*
Note: after creation of class user have to static initialize the three static member
as they are static....this is class is here so you don't have to paste too many code 
everywhere
can use TYPEDEFINE , if the type is NAMESPACE::type remember to include the namespace

Usage struct Name : TypeHelper<TypeEnumName, Name, 100>
*/
template<typename TP, typename Derived, size_t reserve_sz = 200>
struct TypeHelper
{
  static std::vector<const char*> TypeList;
  static std::unordered_map<std::string, TP> StrToType;
  static std::unordered_map<TP, const char *> TypeToStr;
  //this class will only create once so make it a static is the best
  TypeHelper()
  {
    if (!TypeList.size())
    {
      TypeList.reserve(reserve_sz);
      StrToType.reserve(reserve_sz);
      TypeToStr.reserve(reserve_sz);
      static_cast<Derived*>(this)->Create();
    }
  }
  ~TypeHelper()
  {
      TypeList.clear();
      StrToType.clear();
      TypeToStr.clear();
  }
  //user of this class must create this function and within the function use TYPECREATE
  /*
    void Create()
    {
      TYPECREATE(SI_PlayerController);
      TYPECREATE(SI_Bullet);
      TYPECREATE(SI_EnemyBasic);
    }
  */
  //virtual void Create() = 0;
};

template<typename TP, typename Derived, size_t reserve_sz>
std::vector<const char*> TypeHelper<TP, Derived, reserve_sz>::TypeList;

template<typename TP, typename Derived, size_t reserve_sz>
std::unordered_map<std::string, TP> TypeHelper<TP, Derived, reserve_sz>::StrToType;

template<typename TP, typename Derived, size_t reserve_sz>
std::unordered_map<TP, const char *> TypeHelper<TP, Derived, reserve_sz>::TypeToStr;

//this is another helper that helps in creation of objects automatically using map
//template<typename baseType, typename TP, typename Derived, size_t reserve_sz = 200>
//struct CreatorTypeHelper : public TypeHelper<TP, Derived, reserve_sz>
//{
//  static std::unordered_map<TP, std::shared_ptr<baseType>> m_creator;
//};
//
//template<typename baseType, typename TP, typename Derived, size_t reserve_sz>
//std::unordered_map<TP, std::shared_ptr<baseType>> CreatorTypeHelper<baseType, TP, Derived, reserve_sz>::m_creator;

template<typename VecType>
struct Temptween
{
  Temptween() : t(2.f), modfac(1.0f) {}
  Temptween(VecType const& start, VecType const& end, float modification_fac)
    : t(2.f), modfac(modification_fac), startend{ start, end } {}
  inline void reset() { t = 0.f; curr = startend[0]; }
  inline void SetStart(VecType const& start) { startend[0] = start; }
  inline void SetEnd(VecType const& end) { startend[1] = end; }
  inline void SetStartEnd(VecType const& start, VecType const& end) { startend[0] = start; startend[1] = end; }
  float t;
  float modfac;
  VecType startend[2], curr;
  inline bool isDest() const { return t > 1.0f; }
  inline void update(float gdt)
  {
    if (isDest()) return;
    t += gdt * modfac;
    curr = startend[0] + t * (startend[1] - startend[0]);
  };
};


#endif
