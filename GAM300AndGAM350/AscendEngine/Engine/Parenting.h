/******************************************************************************/
/*!
\file   Parenting.h
\author Zelin Ong
\par    email: zelin.ong\@digipen.edu
\par    Digipen login: zelin.ong
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once


//template<typename HandleType>
//struct PatNode
//{
//  //this handle is transform handle
//  HandleType m_objhdl;
//  PatNode* m_parent = nullptr;
//  std::vector<PatNode*> m_childs;
//  //check if this node is in a tree 
//  //there should either have a parent or at least a child
//  bool IsValid() const { return m_parent || m_childs.size(); }
//};
//
//template<typename NodeType>
//struct ParentTreeInfo
//{
//  NodeType* m_root;
//  ParentTreeInfo(NodeType* ptr) : m_root(ptr) {}
//  NodeType* operator->() { return m_root; }
//  operator NodeType*() { return m_root; }
//};
//
////for future implementation\
////for now it is just new
//template<typename T>
//struct MyAllocator
//{
//  template<typename ...Args>
//  T* allocate(Args ...args) { return new T(std::forward<Args>(args)...); }
//  void deallocate(T* ptr) { delete ptr; }
//};
//
//
///*
//suppose to put in the Component type and its handle type
//user needs to use ASC_vargetset(ParentManager<CompType, HandleType>::NodeType, SelfNode)
//*/
//template<typename CompType, typename HandleType>
//class ParentManager
//{
//public:
//  typedef PatNode<HandleType> NodeType;
//  typedef ParentTreeInfo<NodeType> ParentTreeType;
//  typedef std::unordered_map<NodeType*, ParentTreeType> ContainerType;
//
//  ~ParentManager();
//
//  void Attach(CompType& parent, CompType& child);
//  void Detach(CompType& parent, CompType& child);
//  void RemoveChildTree(CompType& parent, CompType& child);
//  void Clear();
//  inline ContainerType& GetListOfParents() { return m_trees; }
//
//private:
//  MyAllocator<NodeType> m_alloc;
//  ContainerType m_trees;
//
//  void ReleaseNode(NodeType* nptr);
//};
//
//template<typename CompType, typename HandleType>
//inline ParentManager<CompType, HandleType>::~ParentManager()
//{
//  Clear();
//}
//
//template<typename CompType, typename HandleType>
//inline void ParentManager<CompType, HandleType>::Attach(CompType & parent, CompType & child)
//{
//  //create a new node for child if child is really not attached
//  if (!child.GetSelfNode()) return;
//
//  NodeType* pnode = parent.GetSelfNode();
//  NodeType* cnode = m_alloc.allocate();
//  child.SetSelfNode(cnode);
//  //if parent node has a reference then a tree is already established and we need to use it
//  //else the parent has no reference to a PatNode then it is a new tree
//  if (!pnode)
//  {
//    //pnode == nullptr
//    //create new node for both parent and child and make a new tree
//    pnode = m_alloc.allocate();
//    parent.SetSelfNode(pnode);
//    m_trees.insert({pnode, pnode});
//  }
//
//  //make the parent the childs parent
//  cnode->m_parent = pnode;
//  //just attach the child to parent list of childs
//  pnode->m_childs.push_back(cnode);
//}
//
////user have to make sure they put in the right parent
////because this class is unable to get the parent themselves
////after detach, child be comes a new tree
//template<typename CompType, typename HandleType>
//inline void ParentManager<CompType, HandleType>::Detach(CompType& parent, CompType & child)
//{
//  NodeType* cnode = child.GetSelfNode();
//  NodeType* pnode = parent.GetSelfNode();
//  int i = -1;
//  //if the parent of child and parent does not match, nothing will happen
//  if (!pnode || cnode->m_parent != pnode) return;
//
//  //else find the child index
//  int counter = 0;
//  for (NodeType* cptr : pnode->m_childs)
//  {
//    if (cptr == cnode)
//    {
//      i = counter;
//      break;
//    }
//    ++counter;
//  }
//
//  //detach and make the child a tree if it have any children
//  if (i != -1)
//  {
//    //replace the current guy of parent childs with the last guy
//    pnode->m_childs[i] = pnode->m_childs.back();
//    //and pop
//    pnode->m_childs.pop_back();
//    //check if child have any children
//    if (cnode->m_childs.empty())
//    {
//      //remove
//      ReleaseNode(cnode);
//      child.SetSelfNode(nullptr);
//    }
//    else
//    {
//      //add as new tree
//      m_trees.insert({ cnode, cnode });
//    }
//  }
//}
//
//template<typename CompType, typename HandleType>
//inline void ParentManager<CompType, HandleType>::RemoveChildTree(CompType & parent, CompType & child)
//{
//  //need to detach first
//
//}
//
//template<typename CompType, typename HandleType>
//inline void ParentManager<CompType, HandleType>::Clear()
//{
//  //destory every node in every tree...this is very bad...
//  //should have a mem man that can just reduce everything to nothing in a single wipe
//  for (auto& ptt : m_trees)
//    ReleaseNode(ptt.first);
//}
//
//template<typename CompType, typename HandleType>
//void ParentManager<CompType, HandleType>::ReleaseNode(NodeType * nptr)
//{
//  for (NodeType* cptr : nptr->m_childs)
//    ReleaseNode(cptr);
//
//  m_alloc.deallocate(nptr);
//}
