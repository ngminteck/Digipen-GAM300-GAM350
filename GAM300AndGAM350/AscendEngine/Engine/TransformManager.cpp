/******************************************************************************/
/*!
\file   TransformManager.cpp
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
#include "pch.h"

//cstring  name for the position scale...
static const char* PropertiesName[] =
{
  "Position",
  "Scale",
  "Rotation",
  "LocalPosition",
  "LocalScale",
  "LocalRotation",
};


TransformManager::TransformManager()
{
  //Transforms.reserve(100);
}

TransformManager::~TransformManager()
{
}

//void TransformManager::Load()
//{
//}
//
void TransformManager::UnLoad()
{
  m_roots.Clear();
  m_parentingObj.Clear();
  m_components.Clear();
}

void TransformManager::Update(float dt)
{
  start = Clock::now();

  if (m_components.m_objects.size() > 0)
  {
      for (auto& t : m_components.m_objects)
      {
          if (t.dirty)
          {
              if (t.parentinghdl.IsValid())
                  t.localmat = Mat4::ConcatModelMat(t.localpos, t.localscale, t.localrotation);
              else
                  t.mat = Mat4::ConcatModelMat(t.pos, t.scale, t.rotation);

              GOHandle gohandle = t.GetGOHandle();

              if (!gohandle.IsValid())
                  continue;
              NewPhyHandle nph = gohandle->GetHdl<NewPhysicsComp>();

#ifdef _RELEASE 
              if (!ASC_SYSTEM.hack_releasemode_submission)
              {
                  if (nph.IsValid())
                  {
                      nph->obj_phy->UpdateTransformInfo(&t);

                      if (nph->obj_phy->have_triggerbox)
                          nph->obj_trigger->UpdateTransformInfo(&t);
                  }
              }
#endif

          }
          t.dirty = false;
      }
  }

  //now for all the parented obj we need to concatenate the matrix
  for (pchdlWrapper& wrap : m_roots.m_objects)
  {
    pchdl pobj = wrap.m_root;
    UpdateChildMatStart(pobj);
  }

  duration = Clock::now() - start;
}


void TransformManager::UpdateChildMatStart(pchdl parentinghdl)
{
  TransformComp& t = *m_components[parentinghdl->m_trans];
  t.mat = Mat4::ConcatModelMat(t.pos, t.scale, t.rotation);
  //comp.mat = comp.localmat;
  for (pchdl& hdl : parentinghdl->m_child) UpdateChildMat(hdl, t);
}

void TransformManager::UpdateChildMat(pchdl child, const TransformComp& t)
{
  ParentingObj& pobj = *m_parentingObj[child];
  TransformComp& comp = *m_components[pobj.m_trans];
  comp.scale = Vec3(t.scale.x * comp.localscale.x,
                    t.scale.y * comp.localscale.y,
                    t.scale.z * comp.localscale.z);
  comp.rotation = t.rotation * comp.localrotation; //untested
  //need to rotate the position as well
  comp.pos = t.pos + t.rotation * (t.scale * comp.localpos);
  comp.mat = comp.localmat * t.mat;

  //GOHandle gohandle=  comp.GetGOHandle();
  //
  //NewPhyHandle nph = gohandle->GetHdl<NewPhysicsComp>();
  //
  //if (nph.IsValid())
  //    nph->UpdateTransformInfo(&comp);
  //
  for (pchdl& hdl : pobj.m_child)
  {
    UpdateChildMat(hdl, comp);
  }
}

void TransformManager::ManualTransformUpdate(GOHandle gohdl)
{
  TransHandle t = gohdl->GetHdl<TransformComp>();
  if (t->parentinghdl.IsValid())
    UpdateChildMatStart(t->parentinghdl);
  else
    t->mat = Mat4::ConcatModelMat(t->pos, t->scale, t->rotation);
}

BaseHandle TransformManager::Deserialize(const GameObject &gameObject, Deserializer &deserializer)
{
  TransHandle id(CreateComponent());
  TransformComp &tmp = *GetComponent(id);

  Deserialize(tmp, deserializer);

  return id;
}

void TransformManager::Deserialize(TransformComp & tc, Deserializer const& d)
{
  using namespace tinyxml2;
  Vec3* pos = &tc.pos;
  Vec3* scale = &tc.scale;
  Quaternion* rot = &tc.rotation;

  bool local = d.m_xmlElem.BoolAttribute("LOCAL");
  if (local)
  {
    pos = &tc.localpos;
    scale = &tc.localscale;
    rot = &tc.localrotation;
  }

  d.Read("Position", *pos);
  d.Read("Scale", *scale);
  d.Read("Rotation", *rot);
}

void TransformManager::Serialize(Serializer &serializer, BaseHandle index)
{
  //if (index >= Transforms.size() || Transforms.empty()) return;
  if (m_components.m_objects.empty()) return;

  // Get the current transform to serialize
  TransformComp &transform = *GetComponent(index);

  //need to check if we should write from local or world
  Vec3* pos = &transform.pos;
  Vec3* scale = &transform.scale;
  Quaternion* rot = &transform.rotation;
  if (transform.parentinghdl.IsValid())
  {
    if (m_parentingObj[transform.parentinghdl]->m_parent.IsValid())
    {
      serializer.m_xmlElem.SetAttribute("LOCAL", true);
      pos = &transform.localpos;
      scale = &transform.localscale;
      rot = &transform.localrotation;
    }
  }

  serializer.Write("Position", *pos);
  serializer.Write("Scale", *scale);
  serializer.Write("Rotation", *rot);
}



void TransformManager::Reflect(BaseHandle id, const Reflector &reflector)
{
  static bool ClipBoard = false;
  static const char* cbtitle = "Clipboard";
  //0 is original, else parented
  int offset = 0;
  TransformComp &transform = *GetComponent(id);

  //for parented objects
  if (transform.parentinghdl.IsValid())
  {
    ParentingObj* pObj = m_parentingObj[transform.parentinghdl];
    //only if m_parent is valid means this obj is a child 
    //so we show the local stuff
    if (pObj->m_parent.IsValid())
    {
      offset = 3;
      //give the option to remove game object
      if (ImGui::Button("Remove Child"))
      {
        DetachFromParent(transform);
      }
      else
      {
        GOHandle pgo = m_components[GetParent(transform)]->GetGOHandle();
        ImGui::Text("Parent: %s", ASC_GETGO_PTR(pgo)->name.c_str());
      }
    }
  }

  Vec3& pos = offset ? transform.localpos : transform.pos;
  Vec3& scale = offset ? transform.localscale : transform.scale;
  Quaternion& rot = offset ? transform.localrotation : transform.rotation;

  transform.dirty |= reflector.Reflect(PropertiesName[offset], pos);
  if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
  {
    ImGui::OpenPopup(cbtitle);
  }
  transform.dirty |= reflector.Reflect(PropertiesName[offset + 1], scale);

  Vec3 euler = QuaternionToEuler(rot);
  bool rotated = false;

  rotated |= reflector.Reflect(PropertiesName[offset + 2], euler);
  //rotated |= reflector.Reflect("Quaternion", transform.rotation);

  if (rotated)
  {
    //transform.rotation = EulerToQuaternion(euler.yaw, euler.pitch, euler.roll);
    rot = EulerToQuaternion(euler);
    transform.dirty = true;
  }

  //popup stuff
  if (ImGui::BeginPopup(cbtitle))
  {
    if (ImGui::MenuItem("Copy to clipboard"))
    {
      savepos.push_back(pos);
      char buffer[256];
      memset(buffer, 0, 256);
      sprintf(buffer, "%d: %f %f %f", (int)savepos.size() - 1, pos.x, pos.y, pos.z);
      savepos_str.push_back(std::string(buffer));
    }
    if (ImGui::MenuItem("Clear Clipboard"))
    {
      savepos.clear();
      savepos_str.clear();
    }

    if (ImGui::BeginMenu("position", savepos.size() != 0))
    {
      int popid = 0;
      for (Vec3& v : savepos)
      {
        ImGui::PushID(popid);
        if (ImGui::MenuItem(savepos_str[popid].c_str()))
        {
          pos = v;
        }
        ImGui::PopID();
        ++popid;
      }

      ImGui::EndMenu();
    }

    ImGui::EndPopup();
  }
}

//Handle TransformManager::CreateComponent(const GameObject &gameObject)
//{
//  //TransformComp transform;
//  //transform.dirty = true;
//  //transform.pos = Vec3(0.0f, 0.0f, 0.0f);
//  //transform.scale = Vec3(1.0f, 1.0f, 1.0f);
//  //transform.rotation = Quaternion();
//
//  //Transforms.push_back(transform);
//  //return static_cast<int>(Transforms.size() - 1);
//  return m_transforms.GetNewObject(TransformComp());
//}
//
void TransformManager::DestroyComponent(BaseHandle hdl)
{
  //check if parenting is involve and check if current component is the root
  TransHandle thdl(hdl);
  TransformComp& t = *m_components[thdl];
  if (t.parentinghdl.IsValid())
  {
    ParentingObj& pobj = *m_parentingObj[t.parentinghdl];
    //check if parenting is root
    if (m_roots[pobj.m_root]->m_root == pobj.GetSelfHandle())
    {
      //release from root
      m_roots.ReleaseObj(pobj.m_root);
    }
    else
    {
      //it definitely has a parent //detach from parent
      RemoveChildFromParent(pobj);
    }
    //destroy all child obj
    RecursiveDestroy(pobj);
    m_parentingObj.ReleaseObj(t.parentinghdl);
  }

  m_components.ReleaseObj(thdl);
}
//
//TransformComp* TransformManager::GetComponent(Handle index)
//{
//  //return index.Val() < Transforms.size() ? &Transforms[index.Val()] : nullptr;
//  return m_transforms.GetObj(index);
//}

//Handle TransformManager::CloneComponent(Handle hdl)
//{
//  Handle newhdl = m_components.GetNewObject();
//  CopyComponentToComponent(*m_components[newhdl], *m_components[hdl]);
//  return newhdl;
//}

void TransformManager::CopyComponentToComponent(TransformComp & target, const TransformComp & source)
{
  target = source;
  target.parentinghdl.AssignNewInValid();
}

void TransformManager::AttachChild(TransHandle parenthdl, TransHandle childhdl)
{
  //check if parent is parenting valid
  //check if child is parenting valid before continue
  TransformComp &pattrans = *GetComponent(parenthdl);
  TransformComp &childtrans = *GetComponent(childhdl);
  AttachChild(pattrans, childtrans);
}

void TransformManager::AttachChild(TransformComp & parenthdl, TransformComp & childhdl)
{
  //do not attach urself
  if (parenthdl.GetSelfHandle() == childhdl.GetSelfHandle()) return;

  bool pAssigned = AssignParentingObj(parenthdl);
  bool cAssigned = AssignParentingObj(childhdl);

  //if child has parenting already
  ParentingObj& patpobj = *m_parentingObj[parenthdl.parentinghdl];
  ParentingObj& childpobj = *m_parentingObj[childhdl.parentinghdl];
  //check if the child pobj has parent anot
  if (childpobj.m_parent.IsValid())
  {
    //we need to detach the current parent the child is attached to
    RemoveChildFromParent(childpobj);
  }
  //make parenthdl a root in m_roots if it does not have one yet
  if (!patpobj.m_root.IsValid())
  {
    AddToRoot(patpobj);
  }
  //child does not have a parent now
  //and reattached it to the new parent
  AttachChildToParent(childpobj, patpobj);

  //and reset all child m_root to be the pcWraphdl of parenthdl
  MakeSubTreeConsistent(childpobj);
}

void TransformManager::DetachFromParent(TransHandle childhdl)
{
  //check if the transform obj is parenting valid anot
  //since if it is not valid how to detach?
  TransformComp &childtrans = *GetComponent(childhdl);
  DetachFromParent(childtrans);
}

void TransformManager::DetachFromParent(TransformComp & childhdl)
{
  if (childhdl.parentinghdl.IsValid())
  {
    ParentingObj& childpobj = *m_parentingObj[childhdl.parentinghdl];
    //check if the child pobj has parent anot
    if (childpobj.m_parent.IsValid())
    {
      //we need to detach the current parent the child is attached to
      RemoveChildFromParent(childpobj);
      //check if child has children
      if (!childpobj.m_child.size())
      {
        //if it has no children, we can remove it as a parenting obj
        m_parentingObj.ReleaseObj(childpobj.GetSelfHandle());
      }
      else
      {
        //else if it has children, make it a root
        AddToRoot(childpobj);
      }
    }
  }
}

//inline Handle TransformManager::GetParent(Handle childhdl) const
//{
//  return GetParent(*m_components[childhdl]);
//}

//inline Handle TransformManager::GetParent(TransformComp const& childhdl) const
//{
//  return m_parentingObj[childhdl.parentinghdl]->m_parent;
//}

/* no error checking */
//inline Handle TransformManager::GetChild(Handle parenthdl, int index) const
//{
//  return GetChild(*m_components[parenthdl], index);
//}

