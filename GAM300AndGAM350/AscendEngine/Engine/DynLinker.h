/******************************************************************************/
/*!
\file   DynLinker.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains all lib includes

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

#ifndef DYN_LINKER_H
#define DYN_LINKER_H

// Requests the linker to add Xinput9_1_0.lib to the runtime libraries
// Same as adding to Linker->Input->additional dependencies
#pragma comment(lib, "Xinput9_1_0.lib")

/* FBXSDK LIBS */
//#ifdef _DEBUG
//
//#pragma comment( lib, "Debug/libfbxsdk" )
//#pragma comment( lib, "Debug/libfbxsdk-md" )
//
//#elif _RELEASE || _SUBMISSION
//
//#pragma comment( lib, "Release/libfbxsdk" )
//#pragma comment( lib, "Release/libfbxsdk-md" )
//
//#endif

/* OPENGL LIBS */
#pragma comment (lib, "opengl32.lib")

/* FMOD LIBS */
#pragma comment (lib, "fmod/lib/fmod64_vc.lib")

/* SOIL LIBS */
//#pragma comment (lib, "SOIL/SOIL.lib")
#endif