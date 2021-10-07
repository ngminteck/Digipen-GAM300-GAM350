/******************************************************************************/
/*!
\file   BaseManager.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions for Singleton and I_Manager

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef AE_BASEMANAGER_H
#define AE_BASEMANAGER_H

template<typename T>
class Singleton
{
public:
  Singleton() { obj = static_cast<T*>(this); }
  virtual ~Singleton() { obj = nullptr; }
  static T& Inst() { return *obj; }

private:
  static T *obj;
};

template<typename T> T* Singleton<T>::obj = nullptr;

struct MsgData_Base;
class GameObject;
struct BaseComponent;

//for prefab
typedef size_t PBHandle;

struct I_Manager
{
  I_Manager() {}
  virtual ~I_Manager() {}

  virtual void Load() = 0;
  virtual void UnLoad() = 0;

  virtual void Update(float dt) = 0;

  virtual BaseHandle Deserialize(const GameObject &gameObject, Deserializer &) = 0;
  virtual void Serialize(Serializer &, BaseHandle) = 0;
  virtual void Reflect(BaseHandle, const Reflector &) = 0; 
  virtual BaseHandle CloneComponent(BaseHandle) = 0;

  virtual BaseHandle CreateComponent() = 0;
  virtual void DestroyComponent(BaseHandle hdl) = 0;
  virtual BaseComponent* GetComponent(BaseHandle index) = 0;
  virtual void InitComponent(const GameObject& go, BaseHandle hdl) = 0;

  virtual PBHandle CreatePrefab() = 0;
  virtual PBHandle CreatePrefabFromComp(BaseHandle) = 0;
  virtual BaseComponent* GetPrefab(PBHandle) = 0;
  virtual BaseHandle CreateComponentFromPrefab(const GameObject& go, PBHandle) = 0;

  //For Msging
  virtual void HandleMsg(MsgData_Base *) = 0;

  typedef std::chrono::high_resolution_clock Clock;
  Clock::time_point start;
  std::chrono::duration<float, std::milli> duration;


};

/*
for light usage such as no need for all object to be together at all times
LW = lightweight
does no error checking
I found that there might be error during destruction of objects during destruction of
vector at the end of this contianer....so this is very bad please do not use it
*/
template<typename ObjType, typename HanType = unsigned>
class LWhandleMan
{
  std::vector<HanType> m_unused;
public:
  //the actual objects
  std::vector<ObjType> m_objects;

  //this function destroy all objects and handles
  inline void Clear() { m_unused.clear(); m_objects.clear(); }

  template<typename ...Args>
  inline HanType CreateObj(Args&&... args)
  {
    HanType hdl;
    if (m_unused.size())
    {
      hdl = m_unused.back();
      m_unused.pop_back();
      //need to construct in place
      new (&m_objects[hdl]) ObjType(std::forward<Args>(args)...);
    }
    else
    {
      hdl = static_cast<HanType>(m_objects.size());
      m_objects.emplace_back(std::forward<Args>(args)...);
    }

    return hdl;
  }

  inline const ObjType& GetObj(HanType handle) const { return m_objects[handle]; }
  inline ObjType& GetObj(HanType handle) 
  { return const_cast<ObjType&>(static_cast<const LWhandleMan<ObjType, HanType>&>(*this).GetObj(handle)); }
  inline const ObjType& operator[](HanType handle) const { return GetObj(handle); }
  inline ObjType& operator[](HanType handle) { return GetObj(handle); }

  inline void ReleaseObj(HanType handle)
  {
    m_unused.push_back(handle);
    m_objects[handle].~ObjType();
  }
};

/*
a class that takes the last obj and replace the deleted one
*/
template<typename T>
struct vectorRPB
{
  std::vector<T> m_data;
  template<typename ...Args>
  inline size_t emplace_back(Args&&... args)
  {
    m_data.emplace_back(std::forward<Args>(args)...);
    //returning the index
    return m_data.size() - 1;
  }

  inline const T& operator[](size_t index) const { return m_data[index]; }
  inline T& operator[](size_t index) { return m_data[index]; }
  inline void erase(size_t index)
  {
    std::swap(m_data[index], m_data.back());
    m_data.pop_back();
  }

  //special functions for only T obj with variable m_index in them
  template<typename ...Args>
  inline void emplace_back_i(Args&&... args)
  {
    size_t index = emplace_back(std::forward<Args>(args)...);
    m_data.back().m_index = index;
  }
  inline void erase_i(size_t index)
  {
    erase(index);
    m_data[index].m_index = index;
  }
};

