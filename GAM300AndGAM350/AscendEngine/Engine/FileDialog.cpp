/******************************************************************************/
/*!
\file   FileDialog.cpp
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
#include "pch.h"

#include <atlbase.h>
#include <Shobjidl.h>
#include <shellapi.h>

static bool FileDialog(std::wstring &fileName, const wchar_t *ext, const wchar_t *wszPath, const wchar_t *button, const wchar_t *title, CLSID dialogType)
{

  // CoCreate the File Open Dialog object.
  IFileDialog *pDlg = nullptr;
  HRESULT hr = CoCreateInstance(dialogType,
                                nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&pDlg));

  if (SUCCEEDED(hr))
  {
    // Create an event handling object, and hook it up to the dialog.
    IFileDialogEvents *pfde = nullptr;
    CComPtr<IShellItem> psiFolder;

    wchar_t buffer[256];
    GetCurrentDirectoryW(sizeof(buffer), buffer);
    std::wstring xws(buffer);
    xws.append(L"\\Assets\\");
    xws.append(wszPath);

    hr = SHCreateItemFromParsingName(xws.c_str(), nullptr, IID_PPV_ARGS(&psiFolder));
    if (SUCCEEDED(hr))
    {
      /*COMDLG_FILTERSPEC aFileTypes[] = {
      { L"Text files", L"*.txt" },
      { L"All files", L"*.*" }
      };*/

      //pDlg->SetFileTypes(_countof(aFileTypes), aFileTypes);
      pDlg->SetTitle(title);
      pDlg->SetFolder(psiFolder);
      pDlg->SetOkButtonLabel(button);
      pDlg->SetDefaultExtension(ext);

      // Show the dialog.
      hr = pDlg->Show(GetDesktopWindow());
      // If the user chose a file, save the user's data to that file.
      if (SUCCEEDED(hr))
      {
        CComPtr<IShellItem> pItem;

        hr = pDlg->GetResult(&pItem);

        if (SUCCEEDED(hr))
        {
          LPOLESTR pwsz = nullptr;

          hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);

          if (SUCCEEDED(hr))
          {
            fileName = pwsz;
            CoTaskMemFree(pwsz);
          }
        }
      }
    }

    pDlg->Release();
  }
  return SUCCEEDED(hr);
}

bool SaveFileDialog(std::wstring &fileName, const wchar_t *ext, const wchar_t *path)
{
  return FileDialog(fileName, ext, path, L"Save", L"Save", CLSID_FileSaveDialog);
}

bool LoadFileDialog(std::wstring &fileName, const wchar_t *ext, const wchar_t *path)
{
  return FileDialog(fileName, ext, path, L"Open", L"Open", CLSID_FileOpenDialog);
}