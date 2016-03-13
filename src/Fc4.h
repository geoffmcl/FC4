
// Fc4.h
#ifndef	_Fc4_h
#define	_Fc4_h

#include    "Fc4Vers.h" // HISTORY and version
#include    <sys/types.h>
#include    <sys/stat.h>
#ifdef WIN32
#define		WIN32_LEAN_AND_MEAN
#include    <windows.h>
#else   // !WIN32
#include    <inttypes.h>
#include    <string.h>  // for strlen, ...
#include    <ctype.h>   // for toupper
#endif // WIN32 y/n
#include    <stdlib.h>
#include	<stdio.h>
#include    <time.h>
#include    "Fc4List.h"

#ifdef WIN32
#define	   MCRLF	"\r\n"
#define STRCMPI strcmpi
#else // !WIN32
#define	   MCRLF	"\n"
#define STRCMPI strcmp
typedef char TCHAR;
typedef void * HANDLE;
typedef void * LPVOID;
typedef int BOOL;
typedef char * LPTSTR;
typedef char * LPSTR;
typedef char * PTSTR;
typedef unsigned int DWORD;
typedef int INT;
typedef void VOID;
typedef long LONG;
typedef unsigned char BYTE;
#ifndef MAX_PATH
#define MAX_PATH 264
#endif
#define TRUE 1
#define FALSE 0
#define INVALID_HANDLE_VALUE (HANDLE)-1
typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;
#endif // WIN32 y/n

#define	   MEOR  MCRLF

#ifdef USE_INT64_TYPE  // FIX20080819 - add some 64-bit support
#ifndef WIN32
typedef int64_t __int64;   // FIX20160312 - add unix 64-bit support
#endif
typedef __int64 off64_t;   // FIX20080819 - add some 64-bit support
#endif // USE_INT64_TYPE

#define		MXARGS		100
#define		MXCMDBUF		1024
#undef		ADDPREV
#define		ADDLOPT			// -L[nnnn] = Max. lines shown
#define		MXSAF			8
// #define		MXOFLINE		65
#define		MXOFLINE		40
#define  MXDIAGBUF   4096  // use a bigger buffer
#define  MXPATH   (MAX_PATH + 16)

#ifndef		_GTOOLS
#define		_GTOOLS
#define		UL		unsigned long
#endif		/* _GTOOLS */

#define     MXLINES     16 // giLnBuf (for MXLINES)
#define     MXLINE      256
#define     MXLINEB     (MXLINE + 8)
#define     MXLINEB2    (MXLINES * MXLINEB)  // gszLnBuf

extern	TCHAR	gcLastOut;

// This is for carrying around a MAPPED file
// =========================================
typedef struct {
	HANDLE	mp_Hf;
    time_t  mp_Tm;
#ifdef USE_INT64_TYPE  // FIX20080819 - add some 64-bit support
	off64_t 	mp_Sz;   // FIX20080819 - add some 64-bit support
	off64_t  mp_Cnt;  // FIX20080819 - add some 64-bit support
#else // !USE_INT64_TYPE
	DWORD 	mp_Sz;   // FIX20080819 - add some 64-bit support
	DWORD  mp_Cnt;  // FIX20080819 - add some 64-bit support
#endif // USE_INT64_TYPE y/n
	HANDLE	mp_Hd;
	LPVOID	mp_Pv;
#ifdef ADD_FILE_INFO
	BY_HANDLE_FILE_INFORMATION mp_sFI;
	BOOL	   mp_bGFI;
#endif // ADD_FILE_INFO
	TCHAR	   mp_Nm[MAX_PATH];
}MPSTR;
typedef MPSTR * LPMPSTR;

#ifdef WIN32
#define NULLPMPSTR(a)	ZeroMemory( a, sizeof(MPSTR) )
#else // !WIN32
#define NULLPMPSTR(a)   memset(a,0,sizeof(MPSTR))
#endif // WIN32 y/n