/*
for HandleManager
there is potential that iteration might overflow but the chances of old 
handle that exist that matches the overflow is slim
*/
template<typename ObjType>
struct HandleManCheck : public BasicHandle
{
  HandleManCheck(bool isUsing = false, int id = -1, int iteration = 0) :
    BasicHandle(id), m_isUsing(isUsing) {
    m_iteration = iteration;
  }
  bool m_isUsing;
  inline void SetVal(unsigned id) { m_id = id; }
  inline void SetObjPtr(ObjType* ptr) { BasicHandle::SetObjPtr(ptr); }
};

/*
generic class that holds manage the handles and its underlying objects
note:
HanType is handle Type which can actual only used by a Handle class
but because i wanted the intellisense to show any type that mimics Handle
to show as its type instead of Handle
*/
template<typename ObjType>//, typename HanType = Handle>
class HandleManager
{
public:
  typedef SharedHandle<ObjType> HanType;
  typedef HandleManCheck<ObjType> HanCheckType;

  //the actual objects
  std::vector<ObjType> m_objects;

  //methods
  HandleManager(int reserve = MEMSZ);
  //this function destroy all objects and handles
  void Clear();

  //this will create a new object in the manager and return a handle
  //for now this function will help add the handle to the respective
  //object but then it might be removed as user have to make sure the function
  //SetSelfHandle must be available 
  template<typename ...Args>
  HanType GetNewObject(Args&&... args);
  HanType MakeCopy(HanType handle);

  inline const ObjType* GetObj(HanType handle) const;
  inline ObjType* GetObj(HanType handle);
  inline const ObjType* operator[](HanType handle) const { return GetObj(handle); }
  inline ObjType* operator[](HanType handle) { return GetObj(handle); }
  void ReleaseObj(HanType handle);

  void ErrCheck(HanType Handle) const;
private:
  //members
  //std::vector<int> m_objectshandle;
  std::vector<HanCheckType> m_handles;
  std::vector<unsigned> m_unused;

  size_t m_objCap;
  void CheckVecGrow();
};

/*
 Generic templated base manager that contains the common definitions
 new manager can inherit from this and will handle handle manager
 unless user override for more specialized interface
*/
//typedef TransformComp COMPTYPE;
template<typename COMPTYPE, int reserve = MEMSZ>
struct ComponentManagerBase : public I_Manager
{
  std::vector<COMPTYPE> m_pbcomponents;
  HandleManager<COMPTYPE> m_components;
public:
  typedef HandleManager<COMPTYPE> HanManType;
  typedef typename HanManType::HanType COMPHANDLETYPE;
  typedef BaseHandle HANDLETYPE;

  ComponentManagerBase() : m_components(reserve) 
  {
    m_pbcomponents.reserve(reserve);
  }

  virtual void Load() override {}
  virtual void UnLoad() override { m_components.Clear(); }

  virtual HANDLETYPE CreateComponent() override;
  virtual void DestroyComponent(HANDLETYPE hdl) override;
  virtual COMPTYPE* GetComponent(HANDLETYPE hdl) override;
  virtual void InitComponent(const GameObject& go, HANDLETYPE hdl) override;
  virtual HANDLETYPE CloneComponent(HANDLETYPE hdl) override;

  //prefab interface
  virtual PBHandle CreatePrefab() override;
  virtual PBHandle CreatePrefabFromComp(HANDLETYPE) override;
  virtual BaseComponent* GetPrefab(PBHandle) override;
  virtual HANDLETYPE CreateComponentFromPrefab(const GameObject& go, PBHandle) override;
  //this function every manager needs to implement as it is a deep copy
  virtual void CopyComponentToComponent(COMPTYPE& target, const COMPTYPE& source) = 0;

  //etc
  bool isActive = true;
  void SetIsActive(bool val) { isActive = val; }
  bool GetIsActive() { return isActive; }

};


//////////////////////////////////////////HANDLE MAN DEFINITIONS///////////////////////////////////////////

template<typename ObjType>//, typename HanType>
inline HandleManager<ObjType>::HandleManager(int reserve)
{
  m_objects.reserve(reserve);
  m_handles.reserve(reserve);
  m_unused.reserve(reserve / 2);

  m_objCap = m_objects.capacity();
}

