/******************************************************************************/
/*!
\file   Serialization.h
\author Goh Zihe
\par    email: zihe.goh\@digipen.edu
\par    Digipen login: zihe.goh
\date   02/02/2020
\brief
Contains definitions required for Serializer & deserializer

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#ifndef SERIALIZER_H
#define SERIALIZER_H

enum MsgType; // forward declaration

class XMLfile
{
public:
  //To load a file at the start set loading parameter to true
  XMLfile(const std::string& filename, bool loading);
  XMLfile(const std::pair<const std::string&, bool>& pair);
  virtual ~XMLfile();

  ///////////////////member functions/////////////////////
  tinyxml2::XMLError Load();
  //If file is not saved yet, it will save during the destruction of this object
  tinyxml2::XMLError Save();
  //will return a nullptr if file is empty
  //else will return the first node and it will be assign to m_curr_node as well
  tinyxml2::XMLElement* GetDocFirstElement();

  tinyxml2::XMLDocument m_xmlDoc;
  //during the parsing
  tinyxml2::XMLElement* m_curr_node;
protected:
  bool Saved = false;
  std::string m_filename;
};

class XMLfileWrite : public XMLfile
{
public:
  //this will add a new node from the start
  XMLfileWrite(const std::string& filename, const std::string& startNodeName);

  //fundemental adding node operations
  //after adding a new node as child to the current node child will become current node
  //unless remainAsParent is specified
  tinyxml2::XMLElement* AddAsChild(const char* elmname, bool remainAsParent = false);
  //after adding of sibling to the current node the sibling will become the current node
  tinyxml2::XMLElement* AddAsSibiling(const char* elmname);
};

class Serializer
{
public:
  Serializer(tinyxml2::XMLElement &xmlElem);
  void Write(const std::string &data);
  void Write(const char *elemName, const Vec2 &vec);
  void Write(const char *elemName, const Vec3 &vec);
  void Write(const char *elemName, const Vec4 &vec);
  void Write(const char *elemName, const Quaternion &vec);
  void Write(const char *elemName, const std::string &name);
  void Write(const char *elemName, std::vector<std::string> &vec);
  //NOTE IT SAVES NAME NOT ADDRRESS
  //void Write(const char* elemName, std::vector<GameObject>& vec);
  void Write(const char *elemName, int val);
  void Write(const char *elemName, bool val);
  void Write(const char *elemName, float val);
  void Write(const char* elemName, double val);
  void Write(const char* elemName, std::string key, std::string value);
  void Write(const char *elemName, std::unordered_map<std::string, std::string> &vec);
  void Write(const char* elemName, std::unordered_map<int , std::string>& vec);
  void Write(const char* elemName, std::map<std::string, bool>& vec);
  void Write(const char* elemName, std::vector<float>& vec);
  void Write(const char* elemName, std::vector<dReal>& vec);
  void Write(const char* elemName, std::vector<dTriIndex>& vec);
  void Write(const char* elemName, std::unordered_map < std::string, MsgType>&);


  void Append(const Vec2 &vec);
  void Append(const Vec3 &vec);
  void Append(const Vec4 &vec);
  void Append(const Quaternion &vec);
  void Append(const std::string &name);
  void Append(std::vector<std::string> &vec);
  void Append(int val);
  void Append(bool val);
  void Append(float val);
  void Append(double val);
  void Append(std::unordered_map<std::string, std::string> &vec);
  void Append(const std::string& key, const std::string& value);
  void EndAppend();

  tinyxml2::XMLElement& CreateNewChild(const char* nodename);
  tinyxml2::XMLElement &m_xmlElem;

private:
  tinyxml2::XMLDocument &m_xmlDoc;
  tinyxml2::XMLElement *lastElement;
};

class Deserializer
{
public:
  Deserializer(tinyxml2::XMLElement &xmlElem);
  void Read(const char *elemName, Vec2 &vec) const;
  void Read(const char *elemName, Vec3 &vec) const;
  void Read(const char *elemName, Vec4 &vec) const;
  void Read(const char *elemName, Quaternion &vec) const;
  void Read(const char *elemName, std::string &name) const;
  void Read(const char *elemName, int & val) const;
  void Read(const char *elemName, bool & val) const;
  void Read(const char *elemName, float & val) const;
  void Read(const char* elemName, double& val) const;


  void Read(const char *elemName, std::unordered_map<std::string, std::string> &vec) const;
  void Read(const char* elemName, std::unordered_map<int, std::string>& vec) const;

  void Read(const char* elemName, std::map < std::string, bool>&) const; 

  void Read(const char* elemName,std::vector<float>& ) const;
  void Read(const char* elemName, std::vector<dReal>&) const;
  void Read(const char* elemName,std::vector<dTriIndex>& ) const;
  void Read(const char* elemName, std::unordered_map < std::string, MsgType>&)const;
  void Read(const char* elemName, std::vector<std::string >&) const;

  template<typename T>
  void Read(const char *elemName, std::vector<T> &vec) const;

  tinyxml2::XMLElement &m_xmlElem;

private:
  template<typename T>
  void InternalRead(tinyxml2::XMLElement *xmlElem, const char *attribute, T &val) const
  {
    if (xmlElem)
    {
      const char *c = xmlElem->Attribute(attribute);
      if (c)
      {
        val = ToType<T>(c);
        return;
      }
    }
    LogError("Error reading attribute: ", attribute, " from XML");
  }

  template<typename T>
  T ToType(const char *str) const
  {
    return str;
  }

  template<>
  bool ToType<bool>(const char *str) const
  {
    return (str != nullptr);
  }

  template<>
  float ToType<float>(const char *str) const
  {
    return strtof(str, nullptr);
  }


  template<>
  double ToType<double>(const char* str) const
  {
      return strtod(str, nullptr);
  }

 // template<>
 // dReal ToType<dReal>(const char* str) const
 // {
 //     return strtod(str, nullptr);
 // }
 //
};

class Reflector
{
  static const float speed;

public:

  bool Reflect(std::string &) const;
  bool Reflect(const char *name, std::string &k, bool = true) const;
  bool Reflect(const char* name, std::string& k, std::vector<std::string>  const& source, bool = true) const;

  //bool Reflect(const char* name, std::string const& k, bool = true) const;
  bool Reflect(const char *name, bool &) const;
  bool Reflect(const char *name) const;               //button

  bool Reflect(const char *name, int &) const;
  bool Reflect(const char *name, int &, int, int) const;

  bool Reflect(const char *name, float &, int precision = 5) const;
  bool Reflect(const char *name, float &, float, float, float dragSpeed = speed) const;

  //bool Reflect(const char *name, Vec2 &, int precision = 2) const;
  bool Reflect(const char *name, Vec2 &, float = -FLT_MAX, float = FLT_MAX) const;

  //bool Reflect(const char *name, Vec3 &, int precision = 2) const;
  bool Reflect(const char *name, Vec3 &, float = -FLT_MAX, float = FLT_MAX) const;
  static bool ColorPicker(const char* name, float *color, bool alpha = true);

  bool Reflect(const char *name, Vec4 &vec) const;
  bool Reflect(const char *name, Vec4 &vec, float, float) const;

  bool Reflect(const char *name, Quaternion &vec) const;
  
  bool Reflect(const char* name, std::vector<std::string>& vec) const;
 // bool Reflect(const char* name, std::vector<std::string> const & vec) const;
  //with combo list added 
  bool Reflect(const char* name, std::vector<std::string>& vec, std::vector<std::string>  const & source , bool removeExtant=true) const;
  bool Reflect(const char* name, std::unordered_map< std::string,std::string>& vec, std::vector<std::string> const & source, bool removeExtant = true) const;

  bool Reflect(const char *name, std::vector<std::string> &vec, int *index) const;
  bool Reflect(const char* name, std::vector<std::string> const & vec, int* index) const;
  bool Reflect(const char *name, std::vector<const char*> &vec, int *index) const;
  static bool Reflect(const char *name, unsigned);
};

template<typename T>
inline void Deserializer::Read(const char * elemName, std::vector<T>& vec) const
{
    tinyxml2::XMLElement *elem = m_xmlElem.FirstChildElement(elemName);
    while (elem)
    {
      T obj;
      InternalRead(elem, "name", obj);
      vec.push_back(obj);
      elem = elem->NextSiblingElement();
    }
}

/*helper*/
inline tinyxml2::XMLElement* CreateAndInsertEndChild(tinyxml2::XMLNode* parent, const char* name)
{
  tinyxml2::XMLElement* child = parent->GetDocument()->NewElement(name);
  parent->InsertEndChild(child);
  return child;
}

#endif