#define	VH(a)			( ( a ) && ( a != INVALID_HANDLE_VALUE ) )
#define	Diff(a,b)		( ( a < b ) ? ( b - a ) : ( a - b ) )
#define	EndBuf(a)		( a + strlen(a) )

// Bits for g_dwNotExact
#define		x_SkipSpaces		0x00000001
#define		x_IgnoreCase		0x00000002


// Program ERRORLEVEL (exit) values
// ================================
#define		Exit_Same		0
#define		Exit_Diff		1
#define		Exit_Error		2
#define		Exit_Help		3
#define		Exit_NoSupp		4
// ================================

// Program DEBUG Bits
// ========================================
// New debug stuff
// start with UNDEFINING EVERYTHING
// NO DEBUG IS ON
#undef		DBGDIAG1		// Diag COMPARE service Bit 1
#undef		DBGDIAG2
#undef		DBGDIAG3
#undef		DBGDIAG4
#undef		DBGDIAG5
#undef		DBGDIAG6
#undef  ADD_DEBUG

#ifdef   NDEBUG
// ****************************************************
// it all stays OFF
// ****************************************************
#else // !NDEBUG
// ****************************************************
#ifdef   ADD_DEBUG   // if this is defined
// ALL DEBUG IS ON
#define		DBGDIAG1		// Diag COMPARE service Bit 1
#define		DBGDIAG2
#define		DBGDIAG3
#define		DBGDIAG4
#define		DBGDIAG5
#define		DBGDIAG6
#endif // #ifdef   ADD_DEBUG   // if this is defined
// ****************************************************
#endif   // ifdef   NDEBUG y/n

/* bit values for g_dwDbgBits */
/* first a bit for it - set by -V */
#define		Dbg_Bit01		0x00000001
#define		Dbg_Bit02		0x00000002
#define		Dbg_Bit03		0x00000004
#define		Dbg_Bit04		0x00000008
#define		Dbg_Bit05		0x00000010
#define		Dbg_Bit06		0x00000020
#define		Dbg_Bit07		0x00000040
#define		Dbg_Bit08		0x00000080
#define		Dbg_Bit09		0x00000100
#define		Dbg_Bit10		0x00000200
#define		Dbg_Bit11		0x00000400
#define		Dbg_Bit12		0x00000800
#define		Dbg_Bit13		0x00001000
#define		Dbg_Bit14		0x00002000
#define		Dbg_Bit15		0x00004000
#define		Dbg_Bit16		0x00008000
#define		Dbg_Bit17		0x00010000
#define		Dbg_Bit18		0x00020000
#define		Dbg_Bit19		0x00040000
#define		Dbg_Bit20		0x00080000

#define		Dbg_Max06	(	Dbg_Bit01|Dbg_Bit02|Dbg_Bit03| \
							Dbg_Bit04|Dbg_Bit05|Dbg_Bit06	)

/* then an if( ... ) for it ... */
#define		Dbg1		( g_dwDbgBits & Dbg_Bit01 )
#define		Dbg2		( g_dwDbgBits & Dbg_Bit02 )
#define		Dbg3		( g_dwDbgBits & Dbg_Bit03 )
#define		Dbg4		( g_dwDbgBits & Dbg_Bit04 )
#define		Dbg5		( g_dwDbgBits & Dbg_Bit05 )
#define		Dbg6		( g_dwDbgBits & Dbg_Bit06 )

#define		Dbg07		( g_dwDbgBits & Dbg_Bit07 )
#define		Dbg08		( g_dwDbgBits & Dbg_Bit08 )
#define		Dbg09		( g_dwDbgBits & Dbg_Bit09 )
#define		Dbg10   	( g_dwDbgBits & Dbg_Bit10 )
#define		Dbg11		( g_dwDbgBits & Dbg_Bit11 )
#define		Dbg12		( g_dwDbgBits & Dbg_Bit12 )
#define		Dbg13		( g_dwDbgBits & Dbg_Bit13 )
#define		Dbg14		( g_dwDbgBits & Dbg_Bit14 )
#define		Dbg15		( g_dwDbgBits & Dbg_Bit15 )
#define		Dbg16		( g_dwDbgBits & Dbg_Bit16 )
#define		Dbg17		( g_dwDbgBits & Dbg_Bit17 )
#define		Dbg18		( g_dwDbgBits & Dbg_Bit18 )
#define		Dbg19		( g_dwDbgBits & Dbg_Bit19 )
#define		Dbg20   	( g_dwDbgBits & Dbg_Bit20 )

