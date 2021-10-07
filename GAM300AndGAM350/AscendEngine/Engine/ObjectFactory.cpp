/******************************************************************************/
/*!
  \file   ObjectFactory.cpp
  \author Zi He Goh
  \par    email: zihe.goh\@digipen.edu
  \par    Digipen login: zihe.goh
  \date   02/02/2020
  \brief

  Copyright (C) 2020 DigiPen Institute of Technology.
  Reproduction or disclosure of this file or its contents without the
  prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

#define GO_NAME "NAME"
#define GO_TYPE "TYPE"

#define USETILEON 1

ObjectFactory::ObjectFactory(std::unordered_map<COMPONENT_TYPE, I_Manager*>& managers)
  : m_managers(managers), reloadTextures(true)
  //, debugf("DEBUGFILESEE.txt")
  //hexgrid
  //,m_hgrid(15, 9, .5, {0.f, -1.f})
{
  //m_hgrid.Setname("Testgrid");
  m_parentingTmpList.reserve(1000);
  ASC_MSGSYS.RegisterCallBack(mTYPE_PLAY, &ObjectFactory::HandleMsg, this);
}

void ObjectFactory::HandleMsg(MsgData_Base * msg)
{
  switch (msg->type)
  {
  case mTYPE_PLAY:
  {
    ReloadAllPB();
  }break;
  default:
  {
  }
  }
}

void GiveMapOfPrefab(std::unordered_map<std::string, bool>& strmap)
{
  std::vector<std::string> filenamesVec = GetFilesInDir(ASC_PREFABS_FLD, "xml");
  for (std::string& name : filenamesVec)
  {
    //take away the extension
    name.resize(name.size() - 4);
    strmap.insert({ name, true });
  }
}

void ObjectFactory::LoadScene(const char *file, bool unloadCurr)
{
    if (unloadCurr)
    {
        UnloadScene();
    }

    std::cout << "odeserialize" << std::endl;
  ASC_NEWPHYSICSMANAGER.InitODESerialize();

  std::unordered_map<std::string, bool> pbfilenames;
  GiveMapOfPrefab(pbfilenames);

  using namespace tinyxml2;
  tinyxml2::XMLDocument xmlDoc;
  tinyxml2::XMLError result = xmlDoc.LoadFile(file);

  std::string status = "Sucessful";
  if (result != tinyxml2::XML_SUCCESS)
  {
    status = tinyxml2::XMLDocument::ErrorIDToName(result);
    // ERROR
    LogError("Error xml load : " + std::string(file) + "\n\t\t" + status);
    //ASC_MSGSYS.SendMsg(Msg_Error(std::move(status)));
  }
  else
  {
    //ASC_MSGSYS.SendMsg(Msg_Error(std::move(status)));
  }

  XMLElement *scene = xmlDoc.FirstChildElement("SCENE");
  if (scene)
  {
    XMLElement *go = scene->FirstChildElement();
    while (go)
    {
      const char* goname = go->Attribute(GO_NAME);
      const char* pbname = go->Attribute("PREFAB");
      bool isRoot = go->BoolAttribute("PARENT");

      //DeserializeGO(go, goname);
      GOHandle gohdl;
      //if we need to load pb and found it in files as well
      if (pbname && pbfilenames.find(pbname) != pbfilenames.end())
      {
        gohdl = LoadGOFromPBFile(pbname);
        //need to set name
        GameObject* goptr = m_gameObjects[gohdl];
        goptr->name = goname;
        //load partial
        XMLElement* transXML = go->FirstChildElement(compStrMap[TRANSFORM]);
        TransformComp tmp;
        ASC_TRANSMAN.Deserialize(tmp, Deserializer(*transXML));
        //you would want to read the transform and overwrite
        TransformComp* gotrans = ASC_GETCOMP_PTR(TransformComp, gohdl);
        gotrans->pos = tmp.pos;
        gotrans->scale = tmp.scale;
        gotrans->rotation = tmp.rotation;
      }
      else
      {
        //not using prefab therefore deserialize normally
#ifdef USETILEON
        gohdl = DeserializeGO(go, goname, !isRoot);
#else
        gohdl = DeserializeGO(go, goname);// , !isRoot);
#endif
        if (isRoot)
        {
          //not using prefab and is a parent in the world
          LoadCheckParenting(go, gohdl);
          ASC_TRANSMAN.ManualTransformUpdate(gohdl);
        }
      }

      //skip the parenting as we are done with it
      if (isRoot) go = go->NextSiblingElement();

      go = go->NextSiblingElement();
    }
  }

  ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_SCENE_LOADED));
}

void ObjectFactory::SaveScene(const char *file)
{
  static const std::string BLANK("");
  tinyxml2::XMLDocument xmlDoc;
  tinyxml2::XMLElement *elem = xmlDoc.NewElement("SCENE");
  xmlDoc.InsertFirstChild(elem);

  for (auto &go : m_gameObjects.m_objects)
  {
    //don't save parented stuff as the root will save them
    if (IsChild(go)) continue;
    //dont save tower_top
    if (compare_str(go.name, "Tower_top")) continue;

    tinyxml2::XMLElement *goElem = xmlDoc.NewElement("GO");
    goElem->SetAttribute(GO_NAME, go.name.c_str());
    goElem->SetAttribute(GO_TYPE, GOTP::GOTypeStrMap[go.m_type]);
    if (go.m_prefab_name != BLANK)
      goElem->SetAttribute("PREFAB", go.m_prefab_name.c_str());

    //Get components and call their serialize function
    for (auto &component : go.components)
    {
      tinyxml2::XMLElement *comElem = xmlDoc.NewElement(compStrMap[component.first]);
      Serializer serializer(*comElem);
      m_managers[component.first]->Serialize(serializer, component.second);
      goElem->InsertEndChild(comElem);
    }

    elem->InsertEndChild(goElem);
    //the reason why we put it behind Insert is so that it is the right order
    if (SaveCheckParenting(goElem, go))
      goElem->SetAttribute("PARENT", true);
  }

  std::string path(ASC_SCENES_FLD);
  path += std::string(file);
  Log("Save Path : ", path);
  xmlDoc.SaveFile(path.data());
}

void ObjectFactory::UnloadScene()
{
  //DestroyGOList(m_GOhandleToBeDestroy);
  m_GOhandleToBeDestroy.clear();
  m_gameObjects.Clear();
  m_objectTypeList.clear();
  for (auto &elem : m_managers)
  {
    elem.second->UnLoad();
  }
  

}

GOHandle ObjectFactory::GetGameObjectByName(std::string name)
{
    GOHandle retObj;

    for (auto it = m_gameObjects.m_objects.begin(); it != m_gameObjects.m_objects.end(); ++it)
    {
        if (name.compare(it->name) == 0)
        {
            retObj = it->GetSelfHandle();
            break;
        }
    }
    return retObj;
}

std::vector<GameObject>& ObjectFactory::GetGameObjectList()
{
  return m_gameObjects.m_objects;
}

GOHandle ObjectFactory::LoadGOFromPBFile(const char * pbname, const char* ext)
{
  XMLfile file(std::string(ASC_PREFABS_FLD) + pbname + ext, true);
  if (!file.GetDocFirstElement())
  {
    LogError(std::string(pbname) + ext + " File is Empty!");
    return GOHandle();
  }

  return LoadGOFromPBFile(file, pbname);
}

/*
  mainly used for creating prefab as a shortcut
*/
GOHandle ObjectFactory::LoadGOFromPBFile(XMLfile& file, const std::string& pbname)
{
  //if (compare_str(pbname, "TowerRange"))
  //{
  //  bool a = (pbname == "Bullet_Basic");
  //  int b = 1;
  //}
  //LogWarning("Debug ",pbname);
  using namespace tinyxml2;
  XMLElement* go = file.GetDocFirstElement();
  GOHandle tmpHandle = DeserializeGO(go, pbname.c_str(), false, true);

  //do parenting
  if (!LoadCheckParenting(go, tmpHandle))
    InitAllComponent(tmpHandle);
#ifndef USETILEON
  InitAllComponent(tmpHandle);
#endif

  ASC_TRANSMAN.ManualTransformUpdate(tmpHandle);
  reloadTextures = true;

  return tmpHandle;
}

