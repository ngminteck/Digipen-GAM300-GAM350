/******************************************************************************/
/*!
\file   GameObject_type.h
\author
\par    email: username\@digipen.edu
\par    Digipen login: username
\date   02/02/2020
\brief
Contains definitions required for GameObject_type

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#pragma once

//static std::hash<std::string> hashstrvar;
//typedef size_t GO_TYPE;
//
//#define GOTPCREATE(GOtype_name) \
//const char* Str_##GOtype_name = #GOtype_name; \
//GO_TYPE GOtype_name = hashstrvar(#GOtype_name);

#define GOTPCREATE(GOtype_name) \
strGOTypeMap.insert(std::pair<std::string, GOTYPE>(#GOtype_name, GOtype_name)); \
GOTypeStrMap.insert(std::pair<GOTYPE, const char *>(GOtype_name , #GOtype_name)); \
GOtypeList.push_back(#GOtype_name);

//for the different type of gameobject
//NOTE: after adding a new object type please
//use GOTPCREATE in GOTP::create()
enum GOTYPE
{
  go_default,
  go_player,
  go_floor,
  go_crate,
  go_playerbullet,
  go_enemy,
  go_remote,

  go_UI,

  go_tower,
  //go_frosttower,
  go_harvester,
  go_waypoint,
  go_powerplant,
  go_powerbeacon,
  go_spawner,
  go_turret,
  //for the Level objs
  go_mess,
  go_lab,
  go_wallsegment,
  go_enemyspawnmanager,
  go_lightbeacon,
  go_levelmanager,
  //for specific stuff
  go_unbuildable,

  //Also add to :
  //1)GOTPCREATE
  go_interact,
  go_doorInteract,
  go_itemInteract,
  go_pickupInteract,
  go_max,

  go_LightsOff
};

struct GOTP
{
  static const size_t reserve_sz;
  static std::vector<const char*> GOtypeList;
  static std::unordered_map<std::string, GOTYPE> strGOTypeMap;
  static std::unordered_map<GOTYPE, const char *> GOTypeStrMap;

  GOTP() { create(); }
  ~GOTP() {
      GOtypeList.clear();
      strGOTypeMap.clear();
      GOTypeStrMap.clear();
  }
  static void create()
  {
    if (!GOtypeList.size())
    {
      GOtypeList.reserve(reserve_sz);
      strGOTypeMap.reserve(reserve_sz);
      GOTypeStrMap.reserve(reserve_sz);

      //add a new line with the respective type after a new type creation
      GOTPCREATE(go_default);
      GOTPCREATE(go_player);
      GOTPCREATE(go_floor);
      GOTPCREATE(go_crate);
      GOTPCREATE(go_playerbullet);
      GOTPCREATE(go_enemy);
      GOTPCREATE(go_remote);


      GOTPCREATE(go_tower);
      //GOTPCREATE(go_frosttower);
      GOTPCREATE(go_harvester);
      GOTPCREATE(go_waypoint);
      GOTPCREATE(go_powerplant);
      GOTPCREATE(go_powerbeacon);
      GOTPCREATE(go_spawner);
      GOTPCREATE(go_turret);
      //level objs
      GOTPCREATE(go_mess);
      GOTPCREATE(go_lab);
      GOTPCREATE(go_wallsegment);
      GOTPCREATE(go_enemyspawnmanager);
      GOTPCREATE(go_lightbeacon);

      GOTPCREATE(go_levelmanager);
      GOTPCREATE(go_unbuildable);
      GOTPCREATE(go_interact);
      GOTPCREATE(go_doorInteract);
      GOTPCREATE(go_itemInteract);
      GOTPCREATE(go_pickupInteract);

      GOTPCREATE(go_UI);

      GOTPCREATE(go_LightsOff);
    }
  }
};

static GOTP gotpInit;

//static std::unordered_map<std::string, GOTYPE> strGOTypeMap = {
//  { "go_default",         go_default      },
//  { "go_player",          go_player       },
//  { "go_floor",           go_floor        },
//  { "go_playerbullet",    go_playerbullet },
//  { "go_enemyGround",      go_enemyGround   }
//};
//
//static std::unordered_map<GOTYPE, const char *> GOTypeStrMap = {
//  { go_default,       "go_default" },
//  { go_player ,        "go_player" },
//  { go_floor  ,        "go_floor" },
//  { go_playerbullet,   "go_playerbullet" },
//  { go_enemyGround,     "go_enemyGround" }
//};

//namespace GOTP
//{
//  GOTPCREATE(Default);
//  GOTPCREATE(Player);
//};



