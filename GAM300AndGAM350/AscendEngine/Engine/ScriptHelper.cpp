/******************************************************************************/
/*!
\file   ScriptHelper.cpp
\author Zelin ong
\par    email: zelin.ong\@digipen.edu
\par    Digipen login: zelin.ong
\date   02/02/2020
\brief
Contains definitions required for ScriptHelper

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"



SCRIPTING::ScriptInterface * GetScript_fromGO(GOHandle handle, ScriptIndex idx)
{
  SCRIPTING::ScriptInterface* script = nullptr;
  LogicComp* l1 = ASC_GETCOMP_PTR(LogicComp, handle);
  if (l1 != nullptr)
  {
      for (auto& sI : l1->scriptsHdl)
      {
          SCRIPTING::ScriptInterface* script_tmp = ASC_SCRIPTMAN.GetScript(sI);
          //if (compare_str(SItype::TypeToStr[idx], script->GetName()))
          if (idx == script_tmp->GetScriptType())
          {
              script = script_tmp;
              return script;
          }
      }
  }
 // assert(false);
  return nullptr;
}


SCRIPTING::ScriptInterface* GetScriptInteract(GOHandle handle)
{
   SCRIPTING::ScriptInterface* script = nullptr;
    LogicComp* l1 = ASC_GETCOMP_PTR(LogicComp, handle);
    for (auto& sI : l1->scriptsHdl)
    {
        SCRIPTING::ScriptInterface* script_tmp = ASC_SCRIPTMAN.GetScript(sI);
        for (const auto& info : interactInfo_map)
        {
            if (script_tmp->GetScriptType() == info.second.scriptType && static_cast<Interact *>(script_tmp)->activated)
            {
                script = script_tmp;
                return script;
            }
        }
    }

   // assert(false);
    return script;
}
SCRIPTING::ScriptInterface* GetScriptBaseTrigger(GOHandle handle)
{
  SCRIPTING::ScriptInterface* script = nullptr;
  LogicComp* l1 = ASC_GETCOMP_PTR(LogicComp, handle);
  for (auto& sI : l1->scriptsHdl)
  {
    SCRIPTING::ScriptInterface* script_tmp = ASC_SCRIPTMAN.GetScript(sI);
    for (const auto& info : textInfo_map)
    {
      if (script_tmp->GetScriptType() == info.second.scriptType)
      {
        script = script_tmp;
        return script;
      }
    }
  }

 // assert(false);
  return script;
}
SCRIPTING::ScriptInterface* GetScript_Infor(GOHandle handle)
{
  SCRIPTING::ScriptInterface* script = nullptr;
  LogicComp* l1 = ASC_GETCOMP_PTR(LogicComp, handle);
  for (auto& sI : l1->scriptsHdl)
  {
    SCRIPTING::ScriptInterface* script_tmp = ASC_SCRIPTMAN.GetScript(sI);
    SCRIPTING::ObjInfor * vaild =  dynamic_cast<SCRIPTING::ObjInfor*> (script_tmp);
    if (vaild != nullptr)
    {
      script = script_tmp;
      return script;
    }
    ////if (compare_str(SItype::TypeToStr[idx], script->GetName()))
    //if (idx == script_tmp->GetScriptType())
    //{
    //  script = script_tmp;
    //  return script;
    //}
  }

  // assert(false);
  return nullptr;
}

float Calculate_ValuePerFrame(float valPerSec, float dt)
{
  float multiplier = 1.0f / dt;
  float valPerFrame = valPerSec / multiplier;

  return valPerFrame;
}


BaseComponent* GetComp(COMPONENT_TYPE type, GOHandle h)
{
  BaseComponent* ret = nullptr;
  switch (type)
  {
    case TRANSFORM:
      ret = ASC_GETCOMP_PTR(TransformComp, h);
      break;
    case GRAPHICS:
      ret = ASC_GETCOMP_PTR(GraphicsComp, h);
      break;
    case PHYSICS:
      ret = ASC_GETCOMP_PTR(PhysicsComp, h);
      break;
    case LOGIC:
      ret = ASC_GETCOMP_PTR(LogicComp, h);
      break;
    case SOUND:
      ret = ASC_GETCOMP_PTR(SoundComp, h);
      break;
    case LIGHT:
      ret = ASC_GETCOMP_PTR(LightComp, h);
      break;
    case WAYPOINT:
      ret = ASC_GETCOMP_PTR(WayPointNode, h);
      break;
    case UITEXT:
      ret = ASC_GETCOMP_PTR(UITextComp, h);
      break;
    case PARTICLE:
      ret = ASC_GETCOMP_PTR(ParticleEmitter, h);
      break;

    case MAX:
      break;
    default:
      break;
  }

  assert(ret);
  return ret;
}


float CalculateDmgRegisterDelay(float distToTarget_sq, float bullet_speed, float mul_modifier_ToResult)
{
  float dist = sqrt(distToTarget_sq);
  return (dist / bullet_speed) * mul_modifier_ToResult;
}

GOHandle ParticleEffect_HitTarget_Basic(const std::string & prefab, const TargetInfo & info/*, Camera & cam, float raycastTime*/, 
  float delay, bool emit)
{
  //////position of sphere
  ////const float angle = ASC_DEG2RAD(20.0f);
  //float rad = info.targetHdl->GetHdl<PhysicsComp>()->GetRadius();
  //float horizontal_offset = rad * cos(angle);
  Vec3 midPos = info.target_pos - info.dir * info.targetHdl->GetHdl<PhysicsComp>()->GetRadius();
  //Vec3 leftPos = midPos + Vec3(horizontal_offset, 0.0f, horizontal_offset);
  //std::vector<Vec3> vec = { midPos, leftPos, -leftPos };
  //////position of sphere
  ////const float angle = ASC_DEG2RAD(20.0f);
  //float sign = ASC_RAND_RANGE_(-1.0f, 1.0f);
  //sign = sign > 0.0f ? 1.0f : -1.0f;
  //float rad = info.targetHdl->GetHdl<PhysicsComp>()->GetRadius();
  //float offset = rad * (ASC_RAND_RANGE_(0.0f, 1.0f) / 1.0f) * sign;
  //Vec3 midPos = info.target_pos - info.dir * info.targetHdl->GetHdl<PhysicsComp>()->GetRadius();
  //Vec3 leftPos = midPos + cam.GetRight() * offset;
  //std::vector<Vec3> vec = { midPos, leftPos, -leftPos };



  GOHandle particle = ASC_FACTORY.CreateGOfromPB(prefab.data());
  //particle->GetHdl<TransformComp>()->pos = vec[idx];
  particle->GetHdl<TransformComp>()->pos = midPos;//[ASC_RAND_RANGE_(0.0, (float)vec.size())];
  particle->GetHdl<TransformComp>()->dirty = true;
  auto emitter = particle->GetHdl<ParticleEmitter>();
  Vec3 dirToSpark = -info.dir;
  dirToSpark.y += 0.5f;
  Vec3 dirToSpark_1 = dirToSpark - Vec3(0.2f, 0, 0);
  Vec3 dirToSpark_2 = dirToSpark + Vec3(0.2f, 0, 0.2f);
  emitter->minVelocity = dirToSpark_1 * 50.f;
  emitter->maxVelocity = dirToSpark_2 * 200.0f;
  emitter->currentDelay = delay;
  emitter->StartCountdown(emitter->maxLifetime);
  //emitter->Emit();

  GOHandle particle2 = ASC_FACTORY.CreateGOfromPB("Particle_Basic_Bullet_Hit_Impact");
  particle2->GetHdl<TransformComp>()->pos = midPos;
  auto emitter2 = particle2->GetHdl<ParticleEmitter>();
  emitter2->currentDelay = delay;
  emitter2->StartCountdown(emitter->maxLifetime);

  if (emit)
  {
    emitter->Emit();
    emitter2->Emit();
  }

  return particle;
}