template<typename ObjType>//, typename HanType>
inline void HandleManager<ObjType>::Clear()
{
  m_handles.clear();
  m_unused.clear();

  for (ObjType& obj : m_objects)
  {
    HanType hdl = obj.GetSelfHandle();
    hdl.InValidateHandle();
  }
  m_objects.clear();
}

/*
place the correct args for the new objects unless not required
*/
template<typename ObjType>//, typename HanType>
template<typename ...Args>
inline typename HandleManager<ObjType>::HanType
HandleManager<ObjType>::GetNewObject(Args && ...args)
{
  unsigned handle;
  HanCheckType hmc;
  if (m_unused.size())
  {
    //get a handle from unused
    handle = m_unused.back();
    //unused size is reduced by one
    m_unused.pop_back();
    //get the index of the potential last obj in m_objects
    hmc = HanCheckType(true, static_cast<int>(m_objects.size()), m_handles[handle].Iteration() + 1);
    m_handles[handle] = hmc;
    //the current m_id in hmc is the index to the actual object but it needs to store the handle
    //represented by the index for m_handles
    hmc.SetVal(handle);
    m_objects.emplace_back(std::forward<Args>(args)...);
  }
  //no unused handle to use
  else
  {
    handle = static_cast<unsigned>(m_handles.size());
    hmc = HanCheckType(true, static_cast<int>(m_objects.size()), 1);
    m_handles.push_back(hmc);
    //the current m_id in hmc is the index to the actual object but it needs to store the handle
    //represented by the index for m_handles
    hmc.SetVal(handle);
    m_objects.emplace_back(std::forward<Args>(args)...);
  }

  //this function call will require ObjType to have the function in order to work
  //this is to set the new handle val
  ObjType& curr_obj = m_objects[m_handles[handle].Val()];
  //set the address of the ptr into the handle
  hmc.SetObjPtr(&curr_obj);
  //when setting of self handle the SharedHandle will create a new slot
  curr_obj.SetSelfHandle(hmc);

  //this is so the handle can maintain their validity if vector grows
  //future implementation probably should not make it grow "fixed size"
  CheckVecGrow();

  //so to not create an other slot we have to get the handle from the object itself
  return curr_obj.GetSelfHandle();
}

template<typename ObjType>//, typename HanType>
inline typename HandleManager<ObjType>::HanType
HandleManager<ObjType>::MakeCopy(HanType handle)
{
  //ObjType& source = GetObj(handle);
  //HanType ret = GetNewObject(GetObj(handle));
  return GetNewObject(*GetObj(handle));
}

/*
new thinking:
lets just error check
old thinking:
after an object is return, the handle will no longer be valid
and this function does not check for whether the handle is valid
user is suppose to make sure it is valid
*/
template<typename ObjType>//, typename HanType>
inline const ObjType * HandleManager<ObjType>::GetObj(HanType handle) const
{
  //error check
#ifndef _SUBMISSION
  ErrCheck(handle);
#endif
  return &m_objects[m_handles[handle.Val()].Val()];
}


template<typename ObjType>//, typename HanType>
inline ObjType* HandleManager<ObjType>::GetObj(HanType handle)
{
  //error check
  //ErrCheck(handle);

  //return &m_objects[m_handles[handle.Val()].Val()];
  return const_cast<ObjType*>(static_cast<const HandleManager<ObjType>&>(*this).GetObj(handle));
  //error check
}


/*
user is suppose to make sure that the handle is valid else..nvm
it will check for error
*/
template<typename ObjType>//, typename HanType>
inline void HandleManager<ObjType>::ReleaseObj(HanType handle)
{
#ifndef _SUBMISSION
  ErrCheck(handle);
#endif
  //turn off using of current slot
  HanCheckType& oldindex = m_handles[handle.Val()];
  m_handles[handle.Val()].m_isUsing = false;
  //and replace the object with the last element
  //update the index as well of the gameobject in m_handles since it is going to be moving
  m_handles[m_objects.back().GetSelfHandle().Val()].SetVal(oldindex.Val());
  //m_objects[oldindex.Val()] = std::move(m_objects.back());
  ObjType& swapping_obj = m_objects[oldindex.Val()];
  std::swap(m_objects[oldindex.Val()], m_objects.back());

  //set the ptr within the handle in the swapped obj to the new address
  //have to do this first as it might swap with itself "killing me..."
  HanType swphdl = swapping_obj.GetSelfHandle();
  swphdl.GetBasicHandle().SetObjPtr(&swapping_obj);

  m_objects.pop_back();
  //throw it to used list
  m_unused.push_back(handle.Val());
  //make the current handle invalidate
  handle.InValidateHandle();
}

