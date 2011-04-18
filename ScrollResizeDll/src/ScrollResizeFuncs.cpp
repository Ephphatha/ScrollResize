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

#include "ScrollResizeFuncs.h"

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  static bool onTitleBar = false;
  static HWND window = NULL;

  if (nCode < 0)
  {
    return CallNextHookEx(NULL, nCode, wParam, lParam);
  }
  
  MOUSEHOOKSTRUCTEX *hookData = reinterpret_cast<MOUSEHOOKSTRUCTEX*>(lParam);

  switch (wParam)
  {
  case WM_MOUSEWHEEL:
    {
      if (onTitleBar)
      {
        short int delta = GET_WHEEL_DELTA_WPARAM(hookData->mouseData);
        float scalingFactor = 1.0f + static_cast<float>(delta) / 1200.0f;

        RECT windowRect;

        GetWindowRect(window, &windowRect);

        HDWP hDWP = BeginDeferWindowPos(1);
        DeferWindowPos(hDWP, window, NULL, 0, 0,
          static_cast<int>(static_cast<float>(windowRect.right - windowRect.left) * scalingFactor),
          static_cast<int>(static_cast<float>(windowRect.bottom - windowRect.top) * scalingFactor),
          SWP_NOMOVE|SWP_NOZORDER);
        EndDeferWindowPos(hDWP);
      }
    }
    break;
    
  default:
    {
      if (hookData->wHitTestCode == HTCAPTION)
      {
        onTitleBar = true;
        window = hookData->hwnd;
      }
      else
      {
        onTitleBar = false;
      }
    }
    break;
  }

  return CallNextHookEx(NULL, nCode, wParam, lParam);
}
//
//LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
//{
//  if (nCode < 0)
//  {
//    return CallNextHookEx(NULL, nCode, wParam, lParam);
//  }
//
//  if(window && wParam == WM_MOUSEWHEEL)
//  {
//    MSLLHOOKSTRUCT *hookData = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
//
//    short int delta = GET_WHEEL_DELTA_WPARAM(hookData->mouseData);
//    float scalingFactor = 1.0f + static_cast<float>(delta) / 1200.0f;
//
//    RECT windowRect;
//
//    GetWindowRect(window, &windowRect);
//
//    HDWP hDWP = BeginDeferWindowPos(1);
//    DeferWindowPos(hDWP, window, NULL, 0, 0,
//      static_cast<int>(static_cast<float>(windowRect.right - windowRect.left) * scalingFactor),
//      static_cast<int>(static_cast<float>(windowRect.bottom - windowRect.top) * scalingFactor),
//      SWP_NOMOVE|SWP_NOZORDER);
//    EndDeferWindowPos(hDWP);
//  }
//
//  return CallNextHookEx(NULL, nCode, wParam, lParam);
//}
