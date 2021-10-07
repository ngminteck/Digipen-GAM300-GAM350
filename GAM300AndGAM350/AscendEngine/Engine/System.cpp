/******************************************************************************/
/*!
\file   System.cpp
\author Alfi Ramli
\par    email: alfi.ramli\@digipen.edu
\par    Digipen login: alfi.ramli
\date   02/02/2020
\brief
Contains definitions required for System

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"
#include "GL/wglext.h"

float oldGlobalVol = 1.0f;
const std::vector<std::string> audio_ext = { ".ogg", ".wav" };
const std::vector<std::string> model_ext = { ".model" };
const std::vector<std::string> texture_ext = { ".tga", ".png" };

bool inFocus = true, closeTriggered = false;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DragDropFiles(HWND, UINT, WPARAM, LPARAM);
void ResizeFunction(HWND, UINT, WPARAM, LPARAM);
std::string SelectDir(const std::string& ext);

#define LoadWGLFunction( x ) (x = reinterpret_cast<decltype(x)>(wglGetProcAddress( #x )))
PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = nullptr;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = nullptr;

System::System(_In_ HINSTANCE hInstance, _In_ int cmdShow, bool menu, bool console) :
  hInst(hInstance), nCmdShow(cmdShow), winWidth(1280), winHeight(720),
  fbWidth(1280), fbHeight(720), isConsoleActive(console),
  defaultWidth(winWidth), defaultHeight(winHeight), isFullScreen(false), dt(0.0f)
{
  // Initialize global strings
  LoadStringA(hInstance, IDS_APP_TITLE, szTitle, ASC_MAX_LOADSTRING);
  LoadStringA(hInstance, IDC_ASCENDENGINE, szWindowClass, ASC_MAX_LOADSTRING);

  RegisterWindow(hInstance, menu);
  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

//#ifndef _SUBMISSION
  if (isConsoleActive)
  {
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
  }
//#endif
  // set the pixel format we want
  pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),  // size of structure
    1,                              // default version
    PFD_DRAW_TO_WINDOW |            // window drawing support
    PFD_SUPPORT_OPENGL |            // OpenGL support
    PFD_DOUBLEBUFFER,               // double buffering support
    PFD_TYPE_RGBA,                  // RGBA color mode
    (BYTE)32,                       // 32 bit color mode
    (BYTE)0, (BYTE)0, (BYTE)0,
    (BYTE)0, (BYTE)0, (BYTE)0,      // ignore color bits, non-palettized mode
    (BYTE)0,                        // no alpha buffer
    (BYTE)0,                        // ignore shift bit
    (BYTE)0,                        // no accumulation buffer
    (BYTE)0, (BYTE)0,
    (BYTE)0, (BYTE)0,               // ignore accumulation bits
    (BYTE)24,                       // 24 bit z-buffer size
    (BYTE)8,                        // no stencil buffer
    (BYTE)0,                        // no auxiliary buffer
    PFD_MAIN_PLANE,                 // main drawing plane
    (BYTE)0,                        // reserved
    0, 0, 0 };                      // layer masks ignored

  RECT desktop;
  // Get a handle to the desktop window
  const HWND hDesktop = GetDesktopWindow();
  // Get the size of screen to the variable desktop
  GetWindowRect(hDesktop, &desktop);
  fsHeight = desktop.bottom; //1200.0f
  fsWidth = desktop.right;   //1920.0f

  //Force 4k
  //fsHeight = 2160.0f;
  //fsWidth = 3840.0f;

  srand(NULL);
}

System::~System()
{
  if (isConsoleActive)
    FreeConsole();
}

int System::Begin(bool fullscreen)
{
  // Perform application initialization:
  if (!InitInstance(hInst, nCmdShow))
    throw std::exception("Failed to initalize system");

  //Initalize game engine
  Engine m_engine;
  ImguiWrapper m_guiWrapper(hWnd);
  MSG msg;
  ZeroMem(msg);

  ShowWindow(hWnd, nCmdShow);
  if (fullscreen) 
	  ToggleFullscreen();

  glClearColor(0, 0, 0, 1);
  SwapBuffers(hDC);
  glClearColor(0, 0, 0, 1);

  bool once = false;
#if !_SUBMISSION
  ASC_INPUT.DisplayCursor(false);
#else
  ASC_INPUT.DisplayCursor(true);
#endif

  closeTriggered = false;
  while (!closeTriggered)
  {
    dt = Time();
    dt = ASC_RANGE(dt, 0.0f, 1.0f / 40.0f);

    ProcessWindowsMessages(msg);
    isInFocus = inFocus;

    // Update when active
    if (isInFocus)
    {
      m_guiWrapper.Update(dt);
      m_engine.Update(closeTriggered, dt);
      SwapBuffers(hDC);
    }
    else
      ASC_INPUT.ClearBuffer();
  }
  try
  {
      throw (int)msg.wParam;

  }
  catch (int e)
  {
    UNREFERENCED_PARAMETER(e);
  }
  return  (int)msg.wParam;
}

void System::ToggleFullscreen()
{
  isFullScreen = !isFullScreen;

  DWORD dwstyle = !isFullScreen ? WS_OVERLAPPEDWINDOW : (WS_OVERLAPPED | WS_POPUP);
  RECT rect;
  rect.left = 0; rect.top = 0;

  if (isFullScreen)
  {
    rect.right = fsWidth;
    rect.bottom = fsHeight;
    SetWindowLongPtr(hWnd, GWL_STYLE, WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
  }
  else
  {
    rect.right = defaultWidth;
    rect.bottom = defaultHeight;
    SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE);
  }

  AdjustWindowRect(&rect, dwstyle, FALSE);
  MoveWindow(hWnd, 0, 0, rect.right, rect.bottom, TRUE);
}

bool System::GetFullScreen() const
{
  return isFullScreen;
}

void System::SetWindowSize(int Width, int Height)
{
  winWidth = Width;
  winHeight = Height;
}

void System::SetRenderSize(int Width, int Height)
{
  fbWidth = Width; fbHeight = Height;
}

void System::GetWindowPos(int & x, int & y) const
{
  RECT rect;
  GetWindowRect(hWnd, &rect);
  x = rect.left;
  y = rect.top;
}

float System::GetDT() const
{
  return dt;
}

ATOM System::RegisterWindow(HINSTANCE hInstance, bool showMenu) const
{
  WNDCLASSEXA wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
//#ifndef _SUBMISSION
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  /*#else
    wcex.hCursor = LoadCursorFromFile("./Assets/Textures/Cursor_albedo.cur");
#endif*/

  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = showMenu ? MAKEINTRESOURCEA(IDC_ASCENDENGINE) : NULL;
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

  return RegisterClassExA(&wcex);
}

