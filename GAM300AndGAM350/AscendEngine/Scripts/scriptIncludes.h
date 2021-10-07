#pragma once

//#define HASH_LIT #
//#define QUOTE(x) #x
//#define HASH() HASH_LIT
//#define SICREATE(x) HASH()include QUOTE(../Scripts/##x.h)
//#include "../Scripts/scriptnames.h"
//#undef SICREATE
//#undef HASH

#include "../Scripts/Player/PlayerController.h"

#include "../Scripts/SplashScreen.h"


#include "../Scripts/LevelManager.h"

#include "../Scripts/PauseScript.h"
#include "../Scripts/PlaySoundOnStart.h"

#include "../Scripts/MainMenuScript.h"
#include "../Scripts/EndGameScript.h"

#include "../Scripts/Etc/RoombaController.h"
#include "../Scripts/Etc/EnableOrDiasble.h"
#include "../Scripts/Etc/UIManager.h"
#include "../Scripts/Etc/WaterContainer.h"

#include "../Scripts/Interact/Interact.h"
#include "../Scripts/Interact/DoorInteract.h"
#include "../Scripts/Interact/ItemInteract.h"
#include "../Scripts/Interact/PickupInteract.h"
#include "../Scripts/Interact/BarkInteract.h"
#include "../Scripts/Interact/ActivateInteract.h"
#include "../Scripts/Interact/MoveInteract.h"
#include "../Scripts/Interact/PuppyInteract.h"
#include "../Scripts/Interact/PushPullInteract.h"
#include "../Scripts/Interact/SwivelInteract.h"
#include "../Scripts/Interact/FakePlacement.h"



#include "../Scripts/Trigger/BaseTrigger.h"
#include "../Scripts/Trigger/EventTrigger.h"

#include "../Scripts/Player/Hint.h"
#include "../Scripts/Player/Waypointer.h"
#include "../Scripts/Player/Scent.h"
#include "../Scripts/Player/PuzzleLock.h"
#include "../Scripts/Player/PuzzleKey.h"
#include "../Scripts/Player/Director.h"
#include "../Scripts/Player/PuzzleKeyHole.h"
#include "../Scripts/Player/CameraTrigger.h"
#include "../Scripts/Player/DirectorTrigger.h"
#include "../Scripts/Etc/StopFMovin.h"