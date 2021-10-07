/******************************************************************************/
/*!
\file   ObjectFactory.h
\author Zelin Ong
\par    email: zelin.ong\@digipen.edu
\par    Digipen login: zelin.ong
\date   02/02/2020
\brief
Contains definitions required for ObjectFactory

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef OBJECT_FACTORY_H
#define OBJECT_FACTORY_H

#define ASC_FACTORY ObjectFactory::Inst()
#define ASC_GETCOMP_HANDLE(COMPONENT_TYPE_ENUM, GO_Handle) (*ASC_FACTORY.GetGameObjectPtr(GO_Handle))[COMPONENT_TYPE_ENUM]
#define ASC_GETGO_PTR(GOhandle) ASC_FACTORY.GetGameObjectPtr(GOhandle)
//#define ASC_GETCOMP_PTR(Comp_TYPE, GO_PTR) ASC_FACTORY.GetComponent<Comp_TYPE>(GO_PTR->GetSelfHandle())
#define ASC_GETCOMP_PTR(Comp_CLASS, GO_Handle) ASC_FACTORY.GetComponentPtr<Comp_CLASS>(GO_Handle)
#define ASC_GETCOMP_HDL(Comp_CLASS, GO_Handle) GO_Handle>GetHdl<Comp_CLASS>()

struct ObjTypeItr
{
  const GOHandle& Next(); //irreversible; cannot get back prev elem
  const GOHandle& Peak(bool peakElem = true);
  size_t GetSizeTotal() const;
  size_t GetSizeCurrent() const;

  size_t contIdx = 0;
  size_t elemIdx = 0;
  GOHandle invalid; //must be invalid; return this when no more elem

  void ResetIdx(); //call this to start from first elem
  void ClearObj(); //reset this obj to its default state(if you wamt to reuse ObjTypeItr instead of reconstructing a new one to replace old one)

//private:
  size_t size = 0;
  size_t curr_size = 0;
  std::vector<const std::vector<GOHandle>*> m_data;
};


class ObjectFactory : public Singleton<ObjectFactory>
{
public:
  ObjectFactory(std::unordered_map<COMPONENT_TYPE, I_Manager *> &managers);

  void HandleMsg(MsgData_Base * msg);
  //scenes load/ save
  void LoadScene(const char *file, bool unloadCurr = true);
  void SaveScene(const char *file);
  void UnloadScene();

  void Reflect(GameObject &gameObject, const Reflector &reflector);
  void Update();

  GameObject &AddObject();
  void DestroyObject(GOHandle hdl);
  BaseHandle AddComponent(GameObject &obj, COMPONENT_TYPE type);
  //both remove component does the same thing
  void RemoveComponent(GameObject &obj, COMPONENT_TYPE type);
  inline void RemoveComponent(GOHandle hdl, COMPONENT_TYPE type) { RemoveComponent(*m_gameObjects[hdl], type); }
  //both remove all component does the same thing
  void RemoveAllComponent(GameObject &obj);
  inline void RemoveAllComponent(GOHandle hdl) { RemoveAllComponent(*m_gameObjects[hdl]); };
  void InitAllComponent(const GameObject &obj);
  inline void InitAllComponent(GOHandle hdl) { InitAllComponent(*m_gameObjects[hdl]); };

  template<typename ... Args>
  void AddComponent(GameObject &obj, COMPONENT_TYPE type, Args && ... args)
  {
    AddComponent(obj, type);
    AddComponent(obj, args...);
  }

  //user using this will need to pass in GOHandle
  template<typename compType>
  compType* GetComponentPtr(GOHandle goindex);

  //this function returns a reference to the list of GOHandle of the same type
  const std::vector<GOHandle>& GetObjectHandleList(GOTYPE gotype);
  void GetObjectHandleList(std::vector<GOTYPE> input_types, std::vector<GOHandle>& output);
  void GetObjectHandleList(ObjTypeItr& output, GOTYPE gotype);
  void GetObjectHandleList(ObjTypeItr& output, const std::vector<GOTYPE>& input_types);
  //template<typename T>
  void GetObjectHandleList(ObjTypeItr& output, std::initializer_list<GOTYPE> l)//GOTYPE gotype, GOTYPE ... args)
  {
    for(GOTYPE got : l)
      GetObjectHandleList(output, got);
    //GetObjectHandleList(output, args...);
  }

  GOHandle GetGameObjectByName(std::string name);


  //this function returns a reference to the list of gameobjects
  std::vector<GameObject>& GetGameObjectList();
  inline const GameObject* GetGameObjectPtr(GOHandle hdl) const { return m_gameObjects[hdl]; };
  inline GameObject* GetGameObjectPtr(GOHandle hdl) { return m_gameObjects[hdl]; };

  void GetGameObjectListsWithTag(std::string key, std::vector<GameObject *>& store_container);

  void LoadPrefab(const char* name);
  void SavePrefab(GameObject* go);
  //mainly for prefab only but anyone should be able to use
  GOHandle CloneGO(GOHandle goh);
  GOHandle CreateGOfromPB(const std::string& pbname);
  void ReloadPB(const std::string& pbname);
  void ReloadAllPB();
  struct PrefabMan
  {
    //void AddGameObjToMap(GameObject go);
    PrefabMan() { m_prefabs.reserve(500); }
    //std::unordered_map<std::string, PBGameObject> m_prefabs;
    std::unordered_map<std::string, XMLfile> m_prefabs;
  } pbman;
  //prefab end

  //object type interface
  void SetGOType(GameObject& go, GOTYPE);
  inline void SetGOType(GOHandle hdl, GOTYPE gotype) { GameObject& go = *GetGameObjectPtr(hdl); SetGOType(go, gotype); }

private:
  std::unordered_map<COMPONENT_TYPE, I_Manager *> &m_managers;
  //std::vector<GameObject> m_gameObjects;
  HandleManager<GameObject> m_gameObjects;
  std::vector<GOHandle> m_GOhandleToBeDestroy;



  void DestroyGOList(std::vector<GOHandle>& hdllist);

  //for storing a list of common objects
  void AddGOToTypeList(GameObject& go);
  bool RemoveGOFromList(GameObject& go);
  std::unordered_map<GOTYPE, std::vector<GOHandle>> m_objectTypeList;

  //for parenting
  void SavePrefabParenting(GameObject* child, tinyxml2::XMLElement* xmlparentelem, int parent, int& index);
  int LoadChildObj(tinyxml2::XMLElement* &xmlcurr, GOHandle callerHdl, int callerIndex, GOHandle& initChild);
  inline GOHandle DeserializeGO(tinyxml2::XMLElement* goxmlelm, const char* name, bool initnow = true, bool useAsPbName = false);
  inline bool LoadCheckParenting(tinyxml2::XMLElement* goxmlelm, GOHandle roothdl);
  inline bool SaveCheckParenting(tinyxml2::XMLElement* goxmlelm, GameObject const& root);
  std::vector<GOHandle> m_parentingTmpList;

  //for cloning 
  GOHandle CloneGO(const GameObject& old);

  //for prefab loading
  //prefab load/ save
  GOHandle LoadGOFromPBFile(const char* name, const char* ext = ASC_PREFAB_EXT_DOT);
  GOHandle LoadGOFromPBFile(XMLfile& file, std::string const& pbname);

  bool reloadTextures;
  void ReloadTextures();
};



//template<COMPONENT_TYPE COMP>
//inline Component * ObjectFactory::GetComponent(int goindex)
//{
//  GameObject* go = &m_gameObjects[goindex];
//  int idx = go->GetComponent<COMP>();
//  return idx >= 0 ? m_managers[COMP]->GetComponent(idx) : nullptr;
//}

template<typename compType>
inline compType * ObjectFactory::GetComponentPtr(GOHandle goindex)
{
  GameObject* go = m_gameObjects.GetObj(goindex);
  BaseHandle& compidx = go->GetComponent<compType::CompEnum>();
  //return reinterpret_cast<compType*>(idx >= 0 ? m_managers[compType::CompEnum]->GetComponent(idx) : nullptr);
  return reinterpret_cast<compType*>(m_managers[compType::CompEnum]->GetComponent(compidx));
}


#endif