void ObjectFactory::ReloadTextures()
{
  if (reloadTextures)
  {
    m_managers[GRAPHICS]->Load();
    reloadTextures = false;
  }
}

//it will return true if there is parenting
bool ObjectFactory::LoadCheckParenting(tinyxml2::XMLElement* goxmlelm, GOHandle roothdl)
{
  using namespace tinyxml2;
  XMLElement* parentstart = goxmlelm->NextSiblingElement("PARENTING");

  //due to us allowing creation of other objects in onStart of script, we have to make sure the
  //loading of parent is correct
#ifdef USETILEON
  int startIdx = static_cast<int>(m_parentingTmpList.size());
#else
  int startIdx = 0;
#endif
  //debugf << static_cast<int>(m_parentingTmpList.size()) << std::endl;
  if (parentstart)
  {
    //we have a parenting object at hand 
    //starting with first object
    XMLElement* xmlchild = parentstart->FirstChildElement();
    // this is so if the recursive function return back we know the child is suppose to attach
    // to this root
    GOHandle childhdl;
    m_parentingTmpList.push_back(roothdl);
    while (xmlchild)
    {
      //get parenting index info
      int currNodeI = xmlchild->IntAttribute("INDEX");
      int parentI = xmlchild->IntAttribute("PARENT");
      GOHandle currGohdl = DeserializeGO(xmlchild, xmlchild->Attribute(GO_NAME), false);
      m_parentingTmpList.push_back(currGohdl);

      AttachChild(m_parentingTmpList[startIdx + parentI], currGohdl);
      xmlchild = xmlchild->NextSiblingElement();
    }
  }

  //debugf << m_parentingTmpList.size() << "\n";
  //now then we inti starting from the bottom of the parent list
#ifdef USETILEON
  for (int i = static_cast<int>(m_parentingTmpList.size()); i > startIdx; --i)
  {
    //InitAllComponent from the back;
    InitAllComponent(m_parentingTmpList.back());
    m_parentingTmpList.pop_back();
  }
#else
  for (int i = static_cast<int>(m_parentingTmpList.size() - 1); i > 0; --i)
  {
    InitAllComponent(m_parentingTmpList[i]);
  }
  m_parentingTmpList.clear();
#endif
  return (parentstart != nullptr);
}