#ifdef   NDEBUG

#define  SaveDbg
#define  RestDbg
#define	SaveDbg01
#define	RestDbg01

#else // !NDEBUG

#define	SaveDbg(a) \
	g_dwDbgSave |= ( g_dwDbgBits & a );\
	g_dwDbgBits &= ~( a );
#define RestDbg(a) \
	g_dwDbgBits |= ( g_dwDbgSave & a );\
	g_dwDbgSave &= ~( a );

#define	SaveDbg01	SaveDbg(Dbg_Bit01)
#define	RestDbg01	RestDbg(Dbg_Bit01)

#endif   // ifdef   NDEBUG y/n

// ========================================

extern	int	gIRet;

// **********************************************
// The MAIN purpose
// LINE BY LINE COMPARE
// ====================
typedef struct {

	UL	c_dwBgn;	/* offset to beginning of line */
	UL	c_dwEnd;	/* offset to end       of line */
	UL	c_dwLen;	/* LENGTH of the data block    */
	UL	c_dwLns;	/* current LINE number         */
	UL	c_dwI;
	UL	c_dwCLen;	/* character length, excl crlf */

}ALINE;

typedef struct {

	LPMPSTR	lpMP;
#ifdef USE_INT64_TYPE  // FIX20080819 - add some 64-bit support
	off64_t	dwSz;
#else // !USE_INT64_TYPE
	DWORD	   dwSz;
#endif // USE_INT64_TYPE y/n
	LPTSTR	lpBuf;
	/* POSITIONS in the file */
	ALINE	Prev1;
	ALINE	Prev2;
	ALINE	Prev3;

	/* Next is actually the CURRENT active line */
	ALINE	Next;
	/* ======   F1, F2, depending on paf ====== */

	ALINE	Save1;
	ALINE	Save2;
	ALINE	Save3;

	/* Output limits - set in SetDisplay, after compare */
	ALINE	Best;
	ALINE	From;
	ALINE	To;

	/* End of File */
	ALINE	End;

	/* temp save area - only valid in service */
	ALINE	Temp;
	ALINE	Temp2;
}AFILE;

typedef	AFILE	* LPAFILE;

typedef struct tagFL {
   LIST_ENTRY  list;
   TCHAR       name[1];
} FL, * PFL;

