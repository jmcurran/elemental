#include <windows.h>

// Create a data class to store information

#ifndef _dbase
#define _dbase
typedef struct dbaseinfo{
   char fname[512];
   int step;
   char **Elements;
   int nElements;
   BOOL *fElements;
   BOOL fDbase;
} dbaseinfo;
#endif