void ObjectFactory::GetGameObjectListsWithTag(std::string key, std::vector<GameObject *>& store_container)
{
    for (auto it = m_gameObjects.m_objects.begin(); it != m_gameObjects.m_objects.end(); ++it)
    {
        TagHandle hdl = it->GetHdl<TagComp>();

        if (hdl.IsValid())
        {
            if (hdl->tag_container.find(key) != hdl->tag_container.end())
            {
                store_container.push_back(&(*it));
            }
        }
    }
}

///////////////////// start of prefab ///////////////////////////////////
void ObjectFactory::LoadPrefab(const char * name)
{
  pbman.m_prefabs.emplace(name, std::make_pair(std::string(ASC_PREFABS_FLD) + name + ASC_PREFAB_EXT_DOT, true));
  Log(name, "pb xmlfile loaded");
}

void ObjectFactory::SavePrefab(GameObject * go)
{
  using namespace tinyxml2;
  XMLfileWrite file(ASC_PREFABS_FLD + go->name + "." + ASC_PREFAB_EXT, "GO");
  //file.m_curr_node->SetAttribute(GO_NAME, go->name.c_str());
  file.m_curr_node->SetAttribute(GO_TYPE, GOTP::GOTypeStrMap[go->m_type]);

  //Get components and call their serialize function
  for (auto &component : go->components)
  {
    XMLElement *comElem = file.AddAsChild(compStrMap[component.first], true);
    Serializer serializer(*comElem);
    m_managers[component.first]->Serialize(serializer, component.second);
    //goElem->InsertEndChild(comElem);
  }

  SaveCheckParenting(file.m_curr_node, *go);
  // Note: clear the prefabs stored in prefabman to trigger a reload
  //pbman.m_prefabs.erase(go->name);
  //LoadPrefab(go->name.c_str());
  Log("SavePrefab: ", go->name);

  //set the go name to the prefab name as well
  go->m_prefab_name = go->name;
  //do a reload on the prefabman
  ReloadPB(go->name);
}