static bool WGLExtensionSupported(const char *extension_name)
{
  // this is pointer to function which returns pointer to string with list of all wgl extensions
  PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = nullptr;

  // determine pointer to wglGetExtensionsStringEXT function
  _wglGetExtensionsStringEXT = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(wglGetProcAddress("wglGetExtensionsStringEXT"));

  if (!strstr(_wglGetExtensionsStringEXT(), extension_name))
  {
    // string was not found
    return false;
  }

  // extension is supported
  return true;
}

static void LoadWGLExtensions(const char *window_class_name = nullptr)
{
#if 1
  // Create dummy context
  WNDCLASSEX dummy_wc = { sizeof(WNDCLASSEX), CS_OWNDC, nullptr, 0, 0, GetModuleHandle(nullptr), NULL, LoadCursor(NULL, IDC_ARROW), 0, NULL, (LPCSTR)window_class_name, NULL };
  //WNDCLASSEX dummy_wc = { sizeof(WNDCLASSEX), CS_OWNDC, nullptr, 0, 0, GetModuleHandle(nullptr), NULL, LoadCursorFromFile("./Assets/Textures/Cursor_albedo.cur"), 0, NULL, (LPCSTR)window_class_name, NULL };
  RegisterClassEx(&dummy_wc);

  HWND dummy_hwnd = CreateWindowExA(
    0L,
    window_class_name,
    "dummy window",
    WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    CW_USEDEFAULT, CW_USEDEFAULT,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    NULL,
    NULL,
    NULL,
    NULL
  );

  assert(dummy_hwnd != nullptr);

  HDC dummy_hdc = GetDC(dummy_hwnd);

  // Get pixel format
  PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),  // size of structure
    1,                              // default version
    PFD_DRAW_TO_WINDOW |            // window drawing support
    PFD_SUPPORT_OPENGL |            // OpenGL support
    PFD_DOUBLEBUFFER,               // double buffering support
    PFD_TYPE_RGBA,                  // RGBA color mode
    (BYTE)32,                       // 32 bit color mode
    (BYTE)0, (BYTE)0, (BYTE)0,
    (BYTE)0, (BYTE)0, (BYTE)0,      // ignore color bits, non-palettized mode
    (BYTE)0,                        // no alpha buffer
    (BYTE)0,                        // ignore shift bit
    (BYTE)0,                        // no accumulation buffer
    (BYTE)0, (BYTE)0,
    (BYTE)0, (BYTE)0,               // ignore accumulation bits
    (BYTE)24,                       // 24 bit z-buffer size
    (BYTE)8,                        // no stencil buffer
    (BYTE)0,                        // no auxiliary buffer
    PFD_MAIN_PLANE,                 // main drawing plane
    (BYTE)0,                        // reserved
    0, 0, 0 };                      // layer masks ignored

                                    // Set dummy pixel format
  int dummy_pixel_format = ChoosePixelFormat(dummy_hdc, &pfd);
  assert(dummy_pixel_format != 0);
  SetPixelFormat(dummy_hdc, dummy_pixel_format, &pfd);

  // Create dummy wgl context and make current
  HGLRC dummy_wgl_context = wglCreateContext(dummy_hdc);
  wglMakeCurrent(dummy_hdc, dummy_wgl_context);