//inline Handle TransformManager::GetChild(TransformComp const& parenthdl, int index) const
//{
//  /* this is so bad....need to change this implementation asap during holiday */
//  pchdl cphdl = m_parentingObj[parenthdl.parentinghdl]->m_child[index];
//  return m_parentingObj[cphdl]->m_trans;
//}

/* expensive operations */
//inline Handle TransformManager::FindChild(Handle parenthdl, std::string const & childname) const
//{
//  return FindChild(*m_components[parenthdl], childname);
//}

TransHandle TransformManager::FindChild(TransformComp const& parenthdl, std::string const & childname) const
{
  TransHandle transHdl;
  const ParentingObj& cphdl = *m_parentingObj[parenthdl.parentinghdl];
  for (pchdl chdl : cphdl.m_child)
  {
    //need to find the name from the game object
    TransHandle ctranshdl = m_parentingObj[chdl]->m_trans;
    GOHandle ghdl = m_components[ctranshdl]->GetGOHandle();
    if (ASC_FACTORY.GetGameObjectPtr(ghdl)->name == childname)
    {
      transHdl = ctranshdl;
      break;
    }
  }
  return transHdl;
}

//inline Handle TransformManager::GetTransHdl(pchdl parentingHdl) const
//{
//  return m_parentingObj[parentingHdl]->m_trans;
//}
//
//inline ChildIterate TransformManager::GetChildIter(pchdl parentingHdl)
//{
//  return ChildIterate(*m_parentingObj[parentingHdl]);
//}