bool ObjectFactory::SaveCheckParenting(tinyxml2::XMLElement* goxmlelm, GameObject const& go)
{
  using namespace tinyxml2;
  TransformComp* trans = ASC_TRANSMAN.GetComponent(go[TRANSFORM]);
  bool parenting = trans->parentinghdl.IsValid();
  if (parenting)
  {
    int index = 0;
    //XMLElement* parentingElem = file.m_curr_node->GetDocument()->NewElement("PARENTING");
    XMLElement* parentingElem = CreateAndInsertEndChild(goxmlelm->Parent(), "PARENTING");
    ChildIterate ci = ASC_TRANSMAN.GetChildIter(trans->parentinghdl);
    while (ci.Next().IsValid())
    {
      GameObject* nextChild = ASC_GETGO_PTR(ASC_TRANSMAN.GetComponent(ci.currTransHdl)->GetGOHandle());
      SavePrefabParenting(nextChild, parentingElem, 0, index);
    }
    //SavePrefabParenting(go, parentingElem, 0, index);
    parentingElem->SetAttribute("NUM", index);
    return true;
  }
  return false;
}

void ObjectFactory::SavePrefabParenting(GameObject * child, tinyxml2::XMLElement * xmlparentelem, int parent, int & index)
{
  using namespace tinyxml2;
  int currNum = ++index;
  XMLElement* meElem = CreateAndInsertEndChild(xmlparentelem, "CHILD_GO");
  meElem->SetAttribute(GO_NAME, child->name.c_str());
  meElem->SetAttribute(GO_TYPE, GOTP::GOTypeStrMap[child->m_type]);
  meElem->SetAttribute("INDEX", currNum);
  meElem->SetAttribute("PARENT", parent);

  //Get components and call their serialize function
  for (auto &component : child->components)
  {
    XMLElement *comElem = CreateAndInsertEndChild(meElem, compStrMap[component.first]);
    Serializer serializer(*comElem);
    m_managers[component.first]->Serialize(serializer, component.second);
    //goElem->InsertEndChild(comElem);
  }

  //get transform to get the parenting info
  TransformComp* trans = ASC_TRANSMAN.GetComponent((*child)[TRANSFORM]);
  ChildIterate ci = ASC_TRANSMAN.GetChildIter(trans->parentinghdl);
  while (ci.Next().IsValid())
  {
    GameObject* nextChild = ASC_GETGO_PTR(ASC_TRANSMAN.GetComponent(ci.currTransHdl)->GetGOHandle());
    SavePrefabParenting(nextChild, xmlparentelem, currNum, index);
  }
}

/* if return int is  */
int ObjectFactory::LoadChildObj(tinyxml2::XMLElement *& xmlcurr, GOHandle callerHdl, int callerIndex, GOHandle& initChild)
{
  using namespace tinyxml2;
  //get parenting index info
  int currNodeI = xmlcurr->IntAttribute("INDEX");
  int parentI = xmlcurr->IntAttribute("PARENT");
  GOHandle currGohdl = DeserializeGO(xmlcurr, xmlcurr->Attribute(GO_NAME));
  //attach to parent if index matches else past it up
  if (parentI == callerIndex)
  {
    AttachChild(callerHdl, currGohdl);
    //get next elem
    xmlcurr = xmlcurr->NextSiblingElement();
    //continue trying calling other child if have
    while (xmlcurr)
    {
      int ret = LoadChildObj(xmlcurr, currGohdl, currNodeI, initChild);
      if (ret == currNodeI)
        AttachChild(currGohdl, initChild);
      else
        return ret;

      if (xmlcurr) xmlcurr = xmlcurr->NextSiblingElement();
    }
  }
  else
  {
    initChild = currGohdl;
    return parentI;
  }

  return -1;
}

