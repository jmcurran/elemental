#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "rc.h"
#include "wndproc.h"

#define MIR(X) MAKEINTRESOURCE(X)
#define LOGO_WIDTH 520
#define LOGO_HEIGHT 249
#define IDEDIT 32000

// Global variables
char szWndClass[]      =  "WndClass";
HANDLE hInst;
HMENU hMenuMain;

void DBaseWizard(HWND hwnd, char *szFileName, dbaseinfo *dbase);
void hotel(char *fname, char *result);
BOOL CALLBACK AboutDlgProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SampleDlgProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND  hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
                       LPSTR lpszCmdLine, int nCmdShow)
{
   HWND      hwnd;
   MSG       msg;
   WNDCLASS  wndclass;

   hInst = hInstance;

   if(!hPrevInstance)
   {
      // Register the Frame window class

      wndclass.style          = CS_HREDRAW | CS_VREDRAW;
      wndclass.lpfnWndProc    = WndProc;
      wndclass.cbClsExtra     = 0;
      wndclass.cbWndExtra     = 0;
      wndclass.hInstance      = hInstance;
      wndclass.hIcon          = LoadIcon(hInstance, MIR(IDI_AELT));
      wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
      wndclass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
      wndclass.lpszMenuName   = NULL;
      wndclass.lpszClassName  = szWndClass;

      RegisterClass(&wndclass);
   }

   hMenuMain=LoadMenu(hInstance, MIR(IDM_MAIN));

   hwnd = CreateWindow(szWndClass, "Elementary (Modified) v2.0",
                                   WS_OVERLAPPEDWINDOW ,
                                   CW_USEDEFAULT, CW_USEDEFAULT,
                                   LOGO_WIDTH, 2*LOGO_HEIGHT+10,
                                   NULL, hMenuMain, hInstance, NULL);

   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   while(GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   return msg.wParam;
} // WinMain

void DrawBitmap(HDC hdc, HBITMAP hBitmap, int xStart, int yStart)
{
  BITMAP bm;
  HDC hdcMem;
  POINT ptSize, ptOrg;

  hdcMem=CreateCompatibleDC(hdc);
  SelectObject(hdcMem, hBitmap);
  SetMapMode(hdcMem, GetMapMode(hdc));

  GetObject(hBitmap, sizeof(BITMAP), (LPVOID)&bm);

  ptSize.x = bm.bmWidth;
  ptSize.y = bm.bmHeight;
  DPtoLP(hdc, &ptSize, 1);

  ptOrg.x=0;
  ptOrg.y=0;
  DPtoLP(hdcMem, &ptOrg, 1);

  BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, ptOrg.x, ptOrg.y, SRCCOPY);

  DeleteDC(hdcMem);
}


LRESULT CALLBACK WndProc(HWND  hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  static OPENFILENAME ofn;
  static hwndEdit;
  static char szOutput[900];
  static char szBuf[3000];
  char szDirName[256];      // directory string
  char szFile[256];         // filename string
  char szFileTitle[256];    // file - title string
  char szFilter[]="Comma Separated Value (CSV) Files (*.csv)\0*.csv\0Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";       // Filter string
  static HDC hdc;
  PAINTSTRUCT ps;
  static HBITMAP hbmp;

 switch(LOWORD(message))
 {
  case WM_CREATE:
    szOutput[0]='\0';
    hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(LOGO));

    hwndEdit=CreateWindowEx(0L, "EDIT", "",
                             WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_LEFT |
                             ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                             1, LOGO_HEIGHT+1, LOGO_WIDTH, LOGO_HEIGHT,
                             hwnd, (HMENU)IDEDIT, hInst, (LPVOID)NULL);

    SendMessage(hwndEdit, WM_SETFONT, GetStockObject(SYSTEM_FIXED_FONT), MAKELPARAM(TRUE,0));
    SetWindowText(hwndEdit, (LPCTSTR)"Elementary v2.0 \r\n(C) 1996-2005 James M. Curran\0");
    SendMessage(hwndEdit, EM_FMTLINES, (WPARAM)TRUE, 0L);

    return 0;

  case WM_COMMAND:
    switch(LOWORD(wParam))
    {
      case MAIN_FILE_OPEN:
				GetCurrentDirectory(sizeof(szDirName),szDirName);
				szFile[0]='\0';
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner   = hwnd;
				ofn.lpstrFilter  = szFilter;
				ofn.nFilterIndex = 1;
				ofn.lpstrFile   = szFile;
				ofn.nMaxFile    = sizeof(szFile);
				ofn.lpstrFileTitle   = szFileTitle;
				ofn.nMaxFileTitle    = sizeof(szFileTitle);
				ofn.lpstrInitialDir = szDirName;
				ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
			
				if(GetOpenFileName(&ofn))
				{
					SetWindowText(hwndEdit, "");
					
					hotel(szFile, szOutput);
					sprintf(szBuf, "File: %s\r\n=====\r\n\r\nTSQ         F            df1  df2  Pr(TSQ>F)\r\n==========  ===========  ===  ===  =======\r\n%s",
						szFile, szOutput);
					SetWindowText(hwndEdit, szBuf);
					InvalidateRect(hwnd, NULL, TRUE);
				}
			 return 0;

      case MAIN_FILE_EXIT:  // End the program
        SendMessage(hwnd, WM_CLOSE, (WPARAM)0, (LPARAM)0L);
        return 0;

      case IDM_CONTENTS:
        WinHelp(hwnd, "element.hlp", HELP_INDEX, NULL);
        return 0;

      case IDM_SEARCH:
        WinHelp(hwnd, "element.hlp", HELP_PARTIALKEY, (DWORD)"");
        return 0;

      case IDM_HELPHELP:
        WinHelp(hwnd, "winhelp.hlp", HELP_HELPONHELP, 0);
        return 0;

      case IDM_ABOUT:
        if(DialogBox(hInst, "ABOUT_DLG", hwnd, (DLGPROC)AboutDlgProc))
        return 0;
    } // switch
    break; // case WM_COMMAND

  case WM_PAINT:
    hdc=BeginPaint(hwnd, &ps);
    DrawBitmap(hdc, hbmp, 0, 0);
    EndPaint(hdc, &ps);
    return 0;

  case WM_DESTROY:
    PostQuitMessage(0);
    DeleteObject(hbmp);
    return 0;
 } // switch(message)

 // Pass unprocesed messages to DefWindow

 return DefWindowProc(hwnd, message, wParam, lParam);
}



BOOL CALLBACK AboutDlgProc(HWND hdlg, UINT message, WPARAM wParam,
                                 LPARAM lParam)
{

  switch(LOWORD(message))
  {
    case WM_INITDIALOG:
      return TRUE;

    case WM_COMMAND:
      EndDialog(hdlg, TRUE);
      return TRUE;
  }
  return FALSE;
}


