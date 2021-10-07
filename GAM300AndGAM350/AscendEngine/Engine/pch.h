/******************************************************************************/
/*!
\file   pch.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define CHEATS 1

#define MEMSZ 750
#define DB_MEMSZ 200

// Windows Header Files:
#include <windows.h>
#include <Shellapi.h>
//#include <tchar.h>


//openGL include
#include "../lib/GL/gl3w.h"

// Windows Header Files:
#include "../resource.h"
#include "Paths.h"
#include <iostream>
#include <functional>
#include <thread>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <mutex>
#include <Random>
#include <utility>
#include <iterator>
#include <memory>
#include <type_traits>

#include <chrono>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//Container types
#include <map>
#include <set>
#include <list>
#include <stack>
#include <array>
#include <queue>
#include <vector>
#include <unordered_map>

#include <Xinput.h>
#include <math.h>
#include <cstdlib>
#include <limits.h>
#include <float.h>

#include "Math/MathExt.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
//#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

#include "HelperFn.h"

//Lib includes
#include "../lib/SOIL/SOIL.h"
#include "../lib/tinyxml2/tinyxml2.h"
#include "../lib/IMGUI/imgui.h"
#include "../lib/IMGUI/ImGuizmo.h"
#include "../lib/IMGUI/stb_truetype.h"
#include "../lib/assimp/include/assimp/cimport.h"
#include "../lib/assimp/include/assimp/scene.h"
#include "../lib/assimp/include/assimp/postprocess.h"

#include "Bezier.h"
#include "Curve.h"
#include "Key.h"
#include "FileDialog.h"

#include "../lib/fmod/inc/fmod.h"
#include "../lib/fmod/inc/fmod.hpp"
#include "../lib/fmod/inc/fmod_errors.h"


// physic lib
#include "OpenDynamicEngine/include/ode/ode.h"
// graphic lib for physic
#include <drawstuff/drawstuff.h>


// not lib

#include "Serialization.h"
#include "BaseHandle.h"
#include "BaseManager.h"
#include "Parenting.h"
#include "AppConsole.h"
#include "Logger.h"
#include "GameObject_type.h"
#include "MsgDataBase.h"
#include "Messaging.h"
#include "System.h"
#include "GridData.h"
#include "GameObject.h"
#include "ObjectFactory.h"
#include "BaseComponent.h"
#include "Gamepad.h"
#include "Tween.h"
#include "MTween.h"
#include "Timer.h"
#include "Sequencer.h"
#include "DialogBoxSystem.h"


#include "Shader.h"
#include "Renderer/ObjLoader.h"
#include "Camera.h"
#include "ImguiWrapper.h"

#include "Physics/Shape.h"
#include "Physics/Collision.h"
#include "Physics/Body.h"
#include "Physics/Manifold.h"
#include "Physics/World.h"
#include "Physics/PhysicsManager.h"
#include "Physics/ObjectPool.h"

#include "Physics/CollisionFilter.h"
#include "Physics/NewPhysicsManager.h"

#include "Physics/OpenDynamicsEngine.h"

//Managers
#include "InputManager.h"
#include "TransformManager.h"
#include "Renderer/TextureLoader.h"
#include "Renderer/LightManager.h"
#include "Renderer/GLRenderer.h"
#include "Renderer/ParticleEmitter.h"
#include "Renderer/ParticleManager.h"
#include "Renderer/TrailRenderer.h"
#include "GraphicsManager.h"
#include "LogicManager.h"
#include "SoundManager.h"
#include "WaypointManager.h"
#include "UITextManager.h"
#include "TagManager.h"
#include "AnimationManager.h"

#include "Action.h"
#include "Editor.h"

//Scripts
#include "ScriptHelper.h"
#include "ScriptInterface.h"
#include "ObjectiveManager.h"

//all the scripts include except Objective Man is in scriptIncludes
//and another place to add is in scriptNames
#include "../Scripts/scriptIncludes.h"


#include "ScriptCreator.h"
#include "Engine.h"