GOHandle ObjectFactory::DeserializeGO(tinyxml2::XMLElement * goxmlelm, const char* name, bool initnow, bool useAsPbName)
{
  using namespace tinyxml2;
  XMLElement* go = goxmlelm;
  GOHandle tmpHandle = m_gameObjects.GetNewObject();
  GameObject& gameObject = *m_gameObjects.GetObj(tmpHandle);
  gameObject.name = name;
  if (useAsPbName) gameObject.m_prefab_name = name;
  // go->Attribute(GO_NAME);
                           //gameObject.name = go->Attribute("name");
                           //get the type attribute
  const char* tpstr = go->Attribute(GO_TYPE);
  gameObject.m_type = tpstr ? GOTP::strGOTypeMap[tpstr] : go_default;

  XMLElement *com = go->FirstChildElement();
  while (com)
  {
    const char *componentName = com->Name();
    COMPONENT_TYPE type = strCompMap[componentName];
    Deserializer deserializer(*com);
    BaseHandle componentIndex = m_managers[type]->Deserialize(gameObject, deserializer);
    gameObject.AddComponent(type, componentIndex);

    com = com->NextSiblingElement();
  }

  if (initnow) InitAllComponent(gameObject);
  //InitAllComponent(gameObject);
  //add to the respective type containers
  AddGOToTypeList(gameObject);

  return tmpHandle;
}


GOHandle ObjectFactory::CloneGO(GOHandle goh)
{
  return CloneGO(*m_gameObjects[goh]);
}


GOHandle ObjectFactory::CreateGOfromPB(const std::string & pbname)
{
  auto& pblist = pbman.m_prefabs;
  if (pblist.find(pbname) == pblist.end())
    LoadPrefab(pbname.c_str());

  //return GOHandle();
  return LoadGOFromPBFile(pblist.find(pbname)->second, pbname);
}

void ObjectFactory::ReloadPB(const std::string & pbname)
{
  pbman.m_prefabs.erase(pbname);
  LoadPrefab(pbname.c_str());
}

void ObjectFactory::ReloadAllPB()
{
  pbman.m_prefabs.clear();
  //get the list of prefab names from dir
  std::vector<std::string> filenamesVec = GetFilesInDir(ASC_PREFABS_FLD, "xml");
  for (std::string& str : filenamesVec)
  {
    LoadPrefab(std::string(str.begin(), str.end()-4).c_str());
  }
}


void ObjectFactory::SetGOType(GameObject & go, GOTYPE newType)
{
  //need to remove the object handle from the respective list
  RemoveGOFromList(go);
  //now change the game object type and 
  go.m_type = newType;
  //add its handle to the new respective list
  AddGOToTypeList(go);
}

void ObjectFactory::DestroyGOList(std::vector<GOHandle>& hdllist)
{
  //there is a high possibility that new obj to be deleted is added in during deletion
  for (size_t i = 0; i < hdllist.size(); ++i)
    //for (GOHandle hdl : hdllist)
  {
    GOHandle hdl = hdllist[i];
    //this is so if someone added duplicated hdl we will know
    if (!hdl.IsValid()) continue;
    // Let all system know obj is being destroyed like editor to deselect selected obj
    ASC_MSGSYS.SendMsg(Msg_Logic_OnCollide(mTYPE_OBJ_DESTROYED, hdl, hdl));

    GameObject& goref = *m_gameObjects[hdl];

    //we need to try to remove script component first as that will
    //be the one that runs certain logic that has dependency on
    //other components
    BaseHandle scriptHdl = goref[COMPONENT_TYPE::LOGIC];
    if (scriptHdl.IsValid())
    {
      m_managers[COMPONENT_TYPE::LOGIC]->DestroyComponent(scriptHdl);
      goref.components.erase(COMPONENT_TYPE::LOGIC);
    }

    RemoveAllComponent(goref);
    RemoveGOFromList(goref);
    m_gameObjects.ReleaseObj(hdl);
  }

  //handle are all invalid so clear them
  hdllist.clear();
}

