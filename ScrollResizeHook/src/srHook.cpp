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

#include "srHook.h"

float srHook::resizeFactor = 8.0f;

BOOL WINAPI srHookDllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
  static bool attached = false;

  switch (fdwReason)
  { 
    case DLL_PROCESS_ATTACH:
      if (!attached)
      {
        srHook::setResizeFactor(8.0f);
        attached = true;
      }
      break;

    case DLL_PROCESS_DETACH:
      attached = false;
      break;
  }
  return TRUE;
}

LRESULT CALLBACK srHook::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode < 0)
  {
    return CallNextHookEx(NULL, nCode, wParam, lParam);
  }
  
  if (wParam == WM_MOUSEWHEEL)
  {
    MSLLHOOKSTRUCT *hookData = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

    HWND window = WindowFromPoint(hookData->pt);

    switch (DefWindowProc(window, WM_NCHITTEST, 0,
                          MAKELPARAM(hookData->pt.x, hookData->pt.y)))
    {
    case HTCAPTION:
    case HTCLOSE:
    case HTMAXBUTTON:
    case HTMINBUTTON:
      {
        short int delta = GET_WHEEL_DELTA_WPARAM(hookData->mouseData);
        float mouseTicks = static_cast<float>(delta) / 120.0f;

        RECT windowRect;

        GetWindowRect(window, &windowRect);

        float width = static_cast<float>(abs(windowRect.right - windowRect.left));
        float height = static_cast<float>(abs(windowRect.bottom - windowRect.top));

        float ratio = width / height;

        HDWP hDWP = BeginDeferWindowPos(1);
        DeferWindowPos(hDWP, window, NULL, 0, 0,
          static_cast<int>(width + mouseTicks * resizeFactor * ratio),
          static_cast<int>(height + mouseTicks * resizeFactor / ratio),
          SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
        EndDeferWindowPos(hDWP);
      }
      return 1; //To prevent message propogation

    default:
      break;
    }
  }

  return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void srHook::setResizeFactor(float resizeFactor)
{
  srHook::resizeFactor = resizeFactor;
}
