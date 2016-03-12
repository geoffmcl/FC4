// Fc4List.c
// handle list functions
#include "Fc4.h"

PLE   Add_2_List( PLE ph, PTSTR pf )
{
   size_t len = strlen(pf);
   PTSTR pb;
   if( len > 0 ) {
      PLE pn = MALLOC( sizeof(FL) + len );
      CHECKMEM(pn);
      ZeroMemory(pn, (sizeof(FL) + len));
      pb = (PTSTR) (pn + 1);
      strcpy(pb,pf);
      InsertTailList(ph,pn);
      return pn;
   }
   return NULL;
}

void Expand_Folder( PLE ph, PTSTR pf )
{
   WIN32_FIND_DATA fd;
   size_t len = strlen(pf);
   PTSTR pb;
   HANDLE   hFind;
   if(len == 0) {
      return;
   }
   pb = (PTSTR) MALLOC(264);
   CHECKMEM(pb);
   strcpy(pb,pf);
   Ensure_DOS_Separator(pb);
   if( pb[len-1] != '\\' )
      strcat(pb,"\\");
   len = strlen(pb); // get the PATH part length
   strcat(pb,"*.*");
   hFind = FindFirstFile(pb, &fd);
   if( !VH(hFind) ) {
      MFREE(pb);
      return;
   }
   do {
      if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
         // do we want to iterate?
      } else {
         PTSTR pf2 = GetNxtBuf();
         strcpy(pf2,pb);   // get the PATH
         pf2[len] = 0;
         strcat(pf2, fd.cFileName);
         Add_2_List(ph, pf2);
      }
   } while( FindNextFile(hFind, &fd) );
   FindClose(hFind);
   MFREE(pb);
}

// eof - Fc4List.c
