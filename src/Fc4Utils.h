
// Fc4Utils.h
// various utitlity functions
#ifndef  _Fc4Utils_hhh_
#define  _Fc4Utils_hhh_

#ifdef UNICODE
#define  STAT   _wstat64
#define  PRINTF wprintf
#define  SPRINTF swprintf
#define  STRLEN wcslen
#define  STRCAT wcscat
#define  STRCPY wcscpy
#else
#define  STAT   _stat64
#define  PRINTF printf
#define  SPRINTF sprintf
#define  STRLEN strlen
#define  STRCAT strcat
#define  STRCPY strcpy
#endif

extern PTSTR GetNxtBuf( VOID );
extern DWORD GetNxtBufSize( VOID );
extern void  Ensure_DOS_Separator( PTSTR pf );
extern PTSTR   RetFileTitle( LPTSTR lpf );
extern INT  HasChar( PTSTR lpf, TCHAR c );
extern INT  HasDot( PTSTR lpf );
extern INT  HasAst( LPTSTR lpf );
extern INT  IsWild( PTSTR pf );

#define  CHECKMEM(a) if( a == NULL ) {\
   prt("ERROR: MEMORY FAILED!"MEOR);\
   PgmExit(TRUE);\
}

#ifndef _TCHAR
#define  _TCHAR TCHAR
#define  _T(a) a
#endif

extern _TCHAR * get_nice_number( __int64 num, size_t width );
extern void nice_num( _TCHAR * dst, _TCHAR * src ); // get nice number, with commas
extern _TCHAR * get_k_num( __int64 i64, int ascii, int type );


#endif // #ifndef  _Fc4Utils_hhh_
// eof - Fc4Utils.h
