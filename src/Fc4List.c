/*\
 * Fc4List.c
 *
 * Copyright (c) 2008 - 2016 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
// handle list functions
#include "Fc4.h"
#include <dirent.h>

#ifdef WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

PLE   Add_2_List( PLE ph, PTSTR pf )
{
   size_t len = strlen(pf);
   PTSTR pb;
   if( len > 0 ) {
      PLE pn = (PLE)MALLOC( sizeof(FL) + len );
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
#if (defined(WIN32) && defined(USE_NATIVE_WIN32))
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
#else
    // TODO: Expand FOLDER into files
    size_t len = strlen(pf);
    PTSTR pb;
    DIR *dp;
    struct dirent *pe;

    pb = (PTSTR) MALLOC(264);
    CHECKMEM(pb);
    strcpy(pb,pf);
#ifdef WIN32
    Ensure_DOS_Separator(pb);
#else
    Ensure_UNIX_Separator(pb);
#endif
    dp = opendir(pb);
    if (dp) {
        while ((pe = readdir(dp)) != 0) {
            if (strcmp(pe->d_name,".") == 0)
                continue;
            if (strcmp(pe->d_name,"..") == 0)
                continue;
            pb[len] = 0;
            strcat(pb,PATH_SEP);
            strcat(pb,pe->d_name);
            if (is_file_or_directory(pb) == 1) {
                Add_2_List( ph, pb );
            }
        }

        closedir(dp);
    }
    MFREE(pb);

#endif
}

// eof - Fc4List.c
