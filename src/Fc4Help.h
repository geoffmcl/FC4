/*\
 * Fc4Help.h
 *
 * Copyright (c) 2015 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
#ifndef  _Fc4Help_hhh_
#define  _Dc4Help_hhh_

extern int ProcessCommand( WS, int argc, char * argv[] );
extern void  Show_CMD_Line( void );
extern void prt_help_message(void);
extern HANDLE	OpenForRead( LPTSTR lpf );
#if 0 // 00000000000000000000000000000000000000000
#ifdef USE_INT64_TYPE  // FIX20080819 - add some 64-bit support
extern off64_t	RetFileSize( HANDLE hFile ); // FIX20080819 - add some 64-bit support
#else // !USE_INT64_TYPE
extern DWORD	RetFileSize( HANDLE hFile );
#endif // USE_INT64_TYPE y/n
#endif // 000000000000000000000000000000000000000
extern int   is_valid_path( PTSTR pf );

extern TCHAR	szOut[];   // = [MAX_PATH];
extern TCHAR	szModule[]; // = "Dummy";

#endif // #ifndef  _Fc4Help_hhh_
// eof - Fc4Help.h
