/******************************************************************************/
/*!
\file   ScriptCreator.h
\author Zelin ong
\par    email: zelin.ong\@digipen.edu
\par    Digipen login: zelin.ong
\date   02/02/2020
\brief
Contains definitions required for ScriptCreator

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

#define SICREATE(SCRIPTNAME) \
TYPECREATE(SI_##SCRIPTNAME) \
m_creator.insert(std::pair<ScriptIndex, std::shared_ptr<SCRIPTING::ScriptInterface>>(SI_##SCRIPTNAME, std::make_shared<SCRIPTING::ScriptCommonIF<SCRIPTNAME>>()));
//#include "../Scripts/FloatingRing.h"

//m_creator.insert(std::pair<ScriptIndex, std::shared_ptr<SCRIPTING::ScriptInterface>>(SI_##SCRIPTNAME, std::make_shared<SCRIPTNAME>()));

struct SItype : public TypeHelper<ScriptIndex, SItype>
{
  //this one is only for SItype class not ur generic TypeHelper kind of class
  static std::unordered_map<ScriptIndex, std::shared_ptr<SCRIPTING::ScriptInterface>> m_creator;
  void SItype::Create()
  {
    //m_creator.insert(std::pair<ScriptIndex, std::shared_ptr<SCRIPTING::ScriptInterface>>(SI_PlayerController, std::make_shared<SCRIPTING::ScriptCommonIF<PlayerController>>()));

      SICREATE(ObjectiveManager);
    
    //using SICREATE in scriptnames.h
    #include "../Scripts/scriptnames.h"
  }
};
#undef SICREATE

static SItype sitpInit;

#define TOWER_CREATE(TWNAME) \
TYPECREATE(Tower_##TWNAME) \
m_creator.insert(std::pair<TowerType, std::shared_ptr<WeaponInterface>>(Tower_##TWNAME, std::make_shared<Weapon##TWNAME>()));

struct TowerTypeConvert : public TypeHelper<TowerType, TowerTypeConvert>
{
  //static const size_t reserve_sz;
  //static std::vector<const char*> TypeList;
  //static std::unordered_map<std::string, TowerType> StrToType;
  //static std::unordered_map<TowerType, const char *> TypeToStr;
  //static std::unordered_map<TowerType, std::shared_ptr<WeaponInterface>> m_creator;

  //TowerTypeConvert() { Create(); }
  void Create()
  {
   // TOWER_CREATE(Basic);  //SI_BasicTower
   // TOWER_CREATE(Frost);
   // TOWER_CREATE(Laser);
   // TOWER_CREATE(FrostAOE);
   // TOWER_CREATE(Debuff);
   // TOWER_CREATE(DebuffUpgrade);
  }

  static GOHandle GetTurretPrefab(TowerType type)
  {
    return ASC_FACTORY.CreateGOfromPB(TypeToStr[type] + std::string("_Turret"));
  }

  static GOHandle GetWeaponPrefab(TowerType type)
  {
    return ASC_FACTORY.CreateGOfromPB(TypeToStr[type] + std::string("_Weapon"));
  }
};


#define ENEMY_CREATE(ENEMY_NAME) \
TYPECREATE(Enemy_##ENEMY_NAME) //\
//m_creator.insert(std::pair<TowerType, std::shared_ptr<WeaponInterface>>(Tower_##TWNAME, std::make_shared<Weapon##TWNAME>()));

struct EnemyTypeConvert : public TypeHelper<EnemyType, EnemyTypeConvert>
{
  //static const size_t reserve_sz;
  //static std::vector<const char*> TypeList;
  //static std::unordered_map<std::string, TowerType> StrToType;
  //static std::unordered_map<TowerType, const char *> TypeToStr;
  //static std::unordered_map<EnemyType, std::shared_ptr<WeaponInterface>> m_creator;

  //TowerTypeConvert() { Create(); }
  void Create()
  {
   // ENEMY_CREATE(Ground);
   // ENEMY_CREATE(Air);
   // ENEMY_CREATE(Meteorite);
   // ENEMY_CREATE(Speedy);
  }

  static GOHandle GetPrefab(EnemyType type)
  {
    return ASC_FACTORY.CreateGOfromPB(TypeToStr[type]);
  }
};


