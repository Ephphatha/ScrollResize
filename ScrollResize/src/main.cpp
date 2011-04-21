/*
 *  The MIT License
 *
 *  Copyright 2011 Andrew James <ephphatha@thelettereph.com>.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */

#include <fstream>

#include <Windows.h>

#include "resource.h"

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE previous, LPSTR lpCmdLine, int showCommand)
{
  const wchar_t CLASS_NAME[]  = L"BasicWindow";
    
  WNDCLASS wc = { };

  wc.lpfnWndProc   = WindowProc;
  wc.hInstance     = hInstance;
  wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);

  HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"ScrollResize",    // Window text
        WS_POPUPWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, SW_HIDE);

    NOTIFYICONDATA nid = {};

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 0;
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    nid.uFlags = NIF_ICON | NIF_MESSAGE;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    Shell_NotifyIcon(NIM_ADD, &nid);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Shell_NotifyIcon(NIM_DELETE, &nid);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  static HHOOK mouseHook = NULL;
  static HINSTANCE hinstDLL;

  switch (uMsg)
  {
  case WM_CREATE:
    {
      hinstDLL = LoadLibrary(L"ScrollResizeHook.dll");

      if (hinstDLL)
      {
        HOOKPROC llMouseProc = reinterpret_cast<HOOKPROC>(GetProcAddress(hinstDLL, "LowLevelMouseProc"));

        if (llMouseProc)
        {
          mouseHook = SetWindowsHookEx(WH_MOUSE_LL, llMouseProc, hinstDLL, 0);
        }

        std::ifstream config("config.txt", std::ifstream::in);
        
        if (config.is_open())
        {
          float resizeFactor = 8.0f;

          if (config.good())
          {
            config >> resizeFactor;
          }

          void (*setRF)(float) = reinterpret_cast<void (*)(float)>(GetProcAddress(hinstDLL, "setResizeFactor"));

          if (setRF)
          {
            setRF(resizeFactor);
          }
        }
      }
    }
    return 0;

  case WM_DESTROY:
    UnhookWindowsHookEx(mouseHook);
    FreeLibrary(hinstDLL);
    PostQuitMessage(0);
    return 0;

  case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);

      FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

      EndPaint(hwnd, &ps);
    }
    return 0;

  case WMAPP_NOTIFYCALLBACK:
    {
      switch (lParam)
      {
      case WM_RBUTTONUP:
      case WM_LBUTTONUP:
        HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
        if (hMenu)
        {
          HMENU subMenu = GetSubMenu(hMenu, 0);
          if (subMenu)
          {
            SetForegroundWindow(hwnd);
            UINT uFlags = TPM_RIGHTBUTTON;
            if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
            {
              uFlags |= TPM_RIGHTALIGN;
            }
            else
            {
              uFlags |= TPM_LEFTALIGN;
            }

            POINT mouse = {};
            GetCursorPos(&mouse);

            TrackPopupMenuEx(subMenu, uFlags, mouse.x, mouse.y, hwnd, NULL);
          }
          DestroyMenu(hMenu);
        }
      }
    }
    return 0;

  case WM_COMMAND:
    {
      switch (LOWORD(wParam))
      {
      case ID_FILE_EXIT:
        DestroyWindow(hwnd);
        break;

      default:
        break;
      }
    }
    return 0;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