#endif
  // Load required wgl extensions here
  LoadWGLFunction(wglGetExtensionsStringEXT);
  LoadWGLFunction(wglSwapIntervalEXT);
  LoadWGLFunction(wglGetSwapIntervalEXT);

  //if (WGLExtensionSupported("WGL_EXT_swap_control"))
  //{
  //  // Extension is supported, init pointers.
  //  wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));

  //  // this is another function from WGL_EXT_swap_control extension
  //  wglGetSwapIntervalEXT = reinterpret_cast<PFNWGLGETSWAPINTERVALEXTPROC>(wglGetProcAddress("wglGetSwapIntervalEXT"));
  //}

#if 1
  // Destroy the dummy window and context
  wglMakeCurrent(dummy_hdc, nullptr);
  wglDeleteContext(dummy_wgl_context);
  ReleaseDC(dummy_hwnd, dummy_hdc);
  DestroyWindow(dummy_hwnd);
#endif
}

bool System::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  LoadWGLExtensions(szWindowClass);

  //hWnd = CreateWindowA(szWindowClass, szTitle, WS_TILEDWINDOW | WS_MAXIMIZE ,
  //                     0, 0, winWidth, winHeight, nullptr, nullptr, hInstance, nullptr);
  hWnd = CreateWindowExA(WS_EX_ACCEPTFILES, szWindowClass, szTitle, WS_TILEDWINDOW | WS_MAXIMIZE,
    0, 0, winWidth, winHeight, nullptr, nullptr, hInstance, nullptr);

  if (!hWnd) return false;

  hDC = GetDC(hWnd);

  pixelFormat = ChoosePixelFormat(hDC, &pfd); // choose best matching pixel format
  if (!SetPixelFormat(hDC, pixelFormat, &pfd)) // set pixel format to device context
  {
    MessageBox(NULL, (LPCSTR)"Unable to set pixel format", (LPCSTR)"Error", MB_OK | MB_ICONEXCLAMATION);
    return false;
  }
  
  //UpdateWindow(hWnd);

  InitalizeGL();

  return true;
}

