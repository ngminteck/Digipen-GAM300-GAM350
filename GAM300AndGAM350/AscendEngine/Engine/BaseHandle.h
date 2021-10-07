/******************************************************************************/
/*!
\file   BaseHandle.h
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief
Contains definitions for BasicHandle, Slot, Handle, SharedResource, SharedHandle

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

#define ASC_INVALID_HANDLE -1

/////////////////////////////////////////////////HANDLES STUFF////////////////////////////////////
struct BasicHandle
{
public:
  BasicHandle(unsigned int id = ASC_INVALID_HANDLE)
    : m_iteration(0), m_id(id), m_objptr(nullptr) {}
  //inline operator bool() const { return m_id == ASC_INVALID_HANDLE; };
  //inline operator unsigned int() const { return m_id; };
  inline unsigned& Val() { return m_id; }
  inline unsigned Val() const { return m_id; }
  inline bool IsValid() const { return m_id != ASC_INVALID_HANDLE; }

  //these functions are for Handle manager
  //use wisely as Handle will be invalidated
  inline void InValidateHandle() { m_id = ASC_INVALID_HANDLE; m_objptr = nullptr; }
  inline int Iteration() const { return m_iteration; }

  bool operator==(const BasicHandle& hdl) const { return m_iteration == hdl.m_iteration && m_id == hdl.m_id; }
  bool operator!=(const BasicHandle& hdl) const { return !operator==(hdl); }
  //forward the basic methods in ObjType
  //ObjType* operator->() { return m_objptr; }
  //ObjType const* operator->() const { return m_objptr; }
  inline void* GetPtr() { return m_objptr; }
  inline void const* GetPtr() const { return m_objptr; }
  inline void SetObjPtr(void* ptr) { m_objptr = ptr; }
protected:
  int m_iteration;
  unsigned int m_id;
  void* m_objptr;
};

//static BasicHandle const INVALID_BASIC_HANDLE;
struct SlotData
{
  int m_useNum;
  //bool m_valid;
  typedef BasicHandle BHType;
  BHType m_baseHdl;
  SlotData(int use = 0, const BHType ptr = BHType())//INVALID_BASIC_HANDLE)
    : m_useNum(use),/* m_valid(valid),*/
    m_baseHdl(ptr) {}
};


//////////////////////////////////shared resources implementation////////////////////////////
template<typename ObjType>
struct Slot
{
  int m_useNum;
  ObjType m_obj;
  inline operator ObjType&() { return m_obj; }
  inline operator ObjType const&() const { return m_obj; }
  Slot() : m_useNum(0), m_obj() {}
  Slot(const ObjType& copy, int use) : m_useNum(use), m_obj(copy) {}
};

/*
user needs to make sure that ObjType can be default construct
*/
template<typename ObjType, size_t Arrsz = 20000>
struct SharedResource
{
  //defined type 
  typedef Slot<ObjType> SlotType;
  typedef std::array<SlotType, Arrsz> ContType;
  typedef typename ContType::iterator Iter;

  //methods
  SharedResource(const ObjType& obj);
  virtual ~SharedResource();
  SharedResource(const SharedResource& hdl);
  SharedResource& operator=(const SharedResource& hdl);
  inline ObjType& operator*() { return *m_slot; }
  inline ObjType* operator->() { return &static_cast<ObjType&>(*m_slot); }
protected:
  Iter m_slot;
  static size_t m_count;
  static std::vector<Iter> m_unused;
  static ContType m_data;

  //helper 
  void FreeSlot();
};

/*
declarations of static templated objects
*/
template<typename ObjType, size_t Arrsz>
size_t SharedResource<ObjType, Arrsz>::m_count = 0;

template<typename ObjType, size_t Arrsz>
std::vector<typename SharedResource<ObjType, Arrsz>::Iter> 
SharedResource<ObjType, Arrsz>::m_unused;

template<typename ObjType, size_t Arrsz>
typename SharedResource<ObjType, Arrsz>::ContType 
SharedResource<ObjType, Arrsz>::m_data;

template<typename ObjType, size_t Arrsz>
inline SharedResource<ObjType, Arrsz>::SharedResource(const ObjType & obj)
{
  //either from unused or a new slot
  if (m_unused.size())
  {
    //get an old index
    m_slot = m_unused.back();
    m_unused.pop_back();
  }
  else
  {
    //get a new index
    //m_data.emplace_back(); //no emplace or push back since we are using array
    m_slot = m_data.begin() + m_count++;
  }

  *m_slot = SlotType(obj, 1);
}

template<typename ObjType, size_t Arrsz>
inline SharedResource<ObjType, Arrsz>::~SharedResource()
{
  --m_slot->m_useNum;
  FreeSlot();
}

template<typename ObjType, size_t Arrsz>
inline SharedResource<ObjType, Arrsz>::SharedResource(const SharedResource & hdl)
{
  ++hdl.m_slot->m_useNum;
  m_slot = hdl.m_slot;

}

