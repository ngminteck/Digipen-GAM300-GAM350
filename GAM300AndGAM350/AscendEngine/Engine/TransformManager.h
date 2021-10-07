/******************************************************************************/
/*!
\file   TransformManager.h
\author Zelin Ong
\par    email: zelin.ong\@digipen.edu
\par    Digipen login: zelin.ong
\date   02/02/2020
\brief
Contains definitions required for TransformManager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/


#ifndef TRANSFORM_MAN_H
#define TRANSFORM_MAN_H

//typedef Handle TransHandle;
typedef ComponentManagerBase<TransformComp>::COMPHANDLETYPE TransHandle;

#define ASC_TRANSMAN TransformManager::Inst()
class TransformManager : public ComponentManagerBase<TransformComp>, public Singleton<TransformManager>
{
  //inline Handle CloneComponent(const TransformComp& old) { return m_components.GetNewObject(old); }
public:
  TransformManager();
  ~TransformManager();

  //Virtual functions
  //void Load() override;
  void UnLoad() override;
  void Update(float dt) override;
  BaseHandle Deserialize(const GameObject &gameObject, Deserializer &) override;
  void Deserialize(TransformComp& tc, Deserializer const& d);
  void Serialize(Serializer &, BaseHandle) override;
  void Reflect(BaseHandle, const Reflector &) override;
  //TransHandle CloneComponent(TransHandle hdl) override;

  //TransHandle CreateComponent(const GameObject &gameObject) override;
  void DestroyComponent(BaseHandle hdl) override;
  //TransformComp* GetComponent(TransHandle index) override;
  //void InitComponent(const GameObject& go, TransHandle hdl) override;
  void CopyComponentToComponent(TransformComp& target, const TransformComp& source) override;

  void HandleMsg(MsgData_Base*) override {}

  /* parenting functions */
  void AttachChild(TransHandle parenthdl, TransHandle childhdl);
  void DetachFromParent(TransHandle childhdl);
  
  inline TransHandle GetParent(TransHandle childhdl) const                                                  { return GetParent(*m_components[childhdl]); }
  inline TransHandle GetParent(TransformComp const& childhdl) const                                    { return m_parentingObj[m_parentingObj[childhdl.parentinghdl]->m_parent]->m_trans; }
  /* no error checking */
  inline TransHandle GetChild(TransHandle parenthdl, int index) const                                       { return GetChild(*m_components[parenthdl], index); }
  inline TransHandle GetChild(TransformComp const& parenthdl, int index) const                         { return m_parentingObj[m_parentingObj[parenthdl.parentinghdl]->m_child[index]]->m_trans; }
  inline TransHandle FindChild(TransHandle parenthdl, std::string const& childname) const                   { return FindChild(*m_components[parenthdl], childname); }
  TransHandle FindChild(TransformComp const& parenthdl, std::string const& childname) const;

  /* parenting functions for ObjectFac */
  inline bool IsChild(pchdl childparentingHdl) const { return m_parentingObj[childparentingHdl]->m_parent.IsValid(); }
  inline TransHandle GetTransHdl(pchdl parentingHdl) const { return m_parentingObj[parentingHdl]->m_trans; }
  inline ChildIterate GetChildIter(pchdl parentingHdl) { return ChildIterate(*m_parentingObj[parentingHdl]); }

  /* for factory to use */
  void ManualTransformUpdate(GOHandle gohdl);

private:
  /* parenting stuff */
  void UpdateChildMatStart(pchdl parentinghdl);
  void UpdateChildMat(pchdl child, const TransformComp& parentT);
  //this function will assign a new parenting object if one does not
  //exist in trans
  inline bool AssignParentingObj(TransformComp &trans);
  inline void RemoveChildFromParent(ParentingObj& cobj);
  inline void AttachChildToParent(ParentingObj& cobj, ParentingObj& pobj);
  void AttachChild(TransformComp& parenthdl, TransformComp& childhdl);
  void DetachFromParent(TransformComp& childhdl);
  //inline void RemoveRoot(ParentingObj& obj);
  inline void AddToRoot(ParentingObj& obj);
  void MakeSubTreeConsistent(ParentingObj& obj);
  void RecursiveDestroy(ParentingObj& obj);
  HandleManager<pchdlWrapper> m_roots;
  HandleManager<ParentingObj> m_parentingObj;

  std::vector<Vec3> savepos;
  std::vector<std::string> savepos_str;

 
};

//helper
//for parenting
inline void AttachChild(GOHandle parenthdl, GOHandle childHandle)
{ 
  //ASC_TRANSMAN.AttachChild(ASC_GETCOMP_HANDLE(TRANSFORM, parenthdl), ASC_GETCOMP_HANDLE(TRANSFORM, childHandle));
  ASC_TRANSMAN.AttachChild(parenthdl->GetHdl<TransformComp>(), childHandle->GetHdl<TransformComp>());
}
inline void DetachFromParent(GOHandle childHandle)
{
  ASC_TRANSMAN.DetachFromParent(childHandle->GetHdl<TransformComp>());
}

inline GOHandle GetParent(GOHandle child)
{
  //TransformComp* trans = ASC_GETCOMP_PTR(TransformComp, child);
  Handle<TransformComp> phdl = ASC_TRANSMAN.GetParent(child->GetHdl<TransformComp>());
  GOHandle ret;
  if (phdl.IsValid()) ret = ASC_TRANSMAN.GetComponent(phdl)->GetGOHandle();

  return ret;
}

inline bool IsChild(GameObject& go)
{
  TransformComp* tc = ASC_TRANSMAN.GetComponent(go[TRANSFORM]);
  return tc->parentinghdl.IsValid() ? ASC_TRANSMAN.IsChild(tc->parentinghdl) : false;
}

inline bool IsChild(GOHandle child)
{
  return IsChild(*ASC_FACTORY.GetGameObjectPtr(child));
}

#endif