typedef struct tagFC4WS {  /* work structure */

	int	ws_iVerbal;    // verbality
	BOOL	ws_bBinary;
	BOOL	ws_bQuick;     // just looking for FIRST difference
	BOOL	ws_bDnHeader;  // have output the header
	BOOL	ws_bDiag1;

	DWORD	ws_dwDbgBits;
	DWORD	ws_dwDbgSave;	/* Save and RESTORE */
	DWORD	ws_dwSaveDbg;	/* g_dwSaveDbg	= W.ws_dwSaveDbg */
	DWORD	ws_dwMinDbg;	/* g_dwMinDbg	= W.ws_dwMinDbg  */

	BOOL	ws_bReverse;   // -R = reverse file names
	int	ws_iPrevCnt;
	BOOL	ws_bAddNums;

	DWORD	ws_dwNotExact; // -X12 Spaces and Case ignores
	DWORD	ws_dwMatchCnt;

	DWORD	ws_dwMaxLines;
	DWORD	ws_dwDbgLine;	/* = g_dwDbgLine (W.ws_dwDbgLine) */

   DWORD ws_dwLastError;   /* last error, if any */

	DWORD	ws_dwF1Lines;
	DWORD	ws_dwF2Lines;

	DWORD	ws_dwB1, ws_dwE1, ws_dwC, ws_dwB2, ws_dwE2;

	BOOL	ws_bFnd1, ws_bFnd2;
	BOOL	ws_bFndIn1, ws_bFndIn2;
   HANDLE	ws_hOutFil;  // ghOutFil

	/* global active saves */
	ALINE	ws_sAct1, ws_sAct2;
	ALINE	ws_sFnd1, ws_sFnd2;
	ALINE	ws_sNxt1, ws_sNxt2;

	/* the two files, F1, F2 and set of positions after compare */
	AFILE	ws_AF1, ws_AF2;

	MPSTR	ws_msFile1, ws_msFile2;

	UL		ws_dwLns1;	/* gdwLns1 */
	UL		ws_dwLns2;	/* gdwLns2 */
   INT   ws_iPath1, ws_iPath2;  // INT - folder 1 & 2
   LIST_ENTRY  ws_sList1, ws_sList2;   // gsList1, gsList2
   INT   ws_iLnBuf;  // giLnBuf (for MXLINES)
   TCHAR ws_szLnBuf[MXLINEB2];   // gszLnBuf

   TCHAR ws_szDiag[MXDIAGBUF]; // gszDiag - a BIG buffer - 4096
   TCHAR ws_szTemp[MXDIAGBUF]; // gszTemp - a BIG buffer - 4096
   TCHAR ws_szDLn1[1024]; // gszDLn1 - an output LINE buffer - 1024
   TCHAR ws_szDLn2[1024]; // gszDLn2 - an output LINE buffer - 1024

   TCHAR ws_szPath1[MXPATH];  // gszPath1 - folder 1
   TCHAR ws_szPath2[MXPATH];  // gszPath2 - folder 2
   TCHAR ws_szLastError[1024];   // gszLastErr

   INT   ws_end; // giEnd - just an END (to check for overrun)

}FC4WS;

typedef FC4WS * LPFC4WS;

// and some MACROS to make life easier for referring to the WORK structure
#define		WS			LPFC4WS pWs
#define		W			(*pWs)

extern   LPFC4WS     glpWs;
#define     GW       (*glpWs)

#define		g_iVerbal		GW.ws_iVerbal
#define		g_bBinary		W.ws_bBinary
#define		g_bQuick		W.ws_bQuick
#define		g_bDnHeader		W.ws_bDnHeader
#define		g_bDiag1		W.ws_bDiag1
#define		g_dwDbgBits		W.ws_dwDbgBits
#define		g_dwDbgSave		W.ws_dwDbgSave
#define		g_dwSaveDbg		W.ws_dwSaveDbg
#define		g_dwMinDbg		W.ws_dwMinDbg

#define		g_bReverse		W.ws_bReverse
#define		g_iPrevCnt		W.ws_iPrevCnt
#define		g_bAddNums		W.ws_bAddNums
#define		g_dwNotExact	W.ws_dwNotExact

#define		g_bSkipSpaces	( g_dwNotExact & x_SkipSpaces )
#define		g_bIgnoreCase	( g_dwNotExact & x_IgnoreCase )

#define		g_dwMatchCnt	W.ws_dwMatchCnt

#define		g_dwMaxLines	W.ws_dwMaxLines
#define		g_dwDbgLine 	W.ws_dwDbgLine

#define		g_dwF1Lines		W.ws_dwF1Lines
#define		g_dwF2Lines		W.ws_dwF2Lines
// Begin / End initial
#define		g_dwB1			W.ws_dwB1
#define		g_dwE1			W.ws_dwE1
// Do the centre
#define		g_dwC			W.ws_dwC
// Begin / End 2nd set
#define		g_dwB2			W.ws_dwB2
#define		g_dwE2			W.ws_dwE2

#define		gbFnd1			W.ws_bFnd1
#define		gbFnd2			W.ws_bFnd2
#define		gbFndIn1		W.ws_bFndIn1
#define		gbFndIn2		W.ws_bFndIn2

#define		gdwLns1			W.ws_dwLns1
#define		gdwLns2			W.ws_dwLns2