template<typename ObjType, size_t Arrsz>
inline SharedResource<ObjType, Arrsz>& 
SharedResource<ObjType, Arrsz>::operator=(const SharedResource & hdl)
{
  //for assignment we will need to
  //increase the counter of the new hdl
  //and decrease the current counter
  //this order will prevent the case during self assignment
  ++hdl.m_slot->m_useNum;
  --m_slot->m_useNum;
  //free slot is here because we need to check the current m_slot to see
  //if it is free
  FreeSlot();
  m_slot = hdl.m_slot;
  return *this;
}

template<typename ObjType, size_t Arrsz>
inline void SharedResource<ObjType, Arrsz>::FreeSlot()
{
  if (!m_slot->m_useNum)
  {
    m_unused.push_back(m_slot);
    m_slot = Iter();
  }
}

//////////////////////////////////shared resources implementation////////////////////////////

struct BaseShareHandle
{
  template<typename T>//, typename h>
  friend class HandleManager;

  typedef SlotData SlotDataT;
  typedef BasicHandle HandleT;
  typedef BaseShareHandle castType;

  BaseShareHandle(const HandleT& baseHdl = HandleT());// INVALID_BASIC_HANDLE);
  ~BaseShareHandle();
  BaseShareHandle(const BaseShareHandle& hdl);
  BaseShareHandle& operator=(const BaseShareHandle& hdl);
  //we do not need move as there is no underlying 
  //SharedHandle(SharedHandle&& hdl);
  //SharedHandle& operator=(SharedHandle&& hdl);

  inline void AssignNewInValid() { *this = BaseShareHandle(); }
  inline unsigned Val() const { return m_data[m_slot].m_baseHdl.Val(); }
  inline bool IsValid() const { return m_data[m_slot].m_baseHdl.IsValid(); }
  inline int Iteration() const { return m_data[m_slot].m_baseHdl.Iteration(); }
  bool operator==(const BaseShareHandle& hdl) const { return m_slot == hdl.m_slot; }
  bool operator!=(const BaseShareHandle& hdl) const { return !operator==(hdl); }
  static void clearData() {
      m_data.clear();
  }
protected:
  inline void InValidateHandle() { m_data[m_slot].m_baseHdl.InValidateHandle(); }
  inline HandleT& GetBasicHandle() { return m_data[m_slot].m_baseHdl; }
  //BasicHandle const* m_baseHdl;
  int m_slot = -1;
  static std::vector<int> m_unused;
  static std::vector<SlotDataT> m_data;

private:
  //for easier debug -_-
  SlotDataT* m_ptr;
  //helper
  void FreeSlot();
};

/* much ugly implementation */
//this will soon change to Handle to allow easy integration
template<typename ObjType>
struct SharedHandle final : public BaseShareHandle
{
  template<typename T>//, typename h>
  friend class HandleManager;

  SharedHandle() : BaseShareHandle() {}

  //************using SFINAE to check if the incoming type is BaseShareHandle **********//
  typedef ObjType castType;
  template<typename T,
    typename = std::enable_if<
    std::is_same<BaseShareHandle, typename T::castType>::value>::type>
    explicit SharedHandle(const T& bsh) : BaseShareHandle(bsh) {}
  //* i have no easy way to do this so will stick to this for now *//

  ObjType* operator->() { return reinterpret_cast<ObjType*>(m_data[m_slot].m_baseHdl.GetPtr()); }
  ObjType const* operator->() const { return reinterpret_cast<const ObjType*>(m_data[m_slot].m_baseHdl.GetPtr()); }
private:
  SharedHandle(const HandleT& baseHdl) : BaseShareHandle(baseHdl) {}
};

typedef BaseShareHandle BaseHandle;

template<typename ObjType>
using Handle = SharedHandle<ObjType>;

// Handle can be anything, set to int for now
//struct Handle
//{
//public:
//  Handle(unsigned int id = ASC_INVALID_HANDLE);
//  //inline operator bool() const { return m_id == ASC_INVALID_HANDLE; };
//  //inline operator unsigned int() const { return m_id; };
//  inline unsigned& Val() { return m_id; }
//  inline unsigned Val() const { return m_id; }
//  inline bool IsValid() const { return m_id != ASC_INVALID_HANDLE; }
//
//  //these functions are for Handle manager
//  //use wisely as Handle will be invalidated
//  inline void InValidateHandle() { m_id = ASC_INVALID_HANDLE; }
//  inline int Iteration() const { return m_iteration; }
//
//  bool operator==(const Handle& hdl) { return m_iteration == hdl.m_iteration && m_id == hdl.m_id; }
//protected:
//  int m_iteration;
//  unsigned int m_id;
//};
/////////////////////////////////////////////////HANDLES STUFF////////////////////////////////////


