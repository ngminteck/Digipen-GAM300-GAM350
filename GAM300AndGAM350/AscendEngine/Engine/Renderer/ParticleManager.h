/******************************************************************************/
/*!
\file   ParticleManager.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Declarations for particle manager.

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

//class ParticleManager : public ComponentManagerBase<ParticleEmitter>
//{
//public:
//  ParticleManager();
//  
//  void Load() override;
//  void Update(float dt) override;
//  void UnLoad() override
//  {
//    m_emitterList.clear();
//  }
//
//  int CreateComponent(int handle);
//  void RemoveComponent(int handle);
//  
//  ParticleEmitter& operator[](unsigned int index){ return m_emitterList[index]; }
//  const ParticleEmitter& operator[](unsigned int index) const { return m_emitterList[index]; }
//
//private:
//  std::vector<ParticleEmitter> m_emitterList;
//  std::stack<int> m_pool;
//  //Concurrency::critical_section lock;
//};

typedef ComponentManagerBase<ParticleEmitter>::COMPHANDLETYPE PEHandle;

#define ASC_PARTICLES ParticleManager::Inst()
class ParticleManager : public ComponentManagerBase<ParticleEmitter> , public Singleton<ParticleManager>
{
public:
  ParticleManager();
  void Load() override;
  void UnLoad() override;

  void Update(float dt) override;

  BaseHandle Deserialize(const GameObject &gameObject, Deserializer &) override;
  void Serialize(Serializer &, BaseHandle) override;
  void Reflect(BaseHandle, const Reflector &) override;

  void InitComponent(const GameObject& go, BaseHandle hdl) override;
  void CopyComponentToComponent(ParticleEmitter& target, const ParticleEmitter& source) override;

  void HandleMsg(MsgData_Base *) override;

  std::vector<ParticleEmitter*> sortedList;
  bool isEditorMode, isGamePause;
private:


  std::stack<GOHandle> m_destroy;
};

#endif
