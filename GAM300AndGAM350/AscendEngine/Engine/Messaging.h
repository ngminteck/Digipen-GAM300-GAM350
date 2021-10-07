/******************************************************************************/
/*!
\file   Messaging.h
\author Tan Hong Boon
\par    email: h.tan\@digipen.edu
\par    Digipen login: h.tan
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#ifndef _MESSAGING
#define _MESSAGING

#define ASC_MSGSYS MessagingSystem::Inst()
class MessagingSystem : public Singleton<MessagingSystem>
{
  //client does not need to use these
  typedef std::function<void(MsgData_Base*)> Observer;
  typedef std::list<Observer> ObserverList;
  typedef std::list<Observer>::const_iterator ObserverList_cIter;
  //

public:
  
  //for client use
  typedef std::pair<MsgType, ObserverList_cIter> ReturnToken;
  typedef std::list<ReturnToken> ReturnTokenList;
  //

  MessagingSystem();
  ~MessagingSystem();
  void Update();

  template<typename T>
  ReturnToken RegisterCallBack(MsgType t, void (T::*fn)(MsgData_Base*), T* ptr);
  ReturnToken RegisterCallBack(MsgType e, Observer fn)
  {
    return ReturnToken(e, Add_Observer(e, fn));
  }

  void Remove_Observer(ReturnToken& token);
  void Remove_Observer(ReturnTokenList& tokenList);
  void Remove_All(MsgType type);

  template<typename T>
  void SendMsg(const T& m);

  void TestCallBack(MsgData_Base* m);
  void TestCallBack2(MsgData_Base* m);

private:
  void Remove_Observer(MsgType type, ObserverList::const_iterator iter);
  ObserverList_cIter Add_Observer(MsgType e, Observer fn);
  void NotifyObservers(ObserverList* list, MsgData_Base* msg);

  //store all msges recevied
  std::queue<MsgData_Base*> inbox;
  //save all observers subscribed to specific event/MsgType
  std::unordered_map<MsgType, ObserverList> observerMap;
};

//ASC_MSGSYS.RegisterCallBack(mTYPE_CHANGE_SCENE, &Engine::HandleMsg, this);
template<typename T>
inline MessagingSystem::ReturnToken	//return type
MessagingSystem::RegisterCallBack(MsgType t, void (T::*fn)(MsgData_Base*), T* objptr)
{
  ObserverList_cIter iter =
    Add_Observer(t,
      std::bind(fn, objptr, std::placeholders::_1));

  return ReturnToken(t, iter);
}

//ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_CHANGE_SCENE));
template<typename T>
inline void MessagingSystem::SendMsg(const T & msg)
{
	//queue msg in inbox
	inbox.push(new T(msg));
}

#endif