void ObjectFactory::AddGOToTypeList(GameObject& go)
{
  GOHandle gohdl = go.GetSelfHandle();
  GOTYPE gotype = go.m_type;

  //check if the vector of the respective object exist
  if (m_objectTypeList.find(gotype) != m_objectTypeList.end())
  {
    m_objectTypeList.at(gotype).push_back(gohdl);
  }
  //vector not found make a new one
  else
  {
    std::vector<GOHandle> temp;
    temp.reserve(1000);
    temp.push_back(gohdl);
    m_objectTypeList.insert({ gotype, std::move(temp) });
  }

}

//function will return false if removal is unsuccessful
bool ObjectFactory::RemoveGOFromList(GameObject & go)
{
  auto pairitr = m_objectTypeList.find(go.GetType());
  //found the list
  if (pairitr != m_objectTypeList.end())
  {
    GOHandle gohdl = go.GetSelfHandle();
    std::vector<GOHandle>& vec = pairitr->second;
    //check if list contains object handle
    //for now we just loop through the whole vector
    size_t size = vec.size();
    int foundIndex = -1;
    for (size_t i = 0; i < size; ++i)
    {
      if (vec[i] == gohdl)
      {
        foundIndex = static_cast<int>(i);
        break;
      }
    }

    if (foundIndex != -1)
    {
      //replace the found slot with the last element of the vec
      vec[foundIndex] = vec.back();
      //and reduce the total element by 1
      vec.pop_back();
      return true;
    }
  }

  //removal fail
  return false;
}


GOHandle ObjectFactory::CloneGO(const GameObject & old)
{
  GOHandle newhdl = m_gameObjects.GetNewObject();
  GameObject* newGO = m_gameObjects[newhdl];
  newGO->name = old.name;
  newGO->m_prefab_name = old.m_prefab_name;
  SetGOType(*newGO, old.m_type);
  for (auto& comppair : old.components)
  {
    BaseHandle newcomphdl = m_managers[comppair.first]->CloneComponent(comppair.second);
    //m_managers[type]->GetComponent(component)->SetGOHandle(go.GetSelfHandle());
    newGO->AddComponent(comppair.first, newcomphdl);
  }

  InitAllComponent(*newGO);

  //damn...we need to check for parented objs
  TransformComp* oldtrans = ASC_TRANSMAN.GetComponent(old[TRANSFORM]);
  bool parenting = oldtrans->parentinghdl.IsValid();
  if (parenting)
  {
    //get transform to get the parenting info
    //TransformComp* trans = ASC_TRANSMAN.GetComponent(old[TRANSFORM]);
    ChildIterate ci = ASC_TRANSMAN.GetChildIter(oldtrans->parentinghdl);
    while (ci.Next().IsValid())
    {
      GameObject* nextChild = ASC_GETGO_PTR(ASC_TRANSMAN.GetComponent(ci.currTransHdl)->GetGOHandle());
      GOHandle newChild = CloneGO(*nextChild);
      AttachChild(newhdl, newChild);
    }
  }
  return newhdl;
}

///////////////////// end of prefab ///////////////////////////////////