GOHandle ParticleEffect_HitFloor_Basic(const std::string & prefab, /*const TargetInfo & info,*/ Camera & cam, float raycastTime,
  bool emit)
{
  GOHandle particle = ASC_FACTORY.CreateGOfromPB(prefab.data());
  particle->GetHdl<TransformComp>()->pos =
    cam.GetPos() + cam.GetForward()* (raycastTime);
  particle->GetHdl<TransformComp>()->dirty = true;
  auto emitter = particle->GetHdl<ParticleEmitter>();
  emitter->currentDelay = raycastTime;
  emitter->StartCountdown(emitter->maxLifetime);

  
  GOHandle particle2 = ASC_FACTORY.CreateGOfromPB("Particle_Basic_Bullet_Hit_Impact");
  particle2->GetHdl<TransformComp>()->pos =
    cam.GetPos() + cam.GetForward()* (raycastTime);
  particle2->GetHdl<TransformComp>()->dirty = true;
  auto emitter2 = particle2->GetHdl<ParticleEmitter>();
  emitter2->currentDelay = raycastTime;
  emitter2->StartCountdown(emitter->maxLifetime);

  if (emit)
  {
    emitter->Emit();
    emitter2->Emit();
  }

  return particle;
}

GOHandle ParticleEffect_TargetBehindCover_Basic(const std::string & prefab,/* const TargetInfo & info,*/ Camera & cam, float raycastTime,
  float delay, bool emit)
{
  Vec3 pos = cam.GetPos() + cam.GetForward()* (raycastTime);

  //raycastTime is time to enemy
  GOHandle particle = ASC_FACTORY.CreateGOfromPB(prefab);
  particle->GetHdl<TransformComp>()->pos = pos;
  particle->GetHdl<TransformComp>()->dirty = true;
  auto emitter = particle->GetHdl<ParticleEmitter>();
  emitter->StartCountdown(emitter->maxLifetime);

  GOHandle particle2 = ASC_FACTORY.CreateGOfromPB(prefab);
  particle2->GetHdl<TransformComp>()->pos = pos;
  particle2->GetHdl<TransformComp>()->dirty = true;
  auto emitter2 = particle2->GetHdl<ParticleEmitter>();
  emitter2->StartCountdown(emitter->maxLifetime);

  GOHandle particle3 = ASC_FACTORY.CreateGOfromPB("Particle_Basic_Bullet_Hit_Impact");
  particle3->GetHdl<TransformComp>()->pos = pos;
  auto emitter3 = particle3->GetHdl<ParticleEmitter>();
  emitter3->StartCountdown(emitter->maxLifetime);

  if (emit)
  {
    emitter->Emit();
    emitter2->Emit();
    emitter3->Emit();
  }

  return particle;
}