//	ALINE	ws_sAct1, ws_sAct2;
//	ALINE	ws_sFnd1, ws_sFnd2;
//	ALINE	ws_sNxt1, ws_sNxt2;
#define		gsAct1			W.ws_sAct1
#define		gsAct2			W.ws_sAct2
#define		gsFnd1			W.ws_sFnd1
#define		gsFnd2			W.ws_sFnd2
#define		gsNxt1			W.ws_sNxt1
#define		gsNxt2			W.ws_sNxt2

#define		VERBAL			( g_iVerbal > 0 )
#define		VERBAL2			( g_iVerbal > 1 )
#define		VERBAL3			( g_iVerbal > 2 )
#define		VERBAL4			( g_iVerbal > 3 )
#define		VERBAL5			( g_iVerbal > 4 )
#define		VERBAL6			( g_iVerbal > 5 )
#define		VERBAL7			( g_iVerbal > 6 )
#define		VERBAL8			( g_iVerbal > 7 )
#define		VERBAL9			( g_iVerbal > 8 )

#define  gszDiag  GW.ws_szDiag
#define  giEnd    GW.ws_end

// Fc4.c
// local/global
extern	int		ProcessCommand( WS, int argc, char * argv[] );
extern	BOOL	GetMapFile( LPMPSTR lpmps );
extern	BOOL	KillMapFile( LPMPSTR lpmps );
extern	LPTSTR	GetsszDate( void );
extern	LPTSTR	GetsszTime( void );

extern	BOOL	wf( HANDLE hf, LPTSTR lpb, DWORD len );
extern	void	PgmExit( BOOL bExit );
extern	void	wof( LPTSTR lpb, DWORD len );
extern	void	oi( LPTSTR lps );
extern	void	prt( LPTSTR lps );
extern	void	Init( WS );

// ABANDONED = extern	TCHAR	szDiag[];	// 1024

// Fc4Comp.c
extern	int		CompFiles( WS, LPMPSTR lpmps1, LPMPSTR lpmps2 );

// some macros
#define  giLnBuf     GW.ws_iLnBuf    // (for MXLINES)
#define  gszLnBuf    GW.ws_szLnBuf   // [MXLINEB2]

#define  gszTemp  GW.ws_szTemp   // [MXDIAGBUF]; a BIG buffer - 4096
#define  gszDLn1  GW.ws_szDLn1   // [1024]; - an output LINE buffer - 1024
#define  gszDLn2  GW.ws_szDLn2   // [1024]; - an output LINE buffer - 1024

//MPSTR	msFile1;
//MPSTR	msFile2;
#define		gmsFile1		W.ws_msFile1
#define		gmsFile2		W.ws_msFile2

#define		gszFile1		W.ws_msFile1.mp_Nm
#define		gszFile2		W.ws_msFile2.mp_Nm
#define		ghFile1			W.ws_msFile1.mp_Hf
#define		ghFile2			W.ws_msFile2.mp_Hf
#define		gi64File1		W.ws_msFile1.mp_Sz
#define		gi64File2		W.ws_msFile2.mp_Sz

#define  gszPath1 GW.ws_szPath1  // [MXPATH] - folder 1
#define  gszPath2 GW.ws_szPath2  // [MXPATH] - folder 2
#define  giPath1 GW.ws_iPath1  // INT - folder 1
#define  giPath2 GW.ws_iPath2  // INT - folder 2
#define  gsList1  GW.ws_sList1
#define  gsList2  GW.ws_sList2  // gsList1, gsList2
#define  ghOutFil GW.ws_hOutFil  // ghOutFil

#define gdwLastError GW.ws_dwLastError   /* last error, if any */
#define gszLastErr   GW.ws_szLastError   // gszLastErr

#include "Fc4Help.h"

#include "Fc4Utils.h"   // should be LAST include

// in Fc4List.c
extern PLE Add_2_List( PLE ph, PTSTR pf );
extern void Expand_Folder( PLE ph, PTSTR pf );

#endif	// _Fc4_h
// eof - Fc4.h