void ObjectFactory::Reflect(GameObject &gameObject, const Reflector &reflector)
{
  static bool popup = false;
  static bool PbnameEdit = false;
  static COMPONENT_TYPE ct;

  if (!gameObject.GetSelfHandle().IsValid())
    return;

  //set is active 
  bool val =gameObject.GetIsEnable();
  if (ImGui::Checkbox("##gameObjectEnable", &val))
  {
    gameObject.SetEnable(val);
  
  }
 
  if (PbnameEdit)
  {
    if (reflector.Reflect("Prefab", gameObject.m_prefab_name))
      PbnameEdit = false;
    //if (PbnameEdit && ImGui::Button("Done")) PbnameEdit = false;
  }
  else
  {
    ImGui::Text("Prefab: %s", gameObject.m_prefab_name.c_str());
    ImGui::SameLine();
    if (ImGui::Button("Edit")) PbnameEdit = true;
    ImGui::SameLine();
    if (ImGui::Button("X")) gameObject.m_prefab_name = "";
  }

  reflector.Reflect(gameObject.name);
  ASC_TWEEN.Reflect();

  //combo box for go type
  int type_select = GOTYPE::go_default;
  auto iter = GOTP::GOTypeStrMap.find(gameObject.m_type);
  if (iter != GOTP::GOTypeStrMap.end())
    type_select = (int)std::distance(GOTP::GOTypeStrMap.begin(), iter);
  ImGui::Combo("GO_Type", &type_select, GOTP::GOtypeList.data(), (int)GOTP::GOtypeList.size());
  //gameObject.m_type = GOTP::strGOTypeMap[GOTP::GOtypeList[type_select]];
  SetGOType(gameObject, GOTP::strGOTypeMap[GOTP::GOtypeList[type_select]]);
  //

  unsigned int i = 0;
  for (const auto& elem : gameObject.components)
  {
    if (ImGui::CollapsingHeader(compStrMap[elem.first], ImGuiTreeNodeFlags_DefaultOpen))
    {
      ASC_EDITOR.ItemHoveredToolTip("Right Click for more");

      if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
      {
        ct = elem.first;
        popup = true;
      }

      ImGui::PushID(i++);
      m_managers[elem.first]->Reflect(elem.second, reflector);
      ImGui::PopID();
    }
  }

  if (popup)
  {
    ImGui::OpenPopup("Options");
    popup = false;
  }
  if (ImGui::BeginPopup("Options"))
  {
    if (ImGui::MenuItem("Delete"))//, NULL, false, false))
    {
      //delete component
      RemoveComponent(gameObject, ct);
    }
    ImGui::EndPopup();
  }
}

void ObjectFactory::Update()
{
  //delay gameobject destruction
  ReloadTextures();
  DestroyGOList(m_GOhandleToBeDestroy);
}

//this function will help in creation of an unknown obj return back its handle
//GOHandle ObjectFactory::GetGOHandle()
//{
//  GOHandle newhand;
//  //get a unused handle
//  if (m_unusedGOHandle.size())
//  {
//    newhand = m_unusedGOHandle.back();
//    m_unusedGOHandle.pop_back();
//  }
//  //get a new handle
//  else
//  {
//    newhand = static_cast<GOHandle>(m_gameObjects.size());
//    //wait....use the handleMan
//  }
//
//  return newhand;
//}

GameObject &ObjectFactory::AddObject()
{
  GOHandle i = m_gameObjects.GetNewObject();//(int)m_gameObjects.size();
  //m_gameObjects.push_back(GameObject());
  GameObject &go = *m_gameObjects.GetObj(i);
  //go.SetSelfHandle(i);
  AddComponent(go, TRANSFORM);

  //add to the type list
  AddGOToTypeList(go);
  return go;
}

void ObjectFactory::DestroyObject(GOHandle hdl)
{
  //delay destroy action
  m_GOhandleToBeDestroy.push_back(hdl);
}

BaseHandle ObjectFactory::AddComponent(GameObject &go, COMPONENT_TYPE type)
{
  BaseHandle component = m_managers[type]->CreateComponent();
  go.AddComponent(type, component);

  //initialize the component
  //m_managers[type]->GetComponent(component)->SetGOHandle(go.GetSelfHandle()); 
  m_managers[type]->InitComponent(go, component);

  return component;
}