bool System::InitalizeGL() const
{
  HGLRC m_hRC;
  /* create the OpenGL rendering context */
  if (!(m_hRC = wglCreateContext(hDC)))
  {
    MessageBox(NULL, (LPCSTR)"Unable to create OpenGL rendering context", (LPCSTR)"Error", MB_OK | MB_ICONEXCLAMATION);
    return false;
  }

  /* now make the rendering context the active one */
  if (!wglMakeCurrent(hDC, m_hRC))
  {
    MessageBox(NULL, (LPCSTR)"Unable to activate OpenGL rendering context", (LPCSTR)"ERROR", MB_OK | MB_ICONEXCLAMATION);
    return false;
  }

  return true;
}

void System::ProcessWindowsMessages(MSG &m_msg) const
{
  // Check message queue
  if (PeekMessage(&m_msg, hWnd, NULL, NULL, PM_REMOVE))
  {
    if (!TranslateAccelerator(m_msg.hwnd, hAccelTable, &m_msg))
    {
      TranslateMessage(&m_msg);
      DispatchMessage(&m_msg);
    }
  }
}

/*
  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)

  PURPOSE:  Processes messages for the main window.

  WM_COMMAND  - process the application menu
  WM_DESTROY  - post a quit message and return
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  if (GUIWndProcHandler(hWnd, message, wParam, lParam))
  {
    return 1;
  }

  switch (message)
  {
  case WM_COMMAND:
  {
    int wmId = LOWORD(wParam);


    // Parse the menu selections:
    switch (wmId)
    {
      //case ID_FILE_SAVE:
      //	ASC_MSGSYS.SendMsg(Msg_UINT(ID_FILE_SAVE, mTYPE_SYSTEM_SHORTCUT));
      //	break;
      //case ID_FILE_OPEN:
      //	ASC_MSGSYS.SendMsg(Msg_UINT(ID_FILE_OPEN, mTYPE_SYSTEM_SHORTCUT));
      //	break;
      //case ID_FILE_NEW:
      //	ASC_MSGSYS.SendMsg(Msg_UINT(ID_FILE_NEW, mTYPE_SYSTEM_SHORTCUT));
      //	break;
      //	//case ID_FILE_OPEN: std::cout << "Open" << std::endl; break;
      //	//case ID_FILE_SAVE: std::cout << "Save" << std::endl; break;
      //case ID_FILE_SAVEAS: std::cout << "Save as" << std::endl; break;
      //case ID_EDIT_UNDO: 
      //  ASC_MSGSYS.SendMsg(Msg_UINT(ID_EDIT_UNDO, mTYPE_SYSTEM_SHORTCUT)); 
      //  break;
      //case ID_EDIT_REDO: std::cout << "Redo" << std::endl; break;
      //case ID_EDIT_CUT: std::cout << "Cut" << std::endl; break;
      //case ID_EDIT_COPY: std::cout << "Copy" << std::endl; break;
      //case ID_EDIT_PASTE: std::cout << "Paste" << std::endl; break;
      //case ID_EDIT_SELECTALL: std::cout << "Sel All" << std::endl; break;
    case ID_FILE_EXIT:


#ifndef _SUBMISSION
       // ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_PAUSE));
      ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_SYSTEM_QUIT));
      //DestroyWindow(hWnd);
#else
      ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_PAUSE));
      ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_QUIT_MENU));
#endif
      break;
      default:
      {
        ASC_MSGSYS.SendMsg(Msg_UINT(wmId, mTYPE_SYSTEM_SHORTCUT));
        return DefWindowProc(hWnd, message, wParam, lParam);
      }
    }
  }
  break;

  case WM_SIZE:
    ResizeFunction(hWnd, message, wParam, lParam);
    break;

  case WM_ACTIVATE:
    if (wParam == WA_INACTIVE && !closeTriggered)
    {
      inFocus = false;
      // Pause engine
      //ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_PAUSE));
	  if (ASC_SOUNDMAN.Inst != NULL)
	  {
		  oldGlobalVol = ASC_SOUNDMAN.GetGlobalVolume();
		  ASC_SOUNDMAN.SetGlobaVolume(0);
		  ASC_SOUNDMAN.Update(0.0f);
	  }
    //minimze the window 
	 // ShowWindow(hWnd, SW_MINIMIZE);
    }
    else
    {
      // Resume
      if (!inFocus && !closeTriggered)
      {
       // ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_RESUME));
        inFocus = true;
        ASC_SOUNDMAN.SetGlobaVolume(oldGlobalVol);
#ifndef _SUBMISSION
        // Reload items
        ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_SYSTEM_GAIN_FOCUS));
#else 
        ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_PAUSE));
#endif
      }
    }
    break;

  case WM_KILLFOCUS: 
  {
    inFocus = false;
  } break;
  case WM_SETFOCUS: 
  {
    inFocus = true; 
  } break;

  case WM_DROPFILES:
    DragDropFiles(hWnd, message, wParam, lParam);
    break;

  case WM_CLOSE:
#if _SUBMISSION
    ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_GAME_PAUSE));
#else
    ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_SYSTEM_QUIT));
#endif
    break;

  case WM_DESTROY:
    closeTriggered = true;
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

void DragDropFiles(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  //std::cout << "Someone tried to drop a file" << std::endl;

  char buffer[MAX_PATH];
  HDROP hDrop = (HDROP)wParam;
  int num_files = DragQueryFileA(hDrop, 0xFFFFFFFF, NULL, NULL);

  //Log("num_files is ", num_files);

	HDROP hDrop2 = (HDROP)wParam;
  for (int i = 0; i < num_files; ++i)
  {
	  ZeroMemory(buffer, MAX_PATH);

    if (DragQueryFileA(hDrop, i, buffer, MAX_PATH))
    {
      std::string src_filepath((char*)buffer);
      //Log(GetFilenameFromPath(src_filepath));
      //Log(GetExtensionFromPath(src_filepath));

      //Log(GetDirFromPath(src_filepath));
      //Log(filename);
      const std::string destination = GetCurrentDir() + "\\Assets\\" + SelectDir(GetExtensionFromPath(src_filepath));
      const std::string filename = GetFilenameFromPath(src_filepath);
      //Log(destination);
      DWORD flags = COPY_FILE_FAIL_IF_EXISTS | COPY_FILE_NO_BUFFERING | COPY_FILE_OPEN_SOURCE_FOR_WRITE;
      if (CopyFileExA(src_filepath.data(),
        (destination + filename).data(),
        NULL, nullptr, NULL, flags))
      {
        Log("Copied to", destination,"successfully");
        ASC_MSGSYS.SendMsg(MsgData_Base(mTYPE_DRAGDROP));
      }
      else
      {
        DWORD err = GetLastError();
        if (err == 80)
          LogError("File", filename, "already existed in ", destination);
        else
          LogError("CopyFileExA failed error code : ", err);
      }
    }
    else
    {
      LogError("DragQueryFileA failed error code : ", GetLastError());
    }
  }

  DragFinish(hDrop);
}

void ResizeFunction(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int nWidth = LOWORD(lParam);
  int nHeight = HIWORD(lParam);
  //Log("Someone tried to resize (", nWidth, ":", nHeight, ")");
  inFocus = (nWidth == 0) ? false : true;
  ASC_SYSTEM.SetWindowSize(nWidth, nHeight);
  ASC_MSGSYS.SendMsg(Msg_Resize(nWidth, nHeight));
}

std::string SelectDir(const std::string & ext)
{
  bool audio = std::find(audio_ext.begin(), audio_ext.end(), ext) == audio_ext.end() ? false : true;
  bool model = std::find(model_ext.begin(), model_ext.end(), ext) == model_ext.end() ? false : true;
  bool texture = std::find(texture_ext.begin(), texture_ext.end(), ext) == texture_ext.end() ? false : true;

  if (audio)
    return "Audios";
  else if (model)
    return  "Models";
  else if (texture)
    return "Textures";
  else
  {
    const std::string error("File(Ext) is Not Audio/Model/Texture");
    LogError(error);
    return error;
  }
}