inline bool TransformManager::AssignParentingObj(TransformComp &trans)
{
  if (!trans.parentinghdl.IsValid())
  {
    trans.parentinghdl = m_parentingObj.GetNewObject();
    m_parentingObj[trans.parentinghdl]->m_trans = trans.GetSelfHandle();
    return true;
  }
  return false;
}

inline void TransformManager::RemoveChildFromParent(ParentingObj& cobj)
{
  pchdl childHdl = cobj.GetSelfHandle();
  std::vector<pchdl>& childList = m_parentingObj[cobj.m_parent]->m_child;
  size_t sz = childList.size();
  for (size_t i = 0; i < sz; ++i)
  {
    if (childList[i] == childHdl)
    {
      //replace with last obj and break
      childList[i] = childList.back();
      childList.pop_back();
      break;
    }
  }
  //make it parentless 
  cobj.m_parent.AssignNewInValid();
}

/*
This function will help attached a child to a parent and also check if the child is a root
remove root if necessary
*/
inline void TransformManager::AttachChildToParent(ParentingObj & cobj, ParentingObj & pobj)
{
  //push back the child handle onto parent
  pobj.m_child.push_back(cobj.GetSelfHandle());
  //set the respective stuff of child
  cobj.m_parent = pobj.GetSelfHandle();
  //we need to check if child is a root first
  if (cobj.m_root.IsValid() && 
    m_roots[cobj.m_root]->m_root == cobj.GetSelfHandle())
  {
    //remove root
    m_roots.ReleaseObj(cobj.m_root);
  }
  cobj.m_root = pobj.m_root;
}