void ObjectFactory::RemoveComponent(GameObject & obj, COMPONENT_TYPE type)
{
  BaseHandle hdl = obj[type];
  if (hdl.IsValid())
  {
    m_managers[type]->DestroyComponent(hdl);
    obj.components.erase(type);
  }
}

void ObjectFactory::RemoveAllComponent(GameObject & obj)
{
  for (auto& keypair : obj.components)
  {
    m_managers[keypair.first]->DestroyComponent(keypair.second);
  }

  obj.components.clear();
}

void ObjectFactory::InitAllComponent(const GameObject & obj)
{
  for (auto& compPr : obj.components)
    m_managers[compPr.first]->InitComponent(obj, compPr.second);
}

const std::vector<GOHandle>& ObjectFactory::GetObjectHandleList(GOTYPE gotype)
{
  //check if the vector of the respective object exist
  //vector not found make a new one
  if (m_objectTypeList.find(gotype) == m_objectTypeList.end())
  {
    std::vector<GOHandle> temp;
    temp.reserve(1000);
    m_objectTypeList.insert({ gotype, std::move(temp) });
  }

  return m_objectTypeList[gotype];
}

void ObjectFactory::GetObjectHandleList(std::vector<GOTYPE> input_types, std::vector<GOHandle>& output)
{
  for (auto & type : input_types)
  {
    const std::vector<GOHandle>& tmp = GetObjectHandleList(type);
    std::copy(tmp.begin(), tmp.end(), back_inserter(output));
  }
}

void ObjectFactory::GetObjectHandleList(ObjTypeItr & output, GOTYPE gotype)
{
  const std::vector<GOHandle>& tmp = GetObjectHandleList(gotype);
  if (!tmp.size())
    return;
  output.m_data.push_back(&tmp);
  output.size += tmp.size();
}

void ObjectFactory::GetObjectHandleList(ObjTypeItr & output, const std::vector<GOTYPE>& input_types)
{
  for (auto& type : input_types)
  {
    GetObjectHandleList(output, type);
  }
}

const GOHandle & ObjTypeItr::Next()
{
  return Peak(false);
}

const GOHandle & ObjTypeItr::Peak(bool peakElem)
{
  if (!m_data.size() || curr_size == size)
  {
    ResetIdx();
    return invalid;
  }

  if (peakElem == false)
  {
    const std::vector<GOHandle>& current_cont = (*m_data[contIdx]);
    size_t current_cont_size = current_cont.size();

    if (elemIdx + 1 > current_cont_size)
    {
      ++contIdx;
      elemIdx = 0;

      size_t totalCont = m_data.size();

      const std::vector<GOHandle>& cont = (*m_data[contIdx]);
      ++curr_size;
      return cont[elemIdx++];
    }


    const GOHandle& hdl = current_cont[elemIdx++];
    assert(curr_size <= size);
    ++curr_size;
    return hdl;
  }
  else
  {
    const std::vector<GOHandle>& current_cont = (*m_data[contIdx]);
    size_t current_cont_size = current_cont.size();

    if (elemIdx + 1 > current_cont_size)
    {
      //contIdx++;
      //elemIdx = 0;

      size_t totalCont = m_data.size();

      if (contIdx >= totalCont)
      {
        return invalid;
      }

      const std::vector<GOHandle>& cont = (*m_data[contIdx + 1]);
      //++curr_size;
      return cont[0];
    }


    const GOHandle& hdl = current_cont[elemIdx + 1];
    //assert(curr_size <= size);
    //++curr_size;
    return hdl;
  }
}

size_t ObjTypeItr::GetSizeTotal() const
{
  return size;
}

size_t ObjTypeItr::GetSizeCurrent() const
{
  return curr_size;
}

void ObjTypeItr::ResetIdx()
{
  size = curr_size = contIdx = elemIdx = 0;
}

void ObjTypeItr::ClearObj()
{
  m_data.clear();
  ResetIdx();
}
