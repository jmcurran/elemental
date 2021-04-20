#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "rc.h"
#include "wndproc.h"

extern HANDLE hInst;


void Wizard1(HWND hdlg, dbaseinfo *dbase, HWND *hwndEltChex)
{
  FILE *fp;
  char s[2000], tmp[2000];
  char *p;
  int nElts;
  int i, j;
  int nCols, nRows;
  int top, left, cbHeight, cbWidth;

  fp=fopen(dbase->fname, "r");
  fgets(s, 2000, fp);
  strcpy(tmp, s);
  nElts=1;
  p=strtok(tmp, ",");
  while(p)
  {
    p=strtok(NULL, ",");
    if(p)
      nElts++;
  }

  dbase->nElements=nElts;
  dbase->Elements=(char **)malloc(nElts*sizeof(char *));
  dbase->fElements=(BOOL *)malloc(dbase->nElements*sizeof(BOOL));
  nElts=0;
  p=strtok(s, ",");
  dbase->Elements[nElts]=(char *)malloc(3*sizeof(char));
  strncpy(dbase->Elements[nElts], p, 2);
  dbase->Elements[nElts][2]='\0';
  dbase->fElements[nElts]=FALSE;
  while(p)
  {
    p=strtok(NULL, ",");
    if(p)
    {
      nElts++;
      dbase->Elements[nElts]=(char *)malloc(3*sizeof(char));
      strncpy(dbase->Elements[nElts], p, 2);
      dbase->Elements[nElts][2]='\0';
      dbase->fElements[nElts]=FALSE;
    }
  }

  hwndEltChex=(HWND)malloc(dbase->nElements*sizeof(HWND));

  top=40;
  left=50;
  cbHeight=50;
  cbWidth=60;

  nCols=dbase->nElements/3 + (dbase->nElements%3);
  nRows=3;

  for(i=0;i<nCols;i++)
    for(j=0;j<nRows;j++)
      if((i*nRows)+j<dbase->nElements)
        hwndEltChex[(i*nRows)+j]=CreateWindowEx(0L, "BUTTON", dbase->Elements[(i*nRows)+j],
                             WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                             left+(i*cbWidth), top+(j*cbHeight), cbWidth, cbHeight,
                             hdlg, (HMENU)(10005+(i*nRows)+j), hInst, (LPVOID)NULL);

}





BOOL CALLBACK WizardDlgProc(HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  static HWND *hwndEltChex;
  static dbaseinfo *dbase;
  int i;

  switch(LOWORD(message))
  {
    case WM_INITDIALOG:
    // Get the info
      dbase=(dbaseinfo *)malloc(sizeof(dbaseinfo));
      dbase=(dbaseinfo *)lParam;
      hwndEltChex=NULL;

      switch(dbase->step)
      {
        case 0:
          Wizard1(hdlg, dbase, hwndEltChex);
          break;
      }
      return  TRUE;

    case WM_COMMAND:
      if(LOWORD(wParam)>=10005&&LOWORD(wParam)<=10100) // IDs 10005 through 10100 reserved now
        dbase->fElements[LOWORD(wParam)-10005]=!dbase->fElements[LOWORD(wParam)-10005];
      else
        switch(LOWORD(wParam))
        {
          case IDOK:
            if(hwndEltChex)
            {
              for(i=0;i<dbase->nElements;i++)
                DestroyWindow(hwndEltChex[i]);
              free(hwndEltChex);
            }
            EndDialog(hdlg, TRUE);
            return TRUE;
          case IDCANCEL:
            if(hwndEltChex)
            {
              for(i=0;i<dbase->nElements;i++)
                DestroyWindow(hwndEltChex[i]);
              free(hwndEltChex);
            }
            EndDialog(hdlg, FALSE);
            return FALSE;
          case IDHELP:
            if(hwndEltChex)
            {
              for(i=0;i<dbase->nElements;i++)
                DestroyWindow(hwndEltChex[i]);
              free(hwndEltChex);
            }
            EndDialog(hdlg, TRUE);
            return TRUE;
        }

      return TRUE;
  }
  return FALSE;
}

void DBaseWizard(HWND hwnd, char *szFileName, dbaseinfo *dbase)
// DBase Wizard calls the same dialog procedure each time with a different lparam
// value, that way the wizard knows which step it is up to
{
  static int step;


  step=0;
  strcpy(dbase->fname, szFileName);
  dbase->step=step;

  if(DialogBoxParam(hInst, "WIZARD_DLG", hwnd,
      WizardDlgProc, (LPARAM)(long)dbase))
      dbase->fDbase=TRUE;
  else
      dbase->fDbase=FALSE;

}