inline void TransformManager::AddToRoot(ParentingObj & obj)
{
  pcWrapHdl roothdl = m_roots.GetNewObject();
  pchdlWrapper& root = *m_roots[roothdl];
  root.m_root = obj.GetSelfHandle();
  obj.m_root = roothdl;
}

void TransformManager::MakeSubTreeConsistent(ParentingObj & obj)
{
  std::vector<pchdl>& childList = obj.m_child;
  for (pchdl& chdl : childList)
  {
    ParentingObj& cobj = *m_parentingObj[chdl];
    cobj.m_root = obj.m_root;
    MakeSubTreeConsistent(cobj);
  }
}

void TransformManager::RecursiveDestroy(ParentingObj & obj)
{
  for (pchdl& chdl : obj.m_child)
  {
    ParentingObj& cobj = *m_parentingObj[chdl];
    RecursiveDestroy(cobj);
    //call upon the factory obj destroy
    ASC_FACTORY.DestroyObject(m_components[m_parentingObj[chdl]->m_trans]->GetGOHandle());
    m_parentingObj.ReleaseObj(chdl);
  }
}

//inline void TransformManager::RemoveRoot(ParentingObj & obj)
//{
//  m_roots.ReleaseObj(obj.m_root);
//}

//void TransformManager::InitComponent(const GameObject & go, Handle hdl)
//{
//  m_components[hdl]->SetGOHandle(go.GetSelfHandle());
//}