template<typename ObjType>//, typename HanType>
inline void HandleManager<ObjType>::ErrCheck(HanType handle) const
{
  //check for out of range and is the slot used or not
  //assert(static_cast<size_t>(handle) < m_handles.size());
  //check for out of range and is the slot used or not
#ifndef _SUBMISSION
  assert(m_handles[handle.Val()].m_isUsing);
  //check if iteration is the wrong one
  assert(m_handles[handle.Val()].Iteration() == handle.Iteration());
#endif
}

template<typename ObjType>
inline void HandleManager<ObjType>::CheckVecGrow()
{
  if (m_objCap != m_objects.capacity())
  {
    //reassign all handles
    for (ObjType& obj : m_objects)
    {
      HanType tmphdl = obj.GetSelfHandle();
      tmphdl.GetBasicHandle().SetObjPtr(&obj);
    }
    m_objCap = m_objects.capacity();
  }
}

////////////////////////////MANAGERBASE DEFINITIONS////////////////////////////////////////////
template<typename COMPTYPE, int reserve>
inline 
typename ComponentManagerBase<COMPTYPE, reserve>::HANDLETYPE
ComponentManagerBase<COMPTYPE, reserve>::CreateComponent()
{
  return m_components.GetNewObject(COMPTYPE());
}

template<typename COMPTYPE, int reserve>
inline void ComponentManagerBase<COMPTYPE, reserve>::DestroyComponent(HANDLETYPE hdl)
{
  m_components.ReleaseObj(COMPHANDLETYPE(hdl));
}

template<typename COMPTYPE, int reserve>
inline COMPTYPE * ComponentManagerBase<COMPTYPE, reserve>::GetComponent(HANDLETYPE hdl)
{
  return m_components.GetObj(COMPHANDLETYPE(hdl));
}

template<typename COMPTYPE, int reserve>
inline void ComponentManagerBase<COMPTYPE, reserve>::InitComponent(const GameObject & go, HANDLETYPE hdl)
{
  m_components[COMPHANDLETYPE(hdl)]->SetGOHandle(go.GetSelfHandle());
}

template<typename COMPTYPE, int reserve>
inline 
typename ComponentManagerBase<COMPTYPE, reserve>::HANDLETYPE
ComponentManagerBase<COMPTYPE, reserve>::CloneComponent(HANDLETYPE oldhdl)
{
  COMPHANDLETYPE chdl = m_components.GetNewObject();
  COMPTYPE& temp = *m_components[chdl];
  CopyComponentToComponent(temp, *m_components[COMPHANDLETYPE(oldhdl)]);
  //set back the handle just in case
  //reason for this order is so that we don't mess with the handle it is holding
  temp.SetSelfHandle(chdl);
  return chdl;
}

template<typename COMPTYPE, int reserve>
inline PBHandle ComponentManagerBase<COMPTYPE, reserve>::CreatePrefab()
{
  m_pbcomponents.emplace_back();
  return m_pbcomponents.size() - 1;
}

template<typename COMPTYPE, int reserve>
inline PBHandle ComponentManagerBase<COMPTYPE, reserve>::CreatePrefabFromComp(HANDLETYPE hdl)
{
  PBHandle pbhdl = CreatePrefab();
  CopyComponentToComponent(m_pbcomponents[pbhdl], *m_components[COMPHANDLETYPE(hdl)]);
  return pbhdl;
}

//does not do error checks
template<typename COMPTYPE, int reserve>
inline BaseComponent * ComponentManagerBase<COMPTYPE, reserve>::GetPrefab(PBHandle hdl)
{
  return &m_pbcomponents[hdl];
}

template<typename COMPTYPE, int reserve>
inline 
typename ComponentManagerBase<COMPTYPE, reserve>::HANDLETYPE
ComponentManagerBase<COMPTYPE, reserve>::CreateComponentFromPrefab(const GameObject& go, PBHandle pbhdl)
{
  //reason for this order is so that we don't mess with the handle
  COMPHANDLETYPE chdl(CreateComponent());
  COMPTYPE& temp = *GetComponent(chdl);
  CopyComponentToComponent(temp, m_pbcomponents[pbhdl]);
  //set back the handle just in case
  //reason for this order is so that we don't mess with the handle it is holding
  temp.SetSelfHandle(chdl);
  InitComponent(go, chdl);
  return chdl;
}


#endif
