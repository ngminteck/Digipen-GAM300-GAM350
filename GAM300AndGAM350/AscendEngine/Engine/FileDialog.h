/******************************************************************************/
/*!
\file   FileDialog.h
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions required for SaveFileDialog & LoadFileDialog

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

bool SaveFileDialog(std::wstring &fileName, const wchar_t *ext, const wchar_t *path);
bool LoadFileDialog(std::wstring &fileName, const wchar_t *ext, const wchar_t *path);