GOHandle ParticleEffect_HitTarget_Frost(const std::string & prefab, const TargetInfo & info/*, Camera & cam, float raycastTime*/,
  float delay, bool emit)
{
  GOHandle particle = ParticleEffect_HitTarget_Basic(prefab, info, delay, false);
  auto emitter = particle->GetHdl<ParticleEmitter>();

  //difference for frost 
  emitter->startMinCol = emitter->startMaxCol =
    emitter->endMinCol = emitter->endMaxCol = Color::BLUE_LIGHT;
  emitter->isBlending = true;

  GOHandle particle2 = ASC_FACTORY.CreateGOfromPB("Particle_Frost_Bullet_Hit_Impact");
  particle2->GetHdl<TransformComp>()->pos = particle->GetHdl<TransformComp>()->pos;
  auto emitter2 = particle2->GetHdl<ParticleEmitter>();
  emitter2->currentDelay = delay;
  emitter2->StartCountdown(emitter->maxLifetime);
  //

  if (emit)
  {
    emitter->Emit();
    emitter2->Emit();
  }

  return particle;
}

GOHandle ParticleEffect_HitFloor_Frost(const std::string & prefab, /*const TargetInfo & info,*/ Camera & cam, 
  float raycastTime, bool emit)
{
  GOHandle particle = ParticleEffect_HitFloor_Basic(prefab, cam, raycastTime, false);
  auto emitter = particle->GetHdl<ParticleEmitter>();

  //difference for frost 
  emitter->startMinCol = emitter->startMaxCol =
    emitter->endMinCol = emitter->endMaxCol = Color::BLUE_LIGHT;
  emitter->isBlending = true;

  GOHandle particle2 = ASC_FACTORY.CreateGOfromPB("Particle_Frost_Bullet_Hit_Impact");
  particle2->GetHdl<TransformComp>()->pos = particle->GetHdl<TransformComp>()->pos;
  auto emitter2 = particle2->GetHdl<ParticleEmitter>();
  emitter->currentDelay = raycastTime;
  emitter2->StartCountdown(emitter->maxLifetime);
  //

  if (emit)
  {
    emitter->Emit();
    emitter2->Emit();
  }

  return particle;
}

GOHandle ParticleEffect_TargetBehindCover_Frost(const std::string & prefab,/* const TargetInfo & info,*/ Camera & cam, float raycastTime,
  float delay, bool emit)
{
  GOHandle particle = ParticleEffect_TargetBehindCover_Basic(prefab, cam, raycastTime, delay, false);
  auto emitter = particle->GetHdl<ParticleEmitter>();

  //difference for frost 
  emitter->startMinCol = emitter->startMaxCol =
    emitter->endMinCol = emitter->endMaxCol = Color::BLUE;
  emitter->isBlending = true;

  GOHandle particle2 = ASC_FACTORY.CreateGOfromPB("Particle_Frost_Bullet_Hit_Impact");
  particle2->GetHdl<TransformComp>()->pos = particle->GetHdl<TransformComp>()->pos;
  auto emitter2 = particle2->GetHdl<ParticleEmitter>();
  emitter2->StartCountdown(emitter->maxLifetime);
  //

  //if (emit)
  //{
  //  emitter->Emit();
  //  emitter2->Emit();
  //}

  return particle;
}





