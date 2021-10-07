/******************************************************************************/
/*!
\file   BaseHandle.cpp
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

#include "pch.h"

//template<typename ObjType>
std::vector<int> BaseShareHandle::m_unused;
//template<typename ObjType>
std::vector<BaseShareHandle::SlotDataT> BaseShareHandle::m_data;

//#define BHDLDEBUG

/*
If baseHdl is default initialize then it would a invalid handle
*/
BaseShareHandle::BaseShareHandle(const HandleT& baseHdl)
{
#ifdef BHDLDEBUG
  bool fromUnused = true;
#endif

  //give it a slot that can contain the address of baseHdl
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
    m_slot = static_cast<int>(m_data.size());
    m_data.emplace_back();
#ifdef BHDLDEBUG
    fromUnused = false;
#endif
  }

#ifdef BHDLDEBUG
  static int cmpNum = 0;
  static int cmpArr[] = { 500, 1000, 1500, 2000, 3000};
  if (m_slot == cmpArr[cmpNum])
  {
    ++cmpNum;
  }
#endif
  //if (m_slot == m_data.size())
  //  int i = 0;

  m_data[m_slot] = SlotDataT(1, baseHdl);
  m_ptr = &m_data[m_slot];
}

BaseShareHandle::~BaseShareHandle()
{
  //destructor will need to lower the counter by 1
  --m_data[m_slot].m_useNum;
  FreeSlot();
}

//template<typename ObjType>
BaseShareHandle::BaseShareHandle(const BaseShareHandle & hdl)
{
  //I believe we will only need to increase the counter
  ++m_data[hdl.m_slot].m_useNum;
  m_slot = hdl.m_slot;
  //for debug
  m_ptr = hdl.m_ptr;
}


//template<typename ObjType>
BaseShareHandle & BaseShareHandle::operator=(const BaseShareHandle & hdl)
{
  //for assignment we will need to
  //increase the counter of the new hdl
  //and decrease the current counter
  //this order will prevent the case during self assignment
  ++m_data[hdl.m_slot].m_useNum;
  --m_data[m_slot].m_useNum;
  FreeSlot();
  m_slot = hdl.m_slot;
  //for debug
  m_ptr = hdl.m_ptr;
  // insert return statement here
  return *this;
}

//this function will free up a slot when the slot counter goes to zero
//template<typename ObjType>
void BaseShareHandle::FreeSlot()
{
  //m_data[slot].m_useNum == 0
  if (!m_data[m_slot].m_useNum)
  {
    m_data[m_slot].m_baseHdl.InValidateHandle();
    m_unused.push_back(m_slot);
    m_slot = -1;
  }
}
