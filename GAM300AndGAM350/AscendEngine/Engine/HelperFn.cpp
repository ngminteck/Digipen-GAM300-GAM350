/*______________________________________________________________________________

@file     HelperFn.h
@author   Alfi Ramli  (alfi.ramli, alfi.ramli@digipen.edu)

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of
Technology is prohibit-ed.
_____________________________________________________________________________*/

#include "pch.h"


std::string GetDirFromPath(const std::string& input)
{
  std::string directory;
  const size_t last_slash_idx = input.rfind('\\');
  if (std::string::npos != last_slash_idx)
  {
    directory = input.substr(0, last_slash_idx);
  }
  return directory;
}

std::string GetExtensionFromPath(const std::string & input)
{
  std::string filename = input;
  SeperateBy(".", filename);
  return filename;
}

std::string GetFilenameFromPath(const std::string & input)
{
  std::string filename = input;
  SeperateBy("\\", filename);
  return filename;
}

void SeperateBy(const std::string& seperator, std::string & input)
{
  std::size_t pos = input.rfind(seperator);
  assert(pos != std::string::npos);
  input = input.substr(pos);

  ////extract filename
  //while (pos != std::string::npos)
  //{
  //  input = input.substr(pos + 1);
  //  pos = input.find_last_of(seperator);
  //}
}

std::string GetCurrentDir()
{
  char buffer[MAX_PATH];
  GetCurrentDirectoryA(MAX_PATH, buffer);
  std::string ws(buffer);
  return ws;
}

std::vector<std::string> GetFilesInDir(const std::string& dir, const std::string& ext, const std::string& prefix, const char *contain)
{
  std::vector<std::string> result;

  WIN32_FIND_DATAA data;
  std::string app(GetCurrentDir() + "/" + dir + "/*" + prefix + "." + ext);
  //std::string appended((const wchar_t*)&app[0], sizeof(char) / sizeof(wchar_t)*app.size());
  HANDLE hFind = FindFirstFileA(app.c_str(), &data);
  if (INVALID_HANDLE_VALUE != hFind)
  {
    do
    {
      if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
        std::string ws(data.cFileName);
        if (contain)
        {
          if (ws.find(contain) == ws.npos)
            continue;
        }
        result.push_back(ws);
      }
    } while (FindNextFileA(hFind, &data) != 0);
    FindClose(hFind);
  }
  else
  {
    // Report failure.
  }
  return result;
}


bool compare_str(const std::string& a, const std::string& b)
{
  if (a.compare(b) == 0)
    return true;

  return false;
}

std::size_t find_str(const std::string & findthis, const std::string & inthis)
{
  std::size_t found = inthis.find(findthis);
  return found;
}

/*
if dir already have a filename embedded with it,
just pass in an empty string ("") in to the param filename
*/
bool DeleteFileInDir(const std::string& dir, const std::string & filename)
{
  std::string app(GetCurrentDir() + dir + filename);
  if (!DeleteFile((LPCSTR)app.c_str()))
  {
    DWORD le = GetLastError();
    Log(app, "\nfile fails to be deleted\n", "Error: ", le);
    return false;
  }
  else
    return true;
}

static const char *lastFile = "_last.txt";

std::string GetLastOpenedFile()
{
#if _SUBMISSION
  return "Splash";
#else
  std::string fileName;
  std::ifstream lf;
  char delim = '\0';
  size_t pos;
  lf.open(lastFile);
  if (lf.is_open())
  {
      std::getline(lf, fileName);
      pos = fileName.find(delim);
      if(pos != std::string::npos)
      {
          return fileName.substr(0, pos);
      }
  }
  else
  {
    // Hard code to first level
    LogWarning("No _LAST file found, using default splash");

    fileName = "Level1";

  }
  return fileName;
#endif
}

void SetLastOpenedFile(const std::string &fileName)
{
  std::ofstream ofs(lastFile);
  if (ofs.good())
  {
    ofs << fileName;
  }
}

float Time()
{
  static int first = 1;
  static LARGE_INTEGER prev;
  static double factor;

  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);

  if (first)
  {
    first = 0;
    prev = now;
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    factor = 1.0 / (double)freq.QuadPart;
  }

  float elapsed = (float)((double)(now.QuadPart - prev.QuadPart) * factor);
  prev = now;
  return elapsed;
}