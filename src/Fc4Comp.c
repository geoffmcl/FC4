

// Fc4Comp.c

#include	"Fc4.h"

#define  SHOWLINE(a)\
   sprintf(lpd, "%5d: [%s]%d"MEOR, a->Next.c_dwLns, GetCLine(a), a->Next.c_dwCLen);\
   prt(lpd);

#define  SHOWLINE1(a)\
   sprintf(gszTemp, "1 %5d: [%s]%d"MEOR, a->Next.c_dwLns, GetCLine(a), a->Next.c_dwCLen);\
   if( strcmp(gszTemp, gszDLn1) ) {\
      strcpy(gszDLn1,gszTemp);\
      prt(gszDLn1);\
   }

#define  SHOWLINE2(a)\
   sprintf(gszTemp, "2 %5d: [%s]%d"MEOR, a->Next.c_dwLns, GetCLine(a), a->Next.c_dwCLen);\
   if( strcmp(gszTemp, gszDLn2) ) {\
      strcpy(gszDLn2,gszTemp);\
      prt(gszDLn2);\
   }

// local
LPTSTR	GetPN( WS, LPAFILE paf );
int		GetPNi( WS, LPAFILE paf );
#ifndef  NDEBUG
void	OutSummary( WS );
#endif   // ifndef  NDEBUG

void	prtnl( char * lpd );

PTSTR GetCLine( LPAFILE paf );
LPTSTR	GetsszFP( WS, LPAFILE paf, int if12 );
LPTSTR	GetsszFP2( WS, LPAFILE paf, int if12 );
LPTSTR	GetsszLine( LPAFILE paf, DWORD max );
LPTSTR	GetsszNX( WS );

static TCHAR gsszFPp[MXSAF][256];
static AFILE gaf[MXSAF] = { 0 };
static TCHAR sszFP[256];
TCHAR	gszType[264];
TCHAR	gszShow[1024];
char *	glpShw = &gszShow[0];


#define	MXSHWLS	8
#define	MXSHWLC	264
char	gszShwLst[ ( ( MXSHWLS + 1 ) * MXSHWLC ) ];
char *	glpNxtShw = &gszShwLst[0];
int		giNxtShw = 0;
int		giLstShw = 0;
int		giPrvShw = 0;
int		giActOne = 0;

/* States of the TWO files */
/* AFILE	af1, af2;      */
DWORD	gdwDownCnt[MXSAF] = { 0 };
DWORD	gdwUpCnt[  MXSAF] = { 0 };

TCHAR	gszMCrLf[] = MCRLF;
TCHAR	gszLead[] = MCRLF"----------";
TCHAR	gszTail[] = MCRLF
					"-----------------------------------------------"
					MCRLF;
UL	gulSaveDbg;
int	gfDnSave      = 0;
int	gfAddData     = 0;
int	giShowNextLns = 1;

int		giSameCnt = 0;
int		giDoShow  = 0;
int		gbDiff;

char	gszDiff[] = "DIFF!";
char	gszJCrLf[] = "Just CrLf!";
/* only pass as equal in this case */
char	gszsame[] = "same";
char	gszjcrlf[] = "just crlf!";

#ifndef  NDEBUG
/* when ( g_dwDbgLine == paf1->From.c_dwLns ) = From F1 LINE NUMBER */
/* CURRENT F? LINE NUMBER */
void	gotdbg( WS, LPAFILE paf, DWORD dwDLine )
{
	LPTSTR	lpd = gszDiag;
	int		index = GetPNi(pWs,paf);
	UL		dwi = paf->From.c_dwLns;

	*lpd = 0;
	if( gcLastOut != '\n' )
		strcpy(lpd,MCRLF);
	sprintf( EndBuf(lpd),
		"Reached: %s Line = %d.",
		GetPN(pWs,paf),
		dwDLine );

	if( g_dwDbgBits )
	{
		strcat( lpd, "[Dbg" );
		if( Dbg1 )	/*( g_dwDbgBits & Dbg_Bit1 ) */
			strcat(lpd,"1");
		if( Dbg2 )
			strcat(lpd,"2");
		if( Dbg3 )
			strcat(lpd,"3");
		if( Dbg4 )
			strcat(lpd,"4");
		if( Dbg5 )
			strcat(lpd,"5");
		if( Dbg6 )
			strcat(lpd,"6");
		strcat(lpd,"]");
	}

	if( !gfDnSave )
	{
		gfDnSave = TRUE;
		gulSaveDbg = g_dwDbgBits;
	}

	/* and bounce it to MAX */
	g_dwDbgBits |= Dbg_Max06;	/* (DWORD)-1; */
	/* ==================== */

	prt(lpd);

	OutSummary( pWs );


}

#endif   // ifndef  NDEBUG


void	chkchk( void )
{
	int	i;
	i = 0;
}
void	chkdiff( void )
{
	int	i;
	i = 0;
}
void	chkdiff2( void )
{
	int	i;
	i = 0;
}
void	chkend( void )
{
	int	i;
	i = 0;
}

BOOL SkipSpace( WS,		// work structure
			   LPTSTR lpb1, DWORD dwl1, LPTSTR lpb2, DWORD dwl2 )
{
	BOOL	diff = TRUE;
	LPTSTR	pb1, pb2;
	DWORD	dw1, dw2;
	if( ( pb1 = lpb1 ) &&
		( pb2 = lpb2 ) )
	{
		dw1 = dwl1;
		dw2 = dwl2;
		while( TRUE )
		{
			// move to sig char in F1
			while( ( *pb1 <= ' ' ) &&
				( dw1 ) )
			{
				pb1++;
				dw1--;
			}
			// move to sig char in F2
			while( ( *pb2 <= ' ' ) &&
				( dw2 ) )
			{
				pb2++;
				dw2--;
			}
			if( ( dw1 == 0 ) &&
				( dw2 == 0 ) )
			{
				// BOTH ran out of chars
				return FALSE;	// all done = EQUAL LINES
			}
			else if( dw1 == 0 )
			{
				return TRUE;
			}
			else if( dw2 == 0 )
			{
				return TRUE;
			}

			while( ( dw1 ) &&
				( dw2 ) )
			{
				if( g_bIgnoreCase )
				{
					if( toupper( *pb1 ) != toupper( *pb2 ) )
					{
						return TRUE;
					}
				}
				else
				{
					if( *pb1 != *pb2 )
					{
						return TRUE;
					}
				}
				pb1++;
				pb2++;
				dw1--;
				dw2--;
				if( ( dw1 == 0 ) &&
					( dw2 == 0 ) )
				{
					return FALSE;
				}
				// if one or the other falls into spacey stuff
				if( ( *pb1 <= ' ' ) ||
					( *pb2 <= ' ' ) )
				{
					// back to TOP to move up to next sig chars
					break;
				}
				//if( ( *pb1 <= ' ' ) &&
				//	( *pb2 <= ' ' ) )
				//{
				//	// back to TOP to move up to next sig chars
				//	break;
				//}
				//else if( *pb1 <= ' ' )
				//{
				//	// 2nd still has sigs
				//	return TRUE;
				//}
				//else if( *pb2 <= ' ' )
				//{
				//	// 1st still has sigs
				//	return TRUE;
				//}
				// else both are still significant
				// cycle comparing
			}
		}
	}
	return diff;
}

int	SetCompType( WS, LPAFILE paf1, LPAFILE paf2, char * lpDest,
				int flg )
{
	char	buf[264];
	int		i = 0;
	char *	lpd;

	lpd = &buf[0];
	*lpd = 0;

	if( gbDiff )
	{
		if(( paf1->Next.c_dwCLen == 0 ) &&
			( paf2->Next.c_dwCLen == 0 ) )
		{
			/* both blanks */
			strcat( lpd, gszJCrLf );
			/* leave as equal */
		}
		else
		{
			if( paf1->Next.c_dwCLen == 0 )	// 1 blank
			{
				strcat( lpd, "1 CrLf" );
				i = 1;
			}
			else if( paf2->Next.c_dwCLen == 0 )
			{
				strcat( lpd, "2 CrLf" );
				i = 2;
			}
			else
			{
				strcat( lpd, gszDiff );
				i = 3;
			}
		}
	}
	else
	{
		if(( paf1->Next.c_dwCLen == 0 ) &&
			( paf2->Next.c_dwCLen == 0 ) )
		{
			// both blanks
			strcat( lpd, gszjcrlf );
		}
		else
		{
			if( paf1->Next.c_dwCLen == 0 )	// 1 blank
			{
				strcat( lpd, "1 CrLf" );
			}
			else if( paf2->Next.c_dwCLen == 0 )
			{
				strcat( lpd, "2 CrLf" );
			}
			else
			{
				strcat( lpd, gszsame );
			}
		}
	}

	strcat( lpDest,  lpd );
	if( flg )
	{
		if( ( strcmp( lpd, gszsame ) == 0 ) ||
			( strcmp( lpd, gszjcrlf ) == 0 ) )
		{
			if( ( strcmp( gszType, gszsame ) == 0 ) ||
				( strcmp( gszType, gszjcrlf ) == 0 ) )
			{
				/* already have SAME type, just store */
				giSameCnt++;
			}
			else
			{
				giDoShow++;
				giSameCnt++;
			}
		}
		else
		{
			giDoShow++;
			/* remember to SET giSameCnt to 0 later, after display */
		}
		strcpy( gszType, lpd );
	}
	return i;

}
/* ==============================
	void	prtlast( void )

	PURPOSE: Spit out the LAST saved line

    =================================================== */

void	prtlast( void )
{
	LPTSTR lpd = gszDiag;
	sprintf( lpd, "=== %d same until === %s.",
		giSameCnt,
		&gszShwLst[ (giPrvShw * MXSHWLC) ] );
	prtnl(lpd);
}

/*	=====================================================
	void	prtshw( void )

	PURPOSE: potentially display, and
			 save a diag line of output

		It always keeps the last MXSHWLS (aka max show line stg)
		was 8 ...

		At present only previous is used.

	====================================================  */
void	prtshw( void )
{
	int		i;


	strcpy( glpNxtShw, glpShw );
	if( giDoShow )
	{
		giDoShow--;
		if( giSameCnt > 1 )
		{
			prtlast();
			giSameCnt = 0;
		}
		prt(glpShw);
	}

	/* ================ */
	i = giNxtShw;
	giPrvShw = i;
	i++;
	if( i >= MXSHWLS )
		i = 0;
	giNxtShw = i;
	/* ================ */

	if( giLstShw == i )
	{
		giLstShw++;
		if( giLstShw >= MXSHWLS )
			giLstShw = 0;
	}

	glpNxtShw = &gszShwLst[ (i * MXSHWLC) ];

	*glpShw = 0;

}

// ==========================================================
// BOOL	CompALine( WS, LPAFILE paf1, LPAFILE paf2 )
//
// Compare current line F1 with currect line F2
//
// 	return bDiff;	TRUE if difference,
//					FALSE if SAME
//
// ==========================================================
BOOL	CompALine( WS, LPAFILE paf1, LPAFILE paf2 )
{
//	BOOL	bDiff = FALSE;
	DWORD	dwlen = 0;
	LPTSTR	lpb1, lpb2;
	LPTSTR	lpd;
	DWORD	k = 0;

	gbDiff = FALSE;
	lpd = gszDiag;
	if(( paf1->lpBuf ) &&
		( paf2->lpBuf ) )
	{
		// GET POINTERS TO BEGINNING OF LINES
		lpb1 = &paf1->lpBuf[paf1->Next.c_dwBgn];
		lpb2 = &paf2->lpBuf[paf2->Next.c_dwBgn];
		// ==================================
      if( VERBAL9 ) {   // show compare of lines
         SHOWLINE1(paf1);
         SHOWLINE2(paf2);
      }

		if( paf1->Next.c_dwCLen == paf2->Next.c_dwCLen )
		{
			// LENGTHS ARE THE SAME
			if( paf1->Next.c_dwCLen )		// and there is LENGTH
			{
				// AND NOT ZERO
				if( g_bIgnoreCase )
				{
					for( dwlen = 0; dwlen < paf1->Next.c_dwCLen; dwlen++ )
					{
						if( toupper(lpb1[dwlen]) != toupper(lpb2[dwlen]) )
						{
							gbDiff = TRUE;
							break;
						}
					}
				}
				else if( g_bSkipSpaces )   // = ( g_dwNotExact & x_SkipSpaces )
 
				{
					gbDiff = SkipSpace( pWs, lpb1, paf1->Next.c_dwCLen, lpb2, paf2->Next.c_dwCLen );
				}
				else	// absolute BINARY compare
				{
					for( dwlen = 0; dwlen < paf1->Next.c_dwCLen; dwlen++ )
					{
						if( lpb1[dwlen] != lpb2[dwlen] )
						{
							gbDiff = TRUE;
							break;
						}
					}
				}
			}	// AND WE HAVE A LENGTH
			else
			{
				// WE HAVE NO SIGNIFICANT LENGTH
				// LEAVE DEFAULT FALSE == LINES ARE THE SAME
				chkend();
			}
		}
		else
		{
			// depend whether significant
			if( g_bSkipSpaces )   // = ( g_dwNotExact & x_SkipSpaces )
			{
				gbDiff = SkipSpace( pWs, lpb1, paf1->Next.c_dwCLen, lpb2, paf2->Next.c_dwCLen );
			}
			else	// absolute binary compare
			{
				// Different LENGTH absolutely means the
				// *** LINES ARE DIFFERENT ***
				// =====================================
				gbDiff = TRUE;
				// =====================================
			}
		}
	}

#ifdef	DBGDIAG1
	if(( Dbg11 ) ||
		( Dbg3  ) )
	{
		if( Dbg11 )
		{
			if( gfAddData )
			{
				strcpy( lpd, MCRLF"Comparing:"MCRLF"F1[" );
				k = strlen(lpd);
				for( dwlen = 0; dwlen < paf1->Next.c_dwCLen; dwlen++ )
				{
					if( lpb1[dwlen] < ' ' )
						break;

					lpd[k++] = lpb1[dwlen];

					if( dwlen > 65 )
						break;
				}
				if( dwlen == 0 )
					lpd[k++] = '~';
				lpd[k] = 0;
				/* add the F! line number */
				sprintf( EndBuf(lpd), "] ln %u with"MCRLF"F2[",
					paf1->Next.c_dwLns );
				k = strlen(lpd);
				for( dwlen = 0; dwlen < paf2->Next.c_dwCLen; dwlen++ )
				{
					if( lpb2[dwlen] < ' ' )
						break;

					lpd[k++] = lpb2[dwlen];
					if( dwlen > 65 )
						break;
				}
				if( dwlen == 0 )
					lpd[k++] = '~';
				lpd[k] = 0;
				sprintf( EndBuf(lpd), "] ln %u Result:",
					paf2->Next.c_dwLns );
			}
			else
			{
				strcpy( lpd, MCRLF"Comparing: F1 " );
				/* add the F! line number */
				sprintf( EndBuf(lpd), "ln %u with F2 ",
					paf1->Next.c_dwLns );
				sprintf( EndBuf(lpd), "ln %u Result:",
					paf2->Next.c_dwLns );
			}
			SetCompType( pWs, paf1, paf2, lpd, 0 );
			prt(lpd);
		}

		if( Dbg3 )
		{
			SetCompType( pWs, paf1, paf2, glpShw, 1 );
			prtshw();
//			prt(glpShw);
//			*glpShw = 0;
		}
	}
#endif	// DBGDIAG1

	return gbDiff;	/* TRUE if difference, */
	/* FALSE if SAME */

}

LPTSTR	GetPN( WS, LPAFILE paf )
{
	LPTSTR	lps;
	LPAFILE	paf1, paf2;
	static TCHAR sszFN[16];

	paf1 = &pWs->ws_AF1;
	paf2 = &pWs->ws_AF2;

	lps = &sszFN[0];
	if( paf1 == paf )
		strcpy( lps, "F1" );
	else if( paf2 == paf )
		strcpy( lps, "F2" );
	else
		strcpy( lps, "F???" );
	return lps;
}

int	GetPNi( WS, LPAFILE paf )
{
	int		i = 0;
	LPAFILE	paf1, paf2;
	paf1 = &pWs->ws_AF1;
	paf2 = &pWs->ws_AF2;
	if( paf1 == paf )
		i = 1;
	else if( paf2 == paf )
		i = 2;
	else
		i = 0;
	return i;
}

BOOL	DownALine( WS, LPAFILE paf )
{
	BOOL	flg;
	BYTE	b1;
	DWORD	dwl;
	LPTSTR	lpd = gszDiag;
	int		index;
//	char	c;
	UL		dwi;

	index = GetPNi(pWs,paf);	/* get WHICH FILE we are using */
	/* keep - set the BEGINNING of the current line */
	paf->Next.c_dwBgn = dwi = paf->Next.c_dwI;	/* BEGIN at Next.c_dwI */

	/* ============================================ */
	gdwDownCnt[index]++;

#ifdef	DBGDIAG3
	if( Dbg13 )
	{
//		sprintf( lpd,
//			MCRLF"Down in %s %s (%d)",
//			GetPN(pWs,paf),
//			GetsszFP(pWs,paf,index),
//			gdwDownCnt[index] );
		*lpd = 0;
		sprintf( EndBuf(lpd),
			MCRLF"Down in %s %s [",
			GetPN(pWs,paf),
			GetsszFP(pWs,paf,index) );
		/* prt(lpd); */
		if( gdwDownCnt[index] == 11 )
		{
			LPTSTR	lpb2;
//			chkchk();
			lpb2 = &paf->lpBuf[paf->Next.c_dwI];
		}
	}
	if( Dbg3 )
	{
		if( ( index == 1 ) ||
			( *glpShw == 0 ) )
		{
			strcpy( glpShw, MCRLF"Down in" );
		}
		sprintf( EndBuf(glpShw),
			" %s ",
			GetPN(pWs,paf) );
	}
#endif	// DBGDIAG3

	/* check and set GO DOWN flag - is there more left in the file? */
	flg = ( (dwi < paf->dwSz) ? TRUE : FALSE );
	dwl = 0; // length of line, sans CR/LF, if any
	for( ; dwi < paf->dwSz; dwi++ )
	{
		b1 = paf->lpBuf[dwi]; /* get BYTE from file */
		/* is it cr or lf */
		if(( b1 == '\r' ) ||
			( b1 == '\n' ) )
		{
			/* bump past the stopper byte */
			dwi++;	/* included in LINE */
			if(( b1 == '\r'      ) &&
				( dwi < paf->dwSz ) )
			{
				/* include CR and LF pair - at end of line */
				if( paf->lpBuf[dwi] == '\n' )
					dwi++;	/* std DOS pair - CR/LF */
			}
			break;
		}
		dwl++;	/* bump to NEXT character count */
	}

	paf->Next.c_dwEnd = paf->Next.c_dwI = dwi;	/* END OF LINE plus 1/2 = start of NEXT line */
	paf->Next.c_dwCLen = dwl;	/* return actual character length, less [Cr/Lf] */

	paf->Next.c_dwLen = dwi -		// paf->Next.c_dwEnd
			paf->Next.c_dwBgn;

	if( flg )
	{
		/* there was length to go down on */
		paf->Next.c_dwLns++;	/* so get the new line number */
	}

#ifdef	DBGDIAG3
	if( Dbg13 )
	{
		if( dwl )
		{
			DWORD	k;
			LPSTR	lpb;
			char	d = 0;
      	char	c;

			// prt( MCRLF );
			if( dwl > MXOFLINE )
				dwl = MXOFLINE;
			/* get to file data */
			lpb = &paf->lpBuf[paf->Next.c_dwBgn];
			k = strlen(lpd);
			for( dwi = 0; dwi < dwl; dwi++ )
			{
				c = lpb[dwi];
				if( c <= ' ' )
				{
					if( d == ' ' )
					{
						// ignore
						c = 0;
					}
					else
					{
						c = ' ';
					}
				}
				else
				{
					d = c;
				}
				if( c )
				{
					lpd[k++] = c;
					d = c;
				}
			}
			lpd[k] = 0;
		}
		else
		{
			strcat( lpd, "<blank>" );
		}

		sprintf( EndBuf(lpd),
			"] End at %s %s",
			GetsszFP(pWs,paf,index),
			(flg ? "T" : "F" ) );

		prt(lpd);

	}
	if( Dbg3 )
	{
		sprintf( EndBuf(glpShw),
			"to %s(%s) ",
			GetsszFP(pWs,paf,index),
			(flg ? "T" : "F" ) );
	}
#endif	// DBGDIAG3

	return flg;		// we went down a line

}

BOOL	UpALine( WS, LPAFILE paf )
{
	BOOL	flg;
	BYTE	b1;
	DWORD	dwl, dwi;
	LPTSTR	lpd = gszDiag;
	int		index;

	dwl = 0;
	index = GetPNi(pWs,paf);
//	paf->Next.c_dwBgn = paf->Next.c_dwI;
	gdwUpCnt[index]++;
#ifdef	DBGDIAG3
	if( Dbg13 )
	{
		sprintf( lpd,
			MCRLF"BUp in %x %s (%d)",
			paf,
			GetsszFP(pWs,paf,index),
			gdwUpCnt[index] );
		prt(lpd);
	}
#endif	// DBGDIAG3

	flg = ( (paf->Next.c_dwI > 0) ? TRUE : FALSE );

	if( flg )
	{
		dwi = paf->Next.c_dwI - 1;
	}
	else
	{
		dwi = 0;
	}
	for( ; dwi >= 0; dwi-- )
	{
		b1 = paf->lpBuf[ dwi ];
		if( ( b1 == '\r' ) ||
			( b1 == '\n' ) )
		{
			if( dwl == 0 )
			{
				dwi--;	// included in LINE
				if( ( b1 == '\n' ) &&
					( dwi > 0 ) )
				{
					if( paf->lpBuf[ dwi ] == '\n' )
						dwi--;	// std pair
				}
			}
			else
			{
				dwi++;
				break;
			}
		}
		dwl++;
	}

//	paf->Next.c_dwEnd = paf->Next.c_dwI;
//	paf->Next.c_dwCLen = dwl;	// return character length
//	paf->Next.c_dwLen = paf->Next.c_dwI -		// paf->Next.c_dwEnd
//			paf->Next.c_dwBgn;

//	if( flg )
//		paf->Next.c_dwLns++;

#ifdef	DBGDIAG3
	if( Dbg13 )
	{
		sprintf( lpd,
			MCRLF"EUp in %x %s %s",
			paf,
			GetsszFP(pWs,paf,GetPNi(pWs,paf)),
			(flg ? "T" : "F" ) );
		prt(lpd);
	}
#endif	// DBGDIAG3
	return flg;		// we went up a line
}

// ==========================================================
// BOOL	CompLines( WS,	// work structure
//				  LPAFILE paf1, LPAFILE paf2,
//				  DWORD mc )
//
// While going ever downwards in both files, compare the
//	corresponding lines
//
// ==========================================================
BOOL	CompLines( WS,	// work structure
				  LPAFILE paf1, LPAFILE paf2,
				  DWORD mc )
{
	BOOL	bRet = FALSE;	// assume SAME for mc lines down
	DWORD	dwc;
	ALINE	temp1, temp2;


	if( ( paf1 ) &&
		( paf2 ) &&
		( dwc = mc ) )
	{
		// save current line - ie Next
		// =================
		temp1 = paf1->Next;
		temp2 = paf2->Next;
		// =================
		while( dwc )
		{
			if( !DownALine( pWs, paf1 ) )
			{
				// if no more F1 down, then
				bRet = TRUE;
				break;
			}

			if( !DownALine( pWs, paf2 ) )
			{
				// if no more F2 down, then
				bRet = TRUE;
				break;
			}

			if( CompALine( pWs, paf1, paf2 ) )
			{
				// Next down is DIFFERENT
				bRet = TRUE;
				break;
			}
			dwc--;
		}

		// restore current lines - ie Next
		// =====================
		paf1->Next = temp1;
		paf2->Next = temp2;
		// =====================
	}

	return bRet;
}

// =========================================================
// BOOL	FindLineIn1( WS, LPAFILE paf1, LPAFILE paf2 )
//
// Try FINDING the current F2 line somewhere in F1
//
// =========================================================
BOOL	FindLineIn1( WS, LPAFILE paf1, LPAFILE paf2 )
{
	BOOL	flg = FALSE;
   if( VERBAL8 ) {
      sprintf(gszTemp, "FindLineIn1: The current F2(%d) line in F1(%d) ..."MEOR,
         paf2->Next.c_dwLns, paf1->Next.c_dwLns);
      prt(gszTemp);
   }
	while( DownALine( pWs, paf1 ) )
	{
		if( !CompALine( pWs, paf1, paf2 ) )
		{
			// found a single line match
			if( !gbFnd1 )
			{
				// Keep FIRST find of an F2 line in F1
				gsNxt2 = paf2->Next;
				gsFnd1 = paf1->Next;
				gbFnd1 = TRUE;
			}
			if( g_dwMatchCnt )
			{
				// but User want -Mnnn lines to match
				// before a FIND is OK.
				if( !CompLines( pWs, paf1, paf2, g_dwMatchCnt ) )
				{
					flg = TRUE;
					break;
				}
			}
			else
			{

				flg = TRUE;
				break;
			}
		}
	}
   if( VERBAL8 ) {
      sprintf( gszTemp, "END FindLineIn1: The current F2(%d) line in F1(%d) ... %s ..."MEOR,
         paf2->Next.c_dwLns, paf1->Next.c_dwLns,
         ( flg ? "FOUND" : "NOT FOUND" ) );
      prt(gszTemp);
   }
	return flg;
}

// =========================================================
// BOOL	FindLineIn2( WS, LPAFILE paf1, LPAFILE paf2 )
//
// Try FINDING the current F1 line somewhere DOWN in F2
//
// RETURN:	TRUE or FALSE
// but also set things like -
// 	Keep FIRST find of an F1 line in F2
//	gsNxt1 = paf1->Next;
//	gsFnd2 = paf2->Next;
//	gbFnd2 = TRUE;
// =========================================================
BOOL	FindLineIn2( WS, LPAFILE paf1, LPAFILE paf2 )
{
	BOOL	flg = FALSE;
	/* note, no save of current done here */
	/* BUT the following SAVES have been done */
	/* gsAct2 = paf2->Next; */
	/* paf2->Save1 = paf2->Next; */
	/* paf2->Save2 = paf2->Next; */
   if( VERBAL8 ) {
      sprintf(gszTemp, "FindLineIn2: current line of F1(%d) in F2(%d) ..."MEOR,
         paf1->Next.c_dwLns, paf2->Next.c_dwLns);
      prt(gszTemp);
   }
	while( DownALine( pWs, paf2 ) )
	{
		if( !CompALine( pWs, paf1, paf2 ) )
		{
			// RETURNED false - Lines are the SAME
			// ===================================
			if( !gbFnd2 )
			{
				// Keep FIRST find of an F1 line in F2
				gsNxt1 = paf1->Next;
				gsFnd2 = paf2->Next;
				gbFnd2 = TRUE;
			}
			// found at least a 1 line match
			if( g_dwMatchCnt )
			{
				// but User want -Mnnn lines to match
				// before a FIND is OK.
				if( !CompLines( pWs, paf1, paf2, g_dwMatchCnt ) )
				{
					flg = TRUE;
					break;
				}
			}
			else
			{
				flg = TRUE;
				break;
			}
		}
	}
   if( VERBAL8 ) {
      sprintf(gszTemp, "END FindLineIn2: current line of F1(%d) in F2(%d) ...%s "MEOR,
         paf1->Next.c_dwLns, paf2->Next.c_dwLns, 
         ( flg ? "FOUND" : "NOT FOUND" ) );
      prt(gszTemp);
   }

	return flg;
}

UL	GetPercent( LPAFILE paf )
{
	UL	dwi = 0;
	if( paf->End.c_dwLns )
	{
		dwi = ( paf->From.c_dwLns * 100 ) / paf->End.c_dwLns;
	}
	else
	{
		dwi = 100;
	}
	return dwi;
}

UL	GetBytes( LPAFILE paf )
{
	UL	dwi = 0;
	dwi = (paf->To.c_dwBgn - paf->From.c_dwBgn);
	return dwi;
}

void	ShowSummary( WS, LPAFILE paf1, LPAFILE paf2 )
{
	LPTSTR	lpd = gszDiag;
	UL		dwp1, dwp2;
	sprintf( lpd,
		MCRLF"Comparing: F1[%s] Ln %u with"
		MCRLF"           F2 from line %u to %u (%u comps)!"
		MCRLF,
		GetsszLine(paf1, 30),
		paf1->Next.c_dwLns,
		paf2->Next.c_dwLns,
		paf2->End.c_dwLns,
		(paf2->End.c_dwLns - paf2->Next.c_dwLns) );

	*lpd = 0;
	if( gcLastOut != '\n' )
		strcpy( lpd, "\r\n" );

	g_dwF1Lines = (paf1->To.c_dwLns - paf1->From.c_dwLns);
	g_dwF2Lines	= (paf2->To.c_dwLns - paf2->From.c_dwLns);

	dwp1 = GetBytes(paf1);
	dwp2 = GetBytes(paf2);

	sprintf( EndBuf(lpd),
		"ShowDiff: F1Ln: %d for %d/%d F2Ln: %d for %d/%d line/bytes.\r\n",
		paf1->From.c_dwLns,	// From F1 LINE NUMBER
		g_dwF1Lines,		// for LINES
		dwp1,				// and PERCENTAGE DONE
		paf2->From.c_dwLns,	// From F2 LINE NUMBER
		g_dwF2Lines,		// for LINES
		dwp2 );				// and PERCENTAGE

	prt(lpd);

}

void	prtnl( char * lpd )
{
	if( gcLastOut != '\n' )
		prt(MCRLF);
	prt(lpd);
}

void	ShowPercentage( WS, LPAFILE paf1, LPAFILE paf2 )
{
	LPTSTR	lpd = gszDiag;
	UL		dwp1, dwp2;

	// GetsszLine(paf1, 30),
	sprintf( lpd,
		     "OutPutting:F1 from Ln %u to %u (%u lns)"
		MCRLF"           F2 from ln %u to %u (%u lns)"
		MCRLF,
		paf1->Next.c_dwLns,
		paf1->End.c_dwLns,
		(paf1->End.c_dwLns - paf1->Next.c_dwLns),
		paf2->Next.c_dwLns,
		paf2->End.c_dwLns,
		(paf2->End.c_dwLns - paf2->Next.c_dwLns) );
	prtnl(lpd);

	*lpd = 0;
	if( gcLastOut != '\n' )
		strcpy( lpd, "\r\n" );

	g_dwF1Lines = (paf1->To.c_dwLns - paf1->From.c_dwLns);
	g_dwF2Lines	= (paf2->To.c_dwLns - paf2->From.c_dwLns);

	dwp1 = GetPercent(paf1);
	dwp2 = GetPercent(paf2);

	sprintf( EndBuf(lpd),
		"ShowDiff: F1Ln: %d for %d %d F2Ln: %d for %d %d line percent.\r\n",
		paf1->From.c_dwLns,	// From F1 LINE NUMBER
		g_dwF1Lines,		// for LINES
		dwp1,				// and PERCENTAGE DONE
		paf2->From.c_dwLns,	// From F2 LINE NUMBER
		g_dwF2Lines,		// for LINES
		dwp2 );				// and PERCENTAGE

	prt(lpd);

}

#ifndef  NDEBUG

void	OutSummary( WS )
{
	LPTSTR	lpd = gszDiag;
	LPAFILE	paf1, paf2;

	paf1 = &pWs->ws_AF1;
	paf2 = &pWs->ws_AF2;
	ShowPercentage( pWs, paf1, paf2 );

	sprintf( lpd,
		"F1 %s"MCRLF,
		GetsszFP2( pWs, paf1, 1 ) );
	prtnl(lpd);
	sprintf( lpd,
		"F2 %s"MCRLF,
		GetsszFP2( pWs, paf2, 2 ) );
	prtnl(lpd);

}
#endif   // ifndef  NDEBUG

int	SanityCheck2( WS, LPAFILE paf1, LPAFILE paf2 )
{
	LPMPSTR lpmps1 = paf1->lpMP;
	LPMPSTR lpmps2 = paf2->lpMP;
	int	i, j, k;
   DWORD jj, kk;
	LPTSTR	lpd = gszDiag;

   i = 0;
   j = (paf1->To.c_dwBgn - paf1->From.c_dwBgn);
   k = (paf2->To.c_dwBgn - paf2->From.c_dwBgn);
   jj = ( j + paf1->From.c_dwBgn );
   kk = ( k + paf2->From.c_dwBgn );
#ifdef	DBGDIAG4
	// Get length of F1
	if( ( paf1->To.c_dwBgn <= lpmps1->mp_Sz ) &&
		( paf1->From.c_dwBgn < lpmps1->mp_Sz ) &&
		( paf1->From.c_dwBgn < paf1->To.c_dwBgn ) &&
		( j                                           ) &&
		( jj <= lpmps1->mp_Sz ) &&
		( paf2->To.c_dwBgn <= lpmps2->mp_Sz ) &&
		( paf2->From.c_dwBgn < lpmps2->mp_Sz ) &&
		( paf2->From.c_dwBgn < paf2->To.c_dwBgn ) &&
		( k ) &&
		( kk <= lpmps2->mp_Sz ) )
	{
		// appears all ok
		if( Dbg4 )
		{
			sprintf( lpd,
            "SanityCheck2: Doing %u of F1 and %u of F2"MCRLF,
				j, k );
			prt(lpd);
		}
	}
	else
	{
		prt( "ERROR: Internal Parameter Failure!"MCRLF
			"Aborting ShowDiff" );
		if( !( paf1->To.c_dwBgn <= lpmps1->mp_Sz ) )
		{
			prt( "( paf1->To.c_dwBgn <= lpmps1->mp_Sz ) FAILED"MCRLF );
			sprintf( lpd,
				"paf1->To.c_dwBgn = %u lpmps1->mp_Sz = %u "MCRLF,
				paf1->To.c_dwBgn,
				lpmps1->mp_Sz );
			prt(lpd);
		}
		else if( !( paf1->From.c_dwBgn < lpmps1->mp_Sz ) )
		{
			prt( "( paf1->From.c_dwBgn < lpmps1->mp_Sz ) FAILED"MCRLF );
		}
		else if( !( paf1->From.c_dwBgn < paf1->To.c_dwBgn ) )
		{
			prt( "( paf1->From.c_dwBgn < paf1->To.c_dwBgn ) FAILED"MCRLF );

		}
		else if( !( j = (paf1->To.c_dwBgn - paf1->From.c_dwBgn) ) )
		{
			prt( "( j = (paf1->To.c_dwBgn - paf1->From.c_dwBgn) ) FAILED"MCRLF );
		}
		else if( !( ( j + paf1->From.c_dwBgn ) <= lpmps1->mp_Sz ) )
		{
			prt( "( ( j + paf1->From.c_dwBgn ) <= lpmps1->mp_Sz ) FAILED"MCRLF );
		}
		else if( !( paf2->To.c_dwBgn <= lpmps2->mp_Sz ) )
		{
			prt( "( paf2->To.c_dwBgn <= lpmps2->mp_Sz ) FAILED"MCRLF );
		}
		else if( !( paf2->From.c_dwBgn < lpmps2->mp_Sz ) )
		{
			prt( "( paf2->From.c_dwBgn < lpmps2->mp_Sz ) FAILED"MCRLF );
		}
		else if( !( paf2->From.c_dwBgn < paf2->To.c_dwBgn ) )
		{
			prt( "( paf2->From.c_dwBgn < paf2->To.c_dwBgn ) FAILED"MCRLF );
		}
		else if( !( k = (paf2->To.c_dwBgn - paf2->From.c_dwBgn) ) )
		{
			prt( "( k = (paf2->To.c_dwBgn - paf2->From.c_dwBgn) ) FAILED" MCRLF );
		}
		else if( !( ( k + paf2->From.c_dwBgn ) <= lpmps2->mp_Sz ) )
		{
			prt( "( ( k + paf2->From.c_dwBgn ) <= lpmps2->mp_Sz ) FAILED"MCRLF );
		}
		else
		{
			prt( "Something else failed!!!"MCRLF );
		}
		chkchk();
		i = 1;
		//PgmExit();
	}
#endif	// DBGDIAG4
   return i;
}

int	SanityCheck( WS, LPAFILE paf1, LPAFILE paf2 )
{
	int	i, j, k;
   DWORD jj, kk;
	LPTSTR	lpd = gszDiag;
//	if( ( paf1->lpMP = lpmps1 ) &&
//		( paf2->lpMP = lpmps2 ) &&
	LPMPSTR lpmps1 = paf1->lpMP;
	LPMPSTR lpmps2 = paf2->lpMP;
//	char *	lpl;
	i = j = k = 0;
	if( lpmps1 && lpmps2 ) {
      // got the buffers
      j = (paf1->To.c_dwBgn - paf1->From.c_dwBgn);
      k = (paf2->To.c_dwBgn - paf2->From.c_dwBgn);
      jj = ( j + paf1->From.c_dwBgn );
      kk = ( k + paf2->From.c_dwBgn );

		if(( paf1->To.c_dwBgn <= lpmps1->mp_Sz     ) &&
			( paf1->From.c_dwBgn < lpmps1->mp_Sz    ) &&
			( paf1->From.c_dwBgn < paf1->To.c_dwBgn ) &&
			( j                                     ) &&
			( jj <= lpmps1->mp_Sz                   ) &&
			( paf2->To.c_dwBgn <= lpmps2->mp_Sz     ) &&
			( paf2->From.c_dwBgn < lpmps2->mp_Sz    ) &&
			( paf2->From.c_dwBgn < paf2->To.c_dwBgn ) &&
			( k                                     ) &&
			( kk <= lpmps2->mp_Sz                   ) )
		{
			// appears all ok
			if( Dbg4 )
			{
				sprintf( lpd,
               "SanityCheck: Doing %u of F1 and %u of F2"MCRLF,
					j, k );
				prt(lpd);
				i = 1;
			}
		}
	}
	return i;
}

void Out_File_Name( WS, LPMPSTR lpmps )
{
	LPTSTR	lpd = gszDiag;
   PTSTR    lpl;
	if( gcLastOut == '\n' )
		lpl = &gszLead[2];	/* Skip beginning CR/LF */
	else
		lpl = &gszLead[0];	/* include MCRLF */
	strcpy( lpd, lpl );
	strcat( lpd, &lpmps->mp_Nm[0] );
	strcat( lpd, gszMCrLf );
	prt(lpd);

}

//----------test1
//Line 2
//
//----------test2
//New Line
//Line 2
//
//-----------------------------------------------
void	ShowDiff( WS, LPAFILE paf1, LPAFILE paf2 )
{
	LPTSTR	lpd = gszDiag;
//	if( ( paf1->lpMP = lpmps1 ) &&
//		( paf2->lpMP = lpmps2 ) &&
	LPMPSTR lpmps1 = paf1->lpMP;
	LPMPSTR lpmps2 = paf2->lpMP;
	LPTSTR	lpb1, lpb2;
	DWORD	i, j, k, l, num;
	TCHAR	tc;
	BOOL	dncr;
	//char *	lpl;

	lpd = gszDiag;
	if( lpmps1 && lpmps2 )
	{
		dncr = FALSE;

		g_dwF1Lines = (paf1->To.c_dwLns - paf1->From.c_dwLns);	// for LINES
		g_dwF2Lines	= (paf2->To.c_dwLns - paf2->From.c_dwLns);	// for LINES

#ifdef	DBGDIAG6
		if( g_dwDbgLine == paf1->From.c_dwLns )	/* From F1 LINE NUMBER */
			gotdbg( pWs, paf1, g_dwDbgLine );

// =========================================================
		if( Dbg6 )
		{
			ShowSummary( pWs, paf1, paf2 );
		}
// ========================================================
#endif	// DBGDIAG6

#ifdef	DBGDIAG4
      SanityCheck2( pWs, paf1, paf2 );
#endif	// DBGDIAG4

		//strcpy( lpd, MCRLF"----------" );
		//strcat( lpd, &lpmps1->mp_Nm[0] );
		//strcat( lpd, MCRLF );
      Out_File_Name( pWs, lpmps1 );

		// Output of FILE 1 section
		// ========================
		j   = paf1->To.c_dwBgn - paf1->From.c_dwBgn;
		lpb1 = &paf1->lpBuf[paf1->From.c_dwBgn];
		k = 0;
		num = paf1->From.c_dwLns;
		if( g_bAddNums )
		{
			sprintf( lpd,
				"%5u ",
				num );
			k = strlen(lpd);
		}
		l = 0;

		g_dwB1 = num;
		g_dwE1 = num + g_dwF1Lines;
		g_dwC = g_dwB2 = g_dwE2 = 0;
		if( ( g_dwMaxLines ) &&
			( g_dwF1Lines > (g_dwMaxLines+1) ) )
		{
			g_dwE1 = num + (g_dwMaxLines / 2);
			g_dwC  = num + (g_dwF1Lines / 2);
			g_dwB2 = num + ((g_dwF1Lines - 1) - (g_dwMaxLines - (g_dwMaxLines / 2)));
			g_dwE2 = num + g_dwF1Lines;

		}
		for( i = 0; i < j ; i++ )
		{
			tc = lpb1[i];
			if( ( tc < ' ' ) &&
				( tc != '\t' ) )
			{
				// is LESS than 20h, and is NOT a TAB
				// ==================================
				if( !dncr )
				{
					lpd[k] = 0;
					strcat( lpd, MCRLF );

					// OUTPUT THE LINE
					// ====================================
					if( ( num >= g_dwB1 ) && ( num <= g_dwE1 ) )
					{
						prt(lpd);
					}
					else if( num == g_dwC )
					{
						sprintf( lpd,
							"\t*** Omitted %d Lines ***\r\n",
							(g_dwF1Lines - g_dwMaxLines) );
						prt(lpd);
					}
					else if( ( num >= g_dwB2 ) && ( num <= g_dwE2 ) )
					{
						prt(lpd);
					}
					// ====================================
					k = l = 0;
					dncr = TRUE;
					num++;
					if( g_bAddNums )
					{
						sprintf( lpd,
						"%5u ",
						num );
						k = strlen(lpd);
					}
				}
				if( tc == '\r' )
				{
					if( (i + 1) < j )
					{
						if( lpb1[i+1] == '\n' )
						{
							dncr = FALSE;
							i++;
						}
					}
				}
			}
			else
			{
				lpd[k++] = tc;
				dncr = FALSE;
				l++;
			}
		}

		lpd[k] = 0;
//		if( !dncr )
//			strcat( lpd, MCRLF );
		//strcat( lpd, MCRLF );
		if( l )
			prt(lpd);


		// Output from File 2
		// ==================
		*lpd = 0;
		//strcpy( lpd, MCRLF"----------" );
		//strcat( lpd, &lpmps2->mp_Nm[0] );
		//strcat( lpd, MCRLF );
		strcpy( lpd, gszLead );
		strcat( lpd, &lpmps2->mp_Nm[0] );
		strcat( lpd, gszMCrLf );
		prt(lpd);
		k = 0;
		// Get length in FILE 2
		j = paf2->To.c_dwBgn - paf2->From.c_dwBgn;
		lpb2 = &paf2->lpBuf[paf2->From.c_dwBgn];
		num = paf2->From.c_dwLns;
		if( g_bAddNums )
		{
			sprintf( lpd,
				"%5u ",
				num );
			k = strlen(lpd);
		}
		l = 0;
		g_dwB1 = num;
		g_dwE1 = num + g_dwF2Lines;
		g_dwC = g_dwB2 = g_dwE2 = 0;
		if( ( g_dwMaxLines ) &&
			( g_dwF2Lines > (g_dwMaxLines+1) ) )
		{
			g_dwE1 = num + (g_dwMaxLines / 2);
			g_dwC  = num + (g_dwF2Lines / 2);
			g_dwB2 = num + ((g_dwF2Lines - 1) - (g_dwMaxLines - (g_dwMaxLines / 2)));
			g_dwE2 = num + g_dwF2Lines;
		}

		for( i = 0; i < j; i++ )
		{
			tc = lpb2[i];
			if( ( tc < ' ' ) &&
				( tc != '\t' ) )
			{
				if( !dncr )
				{
					lpd[k] = 0;
					strcat( lpd, MCRLF );
					//prt(lpd);
					// OUTPUT THE LINE
					// ====================================
					if( ( num >= g_dwB1 ) && ( num <= g_dwE1 ) )
					{
						prt(lpd);
					}
					else if( num == g_dwC )
					{
						sprintf( lpd,
							"\t*** Omitted %d Lines ***.\r\n",
							(g_dwF2Lines - g_dwMaxLines) );
						prt(lpd);
					}
					else if( ( num >= g_dwB2 ) && ( num <= g_dwE2 ) )
					{
						prt(lpd);
					}
					// ====================================
					k = l = 0;
					dncr = TRUE;
					num++;
					if( g_bAddNums )
					{
						sprintf( lpd,
						"%5u ",
						num );
						k = strlen(lpd);
					}
				}
				if( tc == '\r' )
				{
					if( (i + 1) < j )
					{
						if( lpb2[i+1] == '\n' )
						{
							dncr = FALSE;
							i++;	// bump past CR/LF pair
						}
					}
				}
			}
			else
			{
				lpd[k++] = tc;
				dncr = FALSE;
				l++;
			}
		}	// For all the bytes

		lpd[k] = 0;
		if( l )
			prt(lpd);

//		if( !dncr )
//			strcat( lpd, MCRLF );
		strcpy( lpd, gszTail );
		//strcat( lpd, MCRLF );
		//strcat( lpd, "-----------------------------------------------" );
		//strcat( lpd, MCRLF );
		prt(lpd);

	}
}

void	SetDbgOn( WS )
{
//#define		Dbg_Bit1		0x00000001
//#define		Dbg_Bit2		0x00000002
//#define		Dbg_Bit3		0x00000004
	g_dwDbgBits = (DWORD)-1;
}

void	SetDisplay( WS, LPAFILE paf1, LPAFILE paf2 )
{
	int		i, j;

	// Set F1 to start at SAVE 2
	/* paf1->Next = paf1->Save2; */
	// Set F2 to start at SAVE 2
	/* paf2->Next = paf2->Save2; */

	/* set END OF DISPLAY of LINE */
	paf1->Save3 = paf1->Next;
	paf2->Save3 = paf2->Next;
	/* but maybe show 1 or more additional */
   j = giShowNextLns;
   if(VERBAL8) {
      sprintf(gszTemp, "SetDisplay: Save of Next to Save3, F1(%d) & F2(%d) ...(j=%d)"MEOR,
         paf1->Next.c_dwLns, paf2->Next.c_dwLns, j );
   }
	if( j )
	{
		// found this F1 line in F2
		// Get NEXT line of File 1
		paf1->Next = paf1->Save2;
		// Get NEXT after found of F2
		paf2->Next = paf2->Save2;
      if(VERBAL8) {
         sprintf(gszTemp, "SetDisplay: Restore Next from Save2, F1(%d) & F2(%d) ..."MEOR,
            paf1->Next.c_dwLns, paf2->Next.c_dwLns );
         prt(gszTemp);
      }
		for( i = 0; i < j; i++ )
		{
			DownALine( pWs, paf1 );
			DownALine( pWs, paf2 );
		}

		paf1->Save3 = paf1->Next;
		// restore beginning F1
		//	paf1->Next  = paf1->Save1;
		paf2->Save3 = paf2->Next;
		// restore beginning F2
		//	paf2->Next  = paf2->Save1;
      if(VERBAL8) {
         sprintf(gszTemp, "SetDisplay: Save Next to Save3, F1(%d) & F2(%d) ..."MEOR,
            paf1->Next.c_dwLns, paf2->Next.c_dwLns );
         prt(gszTemp);
      }

	}

	// Set the output limits
	// =====================
	paf1->From = paf1->Save1;
	paf1->To   = paf1->Save3;	/* end of display F1 */

	paf2->From = paf2->Save1;
	paf2->To   = paf2->Save3;	/* end of display F2 */
   if(VERBAL8) {
      sprintf(gszTemp, "SetDisplay: Set output, F1(%d-%d) & F2(%d-%d) ..."MEOR,
         paf1->From.c_dwLns, paf1->To.c_dwLns,
         paf2->From.c_dwLns, paf2->To.c_dwLns);
      prt(gszTemp);
   }

	/* restore current pointers to start of display  */
	// Set F1 to start at SAVE 2
	paf1->Next = paf1->Save2;
	// Set F2 to start at SAVE 2
	paf2->Next = paf2->Save2;
   if(VERBAL8) {
      sprintf(gszTemp, "SetDisplay: Restore Next from Save2, F1(%d) & F2(%d) ..."MEOR,
         paf1->Next.c_dwLns, paf2->Next.c_dwLns );
      prt(gszTemp);
   }

}

void	SetEnds( WS,	// work structure
				LPAFILE paf1, LPAFILE paf2 )
{
	LPTSTR	lpd = gszDiag;
	int		i;
	static	int sidnse = 0;

	if( !sidnse )
	{
		sidnse = 1;
		paf1->Save1 = paf1->Next;
		paf2->Save1 = paf2->Next;

		gdwLns1 = gdwLns2 = 0;
		// Kill any DEBUG
		g_dwDbgSave = g_dwDbgBits;
		g_dwDbgBits = 0;
      if( VERBAL8 ) {
         prt( "Counting lines in file 1 ..." );
      }

		while( DownALine( pWs, paf1 ) ) {
			gdwLns1++;
		}
      if( VERBAL8 ) {
         sprintf(lpd, " Got %d lines."MEOR, gdwLns1);
         prt(lpd);
         prt( "Counting lines in file 2 ..." );
      }
		while( DownALine( pWs, paf2 ) ) {
			gdwLns2++;
		}
      if( VERBAL8 ) {
         sprintf(lpd, " Got %d lines."MEOR, gdwLns2);
         prt(lpd);
      }

		/* Establish the ENDS */
		paf1->End = paf1->Next;
		paf2->End = paf2->Next;

		/* restore beginning */
		paf1->Next = paf1->Save1;
		paf2->Next = paf2->Save1;

		/* restore any DEBUG */
		g_dwDbgBits = g_dwDbgSave;
		g_dwDbgSave = 0;
		for( i = 0; i < MXSAF; i++ )
		{
			gdwDownCnt[i] = 0;
			gdwUpCnt[i]   = 0;
		}
	}
}

#ifdef   USE_INT64_TYPE
void Output_Hex( WS, PTSTR pbuf, off64_t dwlen, off64_t dwcnt )
{
   LPTSTR lpd = gszDiag;
#ifdef USE_INT64_TYPE  // FIX20080819 - add some 64-bit support
   off64_t	dwi, dwmax, dwstart, dwc;
#else // !USE_INT64_TYPE
   DWORD	dwi, dwmax, dwstart, dwc;
#endif // USE_INT64_TYPE
   dwmax = dwlen; // keep remainder
   if( dwmax == 1 )
      strcpy(lpd, MCRLF"Next 1 Byte: (in hex)"MCRLF);
   else {
#ifdef USE_INT64_TYPE  // FIX20080819 - add some 64-bit support
      sprintf( lpd, MCRLF"Next %lld Bytes: (in hex)"MCRLF, dwmax );
#else // !USE_INT64_TYPE
      sprintf( lpd, MCRLF"Next %d Bytes: (in hex)"MCRLF, dwmax );
#endif // USE_INT64_TYPE y/n
   }
   prt(lpd);
   *lpd = 0;
   while(dwmax) {
      dwstart = dwcnt;
      if( dwlen > 16 ) dwlen = 16;
      dwmax -= dwlen;
   	for( dwi = 0; dwi < dwlen; dwi++ ) {
         if( dwi == 0 ) {
            sprintf( EndBuf(lpd), "%08llu: ", dwcnt );
         }
         sprintf( EndBuf(lpd),
		   	"%02X ",
				pbuf[dwcnt] );
				dwcnt++;
	   }
      while( dwi < 16 ) {
         strcat(lpd,"   ");
         dwi++;
      }
      strcat(lpd," ");
   	for( dwi = 0; dwi < dwlen; dwi++ ) {
         dwc = pbuf[dwstart];
         if(( dwc < 0x20 )||(dwc & 0x80)) {
            sprintf( EndBuf(lpd),
		   		"%c", '.');
         } else {
            sprintf( EndBuf(lpd),
		   		"%c", dwc);
         }
			dwstart++;
		}
      strcat(lpd, MCRLF);
		prt(lpd);
      *lpd = 0;
      dwlen = dwmax;
   }
}

void	DoEndComp( WS, LPAFILE paf1, LPAFILE paf2, UL * pdwIn )
{
	LPTSTR	lpd = gszDiag;
	off64_t	dwcnt, dwmin;
	LPAFILE	paf;

	dwcnt = *pdwIn;
	if( giSameCnt > 1 ) {
			prtlast();
			giSameCnt = 0;
	}

   // if DONE count is LESS that FULL COUNT
   // =====================================
   // #define		paf1->Next.c_dwI		paf1->Next.c_dwI
   // #define		paf2->Next.c_dwI		paf2->Next.c_dwI
   // #define		paf1->dwSz		paf1->dwSz
   // #define		paf2->dwSz		paf2->dwSz
	if( ( paf1->Next.c_dwI < paf1->dwSz ) ||
		 ( paf2->Next.c_dwI < paf2->dwSz ) )
	{
      // THERE IS A REMAINDER IN ONE OR THE OTHER FILE 
      // or BOTH, but I do not think this can happen???
		chkchk();
		paf = 0;
		if( dwcnt == 0 ) {
			if( VERBAL ) {
            if( paf1->Prev1.c_dwLns == paf2->Prev1.c_dwLns )	/* check Prev1 line numbers */
            {
               sprintf(lpd, MCRLF"Rem - Files are exactly the same for %u line(s) ...",
                  (paf1->Prev1.c_dwLns + 1) );
               prt(lpd);
            }
            else
            {
               prt( MCRLF"Rem - Files are exactly the same ..." );
            }
			}
			dwcnt++;
		} else if( VERBAL ) {
			prt( MCRLF"Rem - Files are different ..." );
		}

		if( ( paf1->Next.c_dwI < paf1->dwSz ) &&
			 ( paf2->Next.c_dwI < paf2->dwSz ) ) {
			dwmin = 0;
			sprintf( lpd,
				MCRLF"But %u bytes left in F1[%s],"
				MCRLF"and %u bytes left in F2[%s].",
				(paf1->dwSz - paf1->Next.c_dwI),
				&paf1->lpMP->mp_Nm[0],
				(paf2->dwSz - paf2->Next.c_dwI),
				&paf2->lpMP->mp_Nm[0] );
		}
		else if( paf1->Next.c_dwI < paf1->dwSz )
		{
			dwmin = paf1->dwSz - paf1->Next.c_dwI;
			dwcnt = paf1->Next.c_dwI;
			paf = paf1;
			sprintf( lpd,
				MCRLF"But %u bytes left in F1[%s],",
				dwmin,
				&paf1->lpMP->mp_Nm[0] );
		}
		else // if( paf2->Next.c_dwI < paf2->dwSz )
		{
			dwmin = paf2->dwSz - paf2->Next.c_dwI;
			dwcnt = paf2->Next.c_dwI;
			paf = paf2;
			sprintf( lpd,
				MCRLF"But %u bytes left in F2[%s],",
				dwmin,
				&paf2->lpMP->mp_Nm[0] );			
		}

      // if verbal, give output ...
		if( VERBAL ) {
			prt( lpd );
         chkchk();
         // MAYBE should also do
      	// For backup displaying
		   paf1->Prev3 = paf1->Prev2;
		   paf1->Prev2 = paf1->Prev1;
		   paf1->Prev1 = paf1->Next;

		   paf2->Prev3 = paf2->Prev2;
		   paf2->Prev2 = paf2->Prev1;
		   paf2->Prev1 = paf2->Next;

			//paf1->Save2 = paf1->End;
			//paf2->Save2 = paf2->End;
         //paf1->Save2 = paf1->Next;
			//paf2->Save2 = paf2->Next;
         if( paf1->Next.c_dwI < paf1->dwSz ) {
            while( paf1->Next.c_dwI < paf1->dwSz ) {
               if( !DownALine( pWs, paf1 ) )
                  break;
            }
         }
         while( paf1->Next.c_dwI < paf1->dwSz ) {
            if( !DownALine( pWs, paf2 ) )
               break;
         }
         // establish the display parameters
			//SetDisplay( pWs, paf1, paf2 );
         //paf1->From = paf1->Prev2;
         paf1->From = paf1->Prev1;
	      //paf1->To   = paf1->Next;	/* end of display F1 */
	      paf1->To   = paf1->End;	/* end of display F1 */

	      //paf2->From = paf2->Prev2;
	      paf2->From = paf2->Prev1;
	      //paf2->To   = paf2->Next;	/* end of display F2 */
	      paf2->To   = paf2->End;	/* end of display F2 */

			// show FROM to TO
			// ===============
			ShowDiff( pWs, paf1, paf2 );
			if( VERBAL2 ) {
				if( ( paf   ) &&
					 ( dwmin ) ) {
               Output_Hex( pWs, paf->lpBuf, dwmin, dwcnt );
				}
         }
		}

		if( dwcnt == 0 ) dwcnt++;
	}
	else if( dwcnt )
	{
		if( VERBAL )
		{
			//prt( MCRLF"End - Files are different!" );
         if( g_dwNotExact )
         {
            sprintf( lpd, MCRLF"End - Files are different! Using -X%s",
               GetsszNX(pWs) );
         }
         else
         {
            //prt( MCRLF"End - Files are different!" );
            strcpy( lpd, MCRLF"End - Files are different!" );
         }
         prt(lpd);
		}
	}
	else
	{
		if( VERBAL )
		{
			if( g_dwNotExact )
			{
				sprintf( lpd,
					MCRLF"End - Files are the same using -X%s!",
					GetsszNX( pWs ) );
			}
			else
			{
				strcpy( lpd, MCRLF"End - Files are exactly the same!" );
			}
			prt(lpd);
		}
	}

   if( VERBAL )
      prt( MCRLF );  // FIX20081222 - add CRLF at end

	*pdwIn = (DWORD)dwcnt;
}


#else // !USE_INT64_TYPE
void Output_Hex( WS, PTSTR pbuf, DWORD dwlen, DWORD dwcnt )
{
   LPTSTR lpd = gszDiag;
   DWORD	dwi, dwmax, dwstart, dwc;

   dwmax = dwlen; // keep remainder
   if( dwmax == 1 )
      strcpy(lpd, MCRLF"Next 1 Byte: (in hex)"MCRLF);
   else
      sprintf( lpd, MCRLF"Next %d Bytes: (in hex)"MCRLF, dwmax );
   prt(lpd);
   *lpd = 0;
   while(dwmax) {
      dwstart = dwcnt;
      if( dwlen > 16 ) dwlen = 16;
      dwmax -= dwlen;
   	for( dwi = 0; dwi < dwlen; dwi++ ) {
         if( dwi == 0 ) {
            sprintf( EndBuf(lpd), "%08u: ", dwcnt );
         }
         sprintf( EndBuf(lpd),
		   	"%02X ",
				pbuf[dwcnt] );
				dwcnt++;
	   }
      while( dwi < 16 ) {
         strcat(lpd,"   ");
         dwi++;
      }
      strcat(lpd," ");
   	for( dwi = 0; dwi < dwlen; dwi++ ) {
         dwc = pbuf[dwstart];
         if(( dwc < 0x20 )||(dwc & 0x80)) {
            sprintf( EndBuf(lpd),
		   		"%c", '.');
         } else {
            sprintf( EndBuf(lpd),
		   		"%c", dwc);
         }
			dwstart++;
		}
      strcat(lpd, MCRLF);
		prt(lpd);
      *lpd = 0;
      dwlen = dwmax;
   }
}

void	DoEndComp( WS, LPAFILE paf1, LPAFILE paf2, UL * pdwIn )
{
	LPTSTR	lpd = gszDiag;
	UL		dwcnt, dwmin;
	LPAFILE	paf;

	dwcnt = *pdwIn;
	if( giSameCnt > 1 ) {
			prtlast();
			giSameCnt = 0;
	}

   // if DONE count is LESS that FULL COUNT
   // =====================================
   // #define		paf1->Next.c_dwI		paf1->Next.c_dwI
   // #define		paf2->Next.c_dwI		paf2->Next.c_dwI
   // #define		paf1->dwSz		paf1->dwSz
   // #define		paf2->dwSz		paf2->dwSz
	if( ( paf1->Next.c_dwI < paf1->dwSz ) ||
		 ( paf2->Next.c_dwI < paf2->dwSz ) )
	{
      // THERE IS A REMAINDER IN ONE OR THE OTHER FILE 
      // or BOTH, but I do not think this can happen???
		chkchk();
		paf = 0;
		if( dwcnt == 0 ) {
			if( VERBAL ) {
            if( paf1->Prev1.c_dwLns == paf2->Prev1.c_dwLns )	/* check Prev1 line numbers */
            {
               sprintf(lpd, MCRLF"Rem - Files are exactly the same for %u line(s) ...",
                  (paf1->Prev1.c_dwLns + 1) );
               prt(lpd);
            }
            else
            {
               prt( MCRLF"Rem - Files are exactly the same ..." );
            }
			}
			dwcnt++;
		} else if( VERBAL ) {
			prt( MCRLF"Rem - Files are different ..." );
		}

		if( ( paf1->Next.c_dwI < paf1->dwSz ) &&
			 ( paf2->Next.c_dwI < paf2->dwSz ) ) {
			dwmin = 0;
			sprintf( lpd,
				MCRLF"But %u bytes left in F1[%s],"
				MCRLF"and %u bytes left in F2[%s].",
				(paf1->dwSz - paf1->Next.c_dwI),
				&paf1->lpMP->mp_Nm[0],
				(paf2->dwSz - paf2->Next.c_dwI),
				&paf2->lpMP->mp_Nm[0] );
		}
		else if( paf1->Next.c_dwI < paf1->dwSz )
		{
			dwmin = paf1->dwSz - paf1->Next.c_dwI;
			dwcnt = paf1->Next.c_dwI;
			paf = paf1;
			sprintf( lpd,
				MCRLF"But %u bytes left in F1[%s],",
				dwmin,
				&paf1->lpMP->mp_Nm[0] );
		}
		else // if( paf2->Next.c_dwI < paf2->dwSz )
		{
			dwmin = paf2->dwSz - paf2->Next.c_dwI;
			dwcnt = paf2->Next.c_dwI;
			paf = paf2;
			sprintf( lpd,
				MCRLF"But %u bytes left in F2[%s],",
				dwmin,
				&paf2->lpMP->mp_Nm[0] );			
		}

      // if verbal, give output ...
		if( VERBAL ) {
			prt( lpd );
         chkchk();
         // MAYBE should also do
      	// For backup displaying
		   paf1->Prev3 = paf1->Prev2;
		   paf1->Prev2 = paf1->Prev1;
		   paf1->Prev1 = paf1->Next;

		   paf2->Prev3 = paf2->Prev2;
		   paf2->Prev2 = paf2->Prev1;
		   paf2->Prev1 = paf2->Next;

			//paf1->Save2 = paf1->End;
			//paf2->Save2 = paf2->End;
         //paf1->Save2 = paf1->Next;
			//paf2->Save2 = paf2->Next;
         if( paf1->Next.c_dwI < paf1->dwSz ) {
            while( paf1->Next.c_dwI < paf1->dwSz ) {
               if( !DownALine( pWs, paf1 ) )
                  break;
            }
         }
         while( paf1->Next.c_dwI < paf1->dwSz ) {
            if( !DownALine( pWs, paf2 ) )
               break;
         }
         // establish the display parameters
			//SetDisplay( pWs, paf1, paf2 );
         //paf1->From = paf1->Prev2;
         paf1->From = paf1->Prev1;
	      //paf1->To   = paf1->Next;	/* end of display F1 */
	      paf1->To   = paf1->End;	/* end of display F1 */

	      //paf2->From = paf2->Prev2;
	      paf2->From = paf2->Prev1;
	      //paf2->To   = paf2->Next;	/* end of display F2 */
	      paf2->To   = paf2->End;	/* end of display F2 */

			// show FROM to TO
			// ===============
			ShowDiff( pWs, paf1, paf2 );
			if( VERBAL2 ) {
				if( ( paf   ) &&
					 ( dwmin ) ) {
               Output_Hex( pWs, paf->lpBuf, dwmin, dwcnt );
				}
         }
		}

		if( dwcnt == 0 ) dwcnt++;
	}
	else if( dwcnt )
	{
		if( VERBAL )
		{
			//prt( MCRLF"End - Files are different!" );
         if( g_dwNotExact )
         {
            sprintf( lpd, MCRLF"End - Files are different! Using -X%s",
               GetsszNX(pWs) );
         }
         else
         {
            //prt( MCRLF"End - Files are different!" );
            strcpy( lpd, MCRLF"End - Files are different!" );
         }
         prt(lpd);
		}
	}
	else
	{
		if( VERBAL )
		{
			if( g_dwNotExact )
			{
				sprintf( lpd,
					MCRLF"End - Files are the same using -X%s!",
					GetsszNX( pWs ) );
			}
			else
			{
				strcpy( lpd, MCRLF"End - Files are exactly the same!" );
			}
			prt(lpd);
		}
	}
	*pdwIn = dwcnt;
}

#endif // USE_INT64_TYPE


int	DoRockComp( WS, LPAFILE paf1, LPAFILE paf2,
			   BOOL * pf1, BOOL * pf2 )
{
   LPTSTR lpd = gszDiag;
	BOOL	flg = FALSE;
	BOOL	flg1, flg2;
	int		i = 0;

	flg1 = *pf1;
	flg2 = *pf2;
	//SetDbgOn( pWs );
	//flg1 = flg2 = FALSE;
   if( VERBAL8 ) {
      sprintf( gszTemp, "DoRockComp: F1(%d) F2(%d) Take paf1 down a line ..."MEOR,
         paf1->Next.c_dwLns, paf2->Next.c_dwLns);
      prt(gszTemp);
   }

	while( DownALine( pWs, paf1 ) )
	{
		// while we can
		// continue DOWN in F1
		// finding this new line in F2
      // now in CompALine
      //if( VERBAL9 ) {
      //   sprintf(lpd, "%5d: [%s]%d"MEOR, paf1->Next.c_dwLns, GetCLine(paf1), paf1->Next.c_dwCLen);
      //   prt(lpd);
      //}
		if( paf1->Next.c_dwCLen )
		{
			if( CompALine( pWs, paf1, paf2 ) )
			{
            if( VERBAL9 ) {
               prt( "Finding F1 line in F2 ..." );
            }
				if( FindLineIn2( pWs, paf1, paf2 ) )
				{
					flg1 = TRUE;
					flg  = TRUE;
               if( VERBAL9 ) {
                  sprintf(lpd, " Found at %d line."MEOR, paf2->Next.c_dwLns);
                  prt( lpd );
               }
            } else {
               if( VERBAL9 ) {
                  prt( " Not found!"MEOR );
               }
            }
			}
			else
			{
				flg1 = TRUE;
				flg  = TRUE;
			}
			if( flg )
			{
				// Keep the NEW positions
				// of BOTH files
				paf1->Save2 = paf1->Next;
				paf2->Save2 = paf2->Next;
            if(VERBAL8) {
               sprintf(gszTemp, "DoRockComp: Save Next to Save2, F1(%d) & F2(%d) ..."MEOR,
                  paf1->Next.c_dwLns, paf2->Next.c_dwLns );
               prt(gszTemp);
            }
				break;
			}
			// Restore original F2 for next DOWN in F1
			paf2->Next  = paf2->Save1;
		}	// only IF the line HAS characters
	}

	// Restore originals
	paf1->Next  = paf1->Save1;
	paf2->Next  = paf2->Save1;
   if(VERBAL8) {
      sprintf(gszTemp, "DoRockComp: Restore Next from Save1, F1(%d) & F2(%d) ..."MEOR,
         paf1->Next.c_dwLns, paf2->Next.c_dwLns );
      prt(gszTemp);
   }
	flg = FALSE;
	while( DownALine( pWs, paf2 ) )
	{
		// while we can
		// continue DOWN in F2
		// finding this new line in F1
		if( paf2->Next.c_dwCLen )
		{
			if( CompALine( pWs, paf1, paf2 ) )
			{
				if( FindLineIn1( pWs, paf1, paf2 ) )
				{
					flg2 = TRUE;
					flg  = TRUE;
				}
			}
			else
			{
				flg2 = TRUE;
				flg  = TRUE;
			}
			if( flg )
			{
				// Keep the NEW positions
				// of BOTH files
				//paf1->Save3 = paf1->Next;
				//paf2->Save3 = paf2->Next;
    //        if(VERBAL8) {
    //           sprintf(gszTemp, "DoRockComp: Save Next to Save3, F1(%d) & F2(%d) ..."MEOR,
    //              paf1->Next.c_dwLns, paf2->Next.c_dwLns );
    //           prt(gszTemp);
    //        }
				paf1->Save2 = paf1->Next;
				paf2->Save2 = paf2->Next;
            if(VERBAL8) {
               sprintf(gszTemp, "DoRockComp: Save Next to Save2, F1(%d) & F2(%d) ..."MEOR,
                  paf1->Next.c_dwLns, paf2->Next.c_dwLns );
               prt(gszTemp);
            }
				break;
			}
			// Restore original F2 for next DOWN in F1
			paf1->Next  = paf1->Save1;
		}	// only IF the line HAS characters
	}
	// Restore originals
	paf1->Next  = paf1->Save1;
	paf2->Next  = paf2->Save1;
   if(VERBAL8) {
      sprintf(gszTemp, "DoRockComp: Restore Next from Save1, F1(%d) & F2(%d) ... %s"MEOR,
         paf1->Next.c_dwLns, paf2->Next.c_dwLns,
         (( flg1 | flg2 ) ? "FOUND" : "NOT FOUND" ));
      prt(gszTemp);
   }

	*pf1 = flg1;
	*pf2 = flg2;

	if( flg1 )
		i++;
	else if( flg2 )
		i++;

	return i;
}

LPTSTR GetNxtBuf( VOID );
PTSTR GetCLine( LPAFILE paf )
{
   LPTSTR pb = GetNxtBuf();
   DWORD dwm = GetNxtBufSize();
   LPTSTR pl = &paf->lpBuf[paf->Next.c_dwBgn];
   DWORD dwl = paf->Next.c_dwCLen;
   DWORD dwi;
   if(  dwl < dwm ) {
      for( dwi = 0; dwi < dwl; dwi++ ) {
         pb[dwi] = pl[dwi];
      }
      pb[dwi] = 0;
   } else { // deal with a BIG line
      // get first 120 of line
      DWORD dwj = 120 + (dwl - 240);
      for( dwi = 0; dwi < 120; dwi++ ) {
         pb[dwi] = pl[dwi];
      }
      pb[dwi] = 0;
      strcat(pb,"...");
      dwi += 3;
      for( ; dwj < dwl; dwj++ ) {
         pb[dwi++] = pl[dwj];
      }
      pb[dwi] = 0;
   }

   // for FULL diagnostic purposes, ADD ANY CR/LF to line image returned
   for( ; dwl < paf->Next.c_dwLen; dwl++ )
   {
      TCHAR c = pl[dwl];
      if( c < ' ' ) {
         c += '@';
         pb[dwi++] = '^';
         pb[dwi++] = c;
         pb[dwi] = 0;
      } else {
         break;
      }
   }

   return pb;
}
// =================================================
// DWORD	DoComp( WS, LPAFILE paf1, LPAFILE paf2 )
//
// Compare file 1 with 2, and
// return the number of LINE differences.
//
//typedef struct {
//	UL	c_dwBgn;	/* offset to beginning of line */
//	UL	c_dwEnd;	/* offset to end       of line */
//	UL	c_dwLen;	/* LENGTH of the data block    */
//	UL	c_dwLns;	/* current LINE number         */
//	UL	c_dwI;
//	UL	c_dwCLen;	/* character length, excl crlf */
//}ALINE;
// =================================================
// this does NOT need any 64-bit changes, due to the fact
// that is seems VERY LARGE, especially greater than a
// DWORD max, 0xffffffff = 4,294,967,295 bytes will
// FAIL in the file mapping process, and ONLY a BINARY
// compare is possible ...
DWORD	DoComp( WS, LPAFILE paf1, LPAFILE paf2 )
{
	DWORD	dwcnt, dwmin;
	BOOL	flg1, flg2;
	LPSTR	lpb;
	LPSTR	lpd = gszDiag;
	TCHAR	tc;

	SetEnds( pWs, paf1, paf2 );

	dwcnt = 0;
   if( VERBAL9 ) {
      prt( "Doing compare..."MEOR );
   }
   // while BOTH files HAVE data
   // ==========================
	while( ( paf1->Next.c_dwI < paf1->dwSz ) &&
		    ( paf2->Next.c_dwI < paf2->dwSz ) )
	{
		// For backup displaying
		paf1->Prev3 = paf1->Prev2;
		paf1->Prev2 = paf1->Prev1;
		paf1->Prev1 = paf1->Next;

		paf2->Prev3 = paf2->Prev2;
		paf2->Prev2 = paf2->Prev1;
		paf2->Prev1 = paf2->Next;

		if( !DownALine( pWs, paf1 ) )
		{
         if( VERBAL9 ) {
            prt( "Out of lines in file 1..."MEOR );
         }
			chkchk();
			break;
		}
		else
		{
         // this is now output in COMPARE
         //if( VERBAL9 ) {
         //   sprintf(lpd, "%5d: [%s]%d"MEOR, paf1->Next.c_dwLns, GetCLine(paf1), paf1->Next.c_dwCLen);
         //   prt(lpd);
         //}
			paf1->Save3 = paf1->Next;
			if( g_bSkipSpaces )   // = ( g_dwNotExact & x_SkipSpaces )
			{
FindF1Line:
				lpb = &paf1->lpBuf[paf1->Next.c_dwBgn];
				if( paf1->Next.c_dwCLen )
				{
					// we COULD scan the line.
					// it COULD be all blanks
					tc = *lpb++;	// Get first and bump
					while( tc <= ' ' )
					{
						// The FIRST is
						if( ( tc == '\r' ) ||
							( tc == '\n' ) ||
							( tc == 0x1a ) ||
							( tc == 0    ) )
						{
							tc = '\r';
							break;
						}
						tc = *lpb++;	// Get first and bump
					}
					if( tc == '\r' )
					{
						//LPTSTR	lptmp;
						//lptmp = &paf1->lpBuf[paf1->Next.c_dwBgn];
						if( !DownALine( pWs, paf1 ) )
						{
							// ran out finding next non-space line
							if( paf1->Next.c_dwI < paf1->dwSz )
								chkchk();
							else
								chkend();
						}
						else
						{
							goto FindF1Line;
						}
					}
				}
				else
				{
					if( !DownALine( pWs, paf1 ) )
					{
						// ran out finding next non-space line
						if( paf1->Next.c_dwI < paf1->dwSz )
							chkchk();
						else
							chkend();
					}
					else
					{
						goto FindF1Line;
					}
				}
			}
		}

		if( !DownALine( pWs, paf2 ) )
		{
         if( VERBAL9 ) {
            prt( "Out of lines in file 2..."MEOR );
         }
			chkchk();
			break;
		}
		else
		{
         // this is now output in COMPARE
         //if( VERBAL9 ) {
         //   sprintf(lpd, "%5d: [%s]%d"MEOR, paf2->Next.c_dwLns, GetCLine(paf2), paf2->Next.c_dwCLen);
         //   prt(lpd);
         //}
			paf2->Save3 = paf2->Next;
			if( g_bSkipSpaces )   // = ( g_dwNotExact & x_SkipSpaces )
			{
FindF2Line:
				lpb = &paf2->lpBuf[paf2->Next.c_dwBgn];
				if( paf2->Next.c_dwCLen )
				{
					// we COULD scan the line.
					// it COULD be all blanks
					tc = *lpb++;	// get next and bump
					while( tc <= ' ' )
					{
						// The FIRST is
						if( ( tc == '\r' ) ||
							( tc == '\n' ) ||
							( tc == 0x1a ) ||
							( tc == 0    ) )
						{
							tc = '\r';
							break;
						}
						tc = *lpb++;
					}
					if( tc == '\r' )
					{
						//LPTSTR	lptmp;
						//lptmp = &paf1->lpBuf[paf1->Next.c_dwBgn];
						if( !DownALine( pWs, paf2 ) )
						{
							// ran out finding next non-space line
							if( paf2->Next.c_dwI < paf2->dwSz )
								chkchk();
							else
								chkend();
						}
						else
						{
							goto FindF2Line;
						}
					}
				}
				else
				{
					if( !DownALine( pWs, paf2 ) )
					{
						// ran out finding next non-space line
						if( paf2->Next.c_dwI < paf2->dwSz )
							chkchk();
						else
							chkend();
					}
					else
					{
						goto FindF2Line;
					}
				}
			}
		}

		// NOTE: If skip spaces is on, ONLY significant
		// lines will be compared. BLANKS lines will be SKIPPED
		// ====================================================
		if( CompALine( pWs, paf1, paf2 ) )
      {
			// ====================
			chkdiff();
			dwcnt++;
			flg1 = FALSE;
			flg2 = FALSE;

			gbFnd1 = gbFnd2 = FALSE;
			gbFndIn1 = gbFndIn2 = FALSE;
			/* save the FILE 1 position into global Active 1 */
			gsAct1 = paf1->Next;
			/* save the FILE 2 position into global Active 2 */
			gsAct2 = paf2->Next;
//			if( dwcnt == 2 )
//			{
//				SetDbgOn( pWs );
//				CompALine( pWs, paf1, paf2 );
//			}
#ifdef	DBGDIAG2
			if( Dbg2 )
			{
				sprintf( lpd,
					MCRLF"Lines different. (%d) Find L1 in F2",
					dwcnt );
			}
#endif	// DBGDIAG2

         if( VERBAL9 ) {
            prt( "We have a difference ... positions saved to gsAct1/gsAct2"MEOR );
         }
			// Keep NEXT
			paf2->Save1 = paf2->Next;
			paf2->Save2 = paf2->Next;
         if( VERBAL8 ) {
            sprintf(gszTemp, "DoComp: Save F2 Next to Save1, Save2, F1(%d) & F2(%d) ..."MEOR,
               paf1->Next.c_dwLns, paf2->Next.c_dwLns );
            prt(gszTemp);
         }

#ifdef	DBGDIAG1
			if( paf1->Next.c_dwCLen )
			{
				if( Dbg1 )
				{
					SaveDbg01;
					sprintf( lpd,
						MCRLF"Comparing: F1[%s] Ln %u with"
						MCRLF"           F2 from line %u to %u (%u comps)!"
						MCRLF,
						GetsszLine(paf1, 30),
						paf1->Next.c_dwLns,
						paf2->Next.c_dwLns,
						paf2->End.c_dwLns,
						(paf2->End.c_dwLns - paf2->Next.c_dwLns) );
					prt(lpd);
				}

				// compare CURRENT F1 line with ALL in F2
				g_dwSaveDbg = g_dwDbgBits;
				if( g_dwMinDbg )
					g_dwDbgBits = g_dwMinDbg;
				else if( gfDnSave )
					g_dwDbgBits = gulSaveDbg;
				else
					g_dwDbgBits = 0;

				/* ========================================= */
            if( VERBAL9 ) {
               prt( "Searching for this F1 line in F2 ..." );
            }
				flg1 = FindLineIn2( pWs, paf1, paf2 );
				/* ========================================= */
            if( VERBAL9 ) {
               if(flg1) {
                  sprintf(lpd, "Found at line %d"MEOR, paf2->Next.c_dwLns);
                  prt(lpd);
               } else {
                  prt( " Not found!"MEOR );
               }
            }

				g_dwDbgBits = g_dwSaveDbg;
				RestDbg01;

				if( Dbg1 )
				{
					*lpd = 0;
					if( gcLastOut != '\n' )
						strcpy( lpd, "\r\n" );
					if( flg1 )
					{
						if( g_dwMatchCnt )
						{
							sprintf( EndBuf(lpd),
								"Found F1 in F2 at %u plus %u line(s) after.",
								paf1->Next.c_dwLns,
								g_dwMatchCnt );
						}
						else
						{
							sprintf( EndBuf(lpd),
								"Found F1 in F2 at %u",
								paf2->Next.c_dwLns );
						}
					}
					else
					{
						if( g_dwMatchCnt )
						{
							sprintf( EndBuf(lpd),
								"NOT found in F2 with -M%u!",
								(g_dwMatchCnt + 1 ) );
						}
						else
						{

							strcat( lpd, "NOT found in F2!" );
						}
					}
					prt(lpd);
				}
				if( flg1 )
				{
					/* note Save2 gets the found if found */
					paf2->Save2 = paf2->Next;
				}
			}	// if there IS character lenght
#else	// !DBGDIAG1
			if( paf1->Next.c_dwCLen )
				flg1 = FindLineIn2( pWs, paf1, paf2 );
			paf2->Save2 = paf2->Next;
#endif	// DBGDIAG1 y/n

			// Restore original
			paf2->Next  = paf2->Save1;
         if( VERBAL8 ) {
            sprintf(gszTemp, "DoComp: Save F2 Next to Save2, F2(%d), Restore Next from Save1, F2(%d) ..."MEOR,
               paf2->Save2.c_dwLns, paf2->Next.c_dwLns );
            prt(gszTemp);
         }

			gbFndIn2 = flg1;

#ifdef	DBGDIAG2
			if( Dbg2 )
			{
				*lpd = 0;
				if( gcLastOut != '\n' )
					strcpy( lpd, "\r\n" );
				strcat( lpd, "Restored F2 Save1 as Next and then find L2 in F1" );
				prt(lpd);
			}
#endif	// DBGDIAG2

			paf1->Save1 = paf1->Next;
			paf1->Save2 = paf1->Next;
         if( VERBAL8 ) {
            sprintf(gszTemp, "DoComp: Save F1 Next to Save1, Save2, F1(%d) & F2(%d) ..."MEOR,
               paf1->Next.c_dwLns, paf2->Next.c_dwLns );
            prt(gszTemp);
         }
#ifdef	DBGDIAG1
			if( paf2->Next.c_dwCLen )
			{
				if( Dbg1 )
				{
					*lpd = 0;
					if( gcLastOut != '\n' )
						strcpy( lpd, "\r\n" );
					SaveDbg01;
					sprintf( EndBuf(lpd),
						     "Comparing: F2[%s] Ln %u with"
						MCRLF"           F1 from line %u to %u (%u comps)!"
						MCRLF,
						GetsszLine(paf2, 30),
						paf2->Next.c_dwLns,
						paf1->Next.c_dwLns,
						paf1->End.c_dwLns,
						(paf1->End.c_dwLns - paf1->Next.c_dwLns) );
					prt(lpd);
				}
				// Compare the CURRENT line in F2
				// with ALL the lines in F1
				g_dwSaveDbg = g_dwDbgBits;
				if( g_dwMinDbg )
					g_dwDbgBits = g_dwMinDbg;
				else if( gfDnSave )
					g_dwDbgBits = gulSaveDbg;
				else
					g_dwDbgBits = 0;

            if( VERBAL9 ) {
               prt( "Searching for this F2 line in F1 ..." );
            }
				/* ========================================== */
				flg2 = FindLineIn1( pWs, paf1, paf2 );
				/* ========================================== */
            if( VERBAL9 ) {
               if(flg2) {
                  sprintf(lpd, "Found at line %d"MEOR, paf1->Next.c_dwLns);
                  prt(lpd);
               } else {
                  prt( " Not found!"MEOR );
               }
            }

				g_dwDbgBits = g_dwSaveDbg;
				RestDbg01;
				if( Dbg1 )
				{

					*lpd = 0;
					if( gcLastOut != '\n' )
						strcpy( lpd, "\r\n" );
					if( flg2 )
					{
						if( g_dwMatchCnt )
						{
							sprintf( EndBuf(lpd),
								"Found F2 in F1 at %u plus %u after.",
								paf1->Next.c_dwLns,
								g_dwMatchCnt );
						}
						else
						{
							sprintf( EndBuf(lpd),
								"Found F2 in F1 at %u",
								paf1->Next.c_dwLns );
						}
					}
					else
					{
						if( g_dwMatchCnt )
						{
							sprintf( EndBuf(lpd),
								"NOT found in F1 with -M%u!",
								(g_dwMatchCnt + 1 ) );
						}
						else
						{
							strcat( lpd, "NOT found in F1!" );
						}
					}
					prt(lpd);
				}
				if( flg2 )
				{
					paf1->Save2 = paf1->Next;	// Save the FIND (if any)
				}
			}	// if there IS character length
#else	// !DBGDIAG1
         if( paf2->Next.c_dwCLen ) {
            if( VERBAL9 ) {
               prt( "Searching for this F2 line in F1 ..." );
            }
				flg2 = FindLineIn1( pWs, paf1, paf2 );
            if( VERBAL9 ) {
               if(flg2) {
                  sprintf(lpd, "Found at line %d"MEOR, paf1->Next.c_dwLns);
                  prt(lpd);
               } else {
                  prt( " Not found!"MEOR );
               }
            }
         }
			paf1->Save2 = paf1->Next;	// Save the FIND (if any)
#endif	// DBGDIAG1 y/n
			// Restore original
			paf1->Next  = paf1->Save1;
         if( VERBAL8 ) {
            sprintf(gszTemp, "DoComp: Save F1 Next to Save2, F1(%d), Restore Next from Save1, F1(%d) ..."MEOR,
               paf1->Save2.c_dwLns, paf1->Next.c_dwLns );
            prt(gszTemp);
         }
			gbFndIn1 = flg2;

			dwmin = mymin( ( paf2->Save2.c_dwLns - paf2->Save1.c_dwLns ),
				( paf1->Save2.c_dwLns - paf2->Save1.c_dwLns ) );

#ifdef	DBGDIAG2
			if( ( Dbg2 ) ||
				( Dbg6 ) )
			{
				*lpd = 0;
				if( gcLastOut != '\n' )
					strcpy( lpd, "\r\n" );
				sprintf( EndBuf(lpd),
					"Results: 1in2=%s 2in1=%s Min=%d",
					( flg1 ? "T" : "F" ),
					( flg2 ? "T" : "F" ),
					dwmin );
				prt( lpd );
			}
#endif	// DBGDIAG2

			if( flg1 && flg2 )
			{
				// Choose which to take of the FOUND
				chkchk();
#ifdef		DBGDIAG5
				if( Dbg5 )
				{
					sprintf( lpd,
						MCRLF"Ambiguous Results: Found -"
						MCRLF"F1[%s] Ln %u in F2 at %u, and"
						MCRLF"F2[%s] Ln %u in F1 at %u"
						MCRLF"Which to choose?",
						GetsszLine(paf1, 30),
						paf1->Next.c_dwLns,
						paf2->Save2.c_dwLns,
						GetsszLine(paf2, 30),
						paf2->Next.c_dwLns,
						paf1->Save2.c_dwLns );

					prt(lpd);
				}
#endif	// DBGDIAG5

//				dwmin = mymin( ( paf2->Save2.c_dwLns - paf2->Save1.c_dwLns ),
//					( paf1->Save2.c_dwLns - paf2->Save1.c_dwLns ) );
				if( dwmin )
				{
					while( ( dwmin ) &&
						( DownALine( pWs, paf1 ) ) &&
						( DownALine( pWs, paf2 ) ) )
					{
						if( ( paf1->Next.c_dwCLen == 0 ) ||
							( paf2->Next.c_dwCLen == 0 ) ||
							( CompALine( pWs, paf1, paf2 ) )  )
						{
							// not same - continue
							dwmin--;
						}
						else
						{
							// ok, found match WITHIN the
							// smallest set-of-lines
							paf1->Save2 = paf1->Next;
							paf2->Save2 = paf2->Next;
							flg1 = TRUE;
							break;
						}
					}
				}
			}

			// NOTE: paf1 & paf2->Next has been restored
			// =========================================
			if(( giActOne ) &&
				( flg1     ) )
			{
				// only choice
				SetDisplay( pWs, paf1, paf2 );
			}
			else if(( giActOne ) &&
				     ( flg2     ) )
			{
				/* one choice is to go with this find of F2 line */
				SetDisplay( pWs, paf1, paf2 );
			}
			else
			{
				// Neither F1 line found in F2, nor
				// found F2 line found in F1
				/* or maybe giActOne is OFF */
				// Like a NEW line in F1
				flg1 = FALSE;
				flg2 = FALSE;
				chkdiff2();
				if( !giActOne )
				{
					SetDisplay( pWs, paf1, paf2 );
					g_dwSaveDbg = g_dwDbgBits;
					if( g_dwMinDbg )
						g_dwDbgBits = g_dwMinDbg;
					else if( gfDnSave )
						g_dwDbgBits = gulSaveDbg;
					else
						g_dwDbgBits = 0;
				}

				/* off the big diags for this rocker compare */
				DoRockComp( pWs, paf1, paf2, &flg1, &flg2 );
				/* ========================================= */

				if( !giActOne )
				{
					g_dwDbgBits = g_dwSaveDbg;
				}

				if( flg1 || flg2 )
				{
					if( flg1 && flg2 )
					{
//						prt( MCRLF"Choice of -" );
//						paf1->Temp = paf1->Save2;
//						paf2->Temp = paf2->Save2;
//						paf1->Save2 = paf1->Save3;
//						paf2->Save2 = paf2->Save3;
//						SetDisplay( pWs, paf1, paf2 );
//						ShowDiff( pWs, paf1, paf2 );
//						prt( MCRLF"Or this -" );
//						paf1->Save2 = paf1->Temp;
//						paf2->Save2 = paf2->Temp;
//						SetDisplay( pWs, paf1, paf2 );
//						ShowDiff( pWs, paf1, paf2 );
//						prt( MCRLF"Which is BEST?" );
						chkdiff2();
						if( ( ( paf2->Save2.c_dwLns - paf2->Save1.c_dwLns ) +
							  ( paf1->Save2.c_dwLns - paf1->Save1.c_dwLns ) ) >
							( ( paf2->Save3.c_dwLns - paf2->Save1.c_dwLns ) +
							  ( paf1->Save3.c_dwLns - paf1->Save1.c_dwLns ) ) )
						{
							// Use the second find
							paf1->Save2 = paf1->Save3;
							paf2->Save2 = paf2->Save3;
						}
					}
					SetDisplay( pWs, paf1, paf2 );
				}
				else
				{
					// This SHOULD be the BALANCE
					// of the FILE.
					paf1->Save2 = paf1->End;
					paf2->Save2 = paf2->End;
					SetDisplay( pWs, paf1, paf2 );
				}
			}
			if( VERBAL )
			{
				// show FROM to TO
				// ===============
				ShowDiff( pWs, paf1, paf2 );
			}

			giSameCnt = 0;

#ifdef	DBGDIAG2
			if( Dbg2 )
			{
				sprintf( lpd,
					MCRLF"Bump F1 to %s",
					GetsszFP( pWs, paf1, 1 ) );
				prt( lpd );
				sprintf( lpd,
					MCRLF"Bump F2 to %s",
					GetsszFP( pWs, paf2, 2 ) );
				prt( lpd );
			}
#endif	// DBGDIAG2
			if( g_bQuick )
			{
				return dwcnt;
			}
		}	// there is a DIFFERENCE
      else { // they are THE SAME
         if( VERBAL9 ) {
            PTSTR pb = GetNxtBuf();
            strcpy(pb, "Lines are the SAME" );
				if((g_dwNotExact & x_SkipSpaces) ||
               (g_dwNotExact & x_IgnoreCase) )
            {
               strcat(pb, " -x");
               if(g_dwNotExact & x_SkipSpaces)
                  strcat(pb, "1");
               if(g_dwNotExact & x_IgnoreCase)
                  strcat(pb, "2");
            }
            strcat(pb,MEOR);
            prt(pb);
         }
      }
	}	// while bytes in BOTH files to process

	/* *** END OF POCESSING *** */
	/* ======================== */

	DoEndComp( pWs, paf1, paf2, &dwcnt );
	/* ======================== */

	return dwcnt;
}

// this does NOT need any 64-bit changes, due to the fact
// that is seems VERY LARGE, especially greater than a
// DWORD max, 0xffffffff = 4,294,967,295 bytes will
// FAIL in the file mapping process, and ONLY a BINARY
// compare is possible ...
int	CompFiles( WS, LPMPSTR lpmps1, LPMPSTR lpmps2 )
{
	int		bRet = Exit_Same;	// assume SAME
	DWORD	dwk;
	LPAFILE	paf1, paf2;

	dwk = 0;
	paf1 = &pWs->ws_AF1;
	paf2 = &pWs->ws_AF2;

	ZeroMemory( paf1, sizeof(AFILE) );
	ZeroMemory( paf2, sizeof(AFILE) );

	if( ( paf1->lpMP = lpmps1 ) &&
		( paf2->lpMP = lpmps2 ) &&
		( paf1->lpBuf = lpmps1->mp_Pv ) &&
		( paf2->lpBuf = lpmps2->mp_Pv ) &&
		( paf1->dwSz = lpmps1->mp_Sz ) &&
		( paf2->dwSz = lpmps2->mp_Sz ) )
	{
		paf1->Next.c_dwI = 0;
		paf2->Next.c_dwI = 0;
		dwk = DoComp( pWs, paf1, paf2 );
	}
	else
	{
		prt( MCRLF"ERROR: Internal parameters failed!" );
		gIRet = Exit_Error;
		PgmExit(TRUE);
	}

	if( dwk )
	{
		bRet = Exit_Diff;
	}

	return bRet;

}

LPTSTR	GetsszFP( WS, LPAFILE paf, int if12 )
{
	LPTSTR	lps, lpp;
	int		i, j, k;
	char	c;
	LPAFILE	lpaf;
	lps = &sszFP[0];
	j = if12;
	if( j >= MXSAF )
		j = (MXSAF - 1);

	lpaf = &gaf[j];
	lpp  = &gsszFPp[j][0];
	k    = 0;

	sprintf( lps,
		"C=%u",
		paf->Next.c_dwLns );
	c = *lps;
	if( strcmp( lpp, lps ) )
	{
		strcpy( lpp, lps );
	}
	else
	{
		i = strlen(lps);
		if(i)
		{
			while( i )
			{
				i--;
				c = lps[k];
				if( c == 'C' )
					lps[k] = '=';
				else if( c == '=' )
					lps[k] = ' ';
				k++;
			}
		}
	}

	return lps;

}

LPTSTR	GetsszFP2( WS, LPAFILE paf, int if12 )
{
	LPTSTR	lps, lpp;
	int		i, j, k;
	char	c;
	LPAFILE	lpaf;
	lps = &sszFP[0];
	j = if12;
	if( j >= MXSAF )
		j = (MXSAF - 1);

	lpaf = &gaf[j];
	lpp  = &gsszFPp[j][0];

	sprintf( lps,
		"B=%u E=%u L=%u C=%u I=%u CL=%u",
		paf->Next.c_dwBgn,
		paf->Next.c_dwEnd,
		paf->Next.c_dwLen,
		paf->Next.c_dwLns,
		paf->Next.c_dwI,
		paf->Next.c_dwCLen );

	if( strcmp( lpp, lps ) )
	{
		strcpy( lpp, lps );
		if( lpaf->Next.c_dwBgn == paf->Next.c_dwBgn )
		{
			i = strlen(lps);
			if (i)
			{
				k = 0;
				while( i-- )
				{
					if( lps[k] <= ' ' )
					{
						break;
					}
					else
					{
						lps[k] = ' ';
					}
					k++;
				}
			}
		}
		else
		{
			lpaf->Next.c_dwBgn = paf->Next.c_dwBgn;
		}
		/* ========================================== */
		if( lpaf->Next.c_dwEnd == paf->Next.c_dwEnd )
		{
			i = strlen(lps);
			if(i)
			{
				k = 0;
				while( i-- )
				{
					c = lps[k];
					if( ( c == 'E' ) &&
						( lps[k+1] == '=' ) )
					{
						break;
					}
					k++;
				}
				if( ( i ) &&
					( i != (int)-1 ) &&
					( c == 'E' ) &&
					( lps[k+1] == '=' ) )
				{
					while( i-- )
					{
						c = lps[k];
						if( c <= ' ' )
							break;
						lps[k++] = ' ';
					}
				}
			}
		}
		else
		{
			lpaf->Next.c_dwEnd = paf->Next.c_dwEnd;
		}

	}
	else
	{
		i = strlen(lps);
		if(i)
		{
			while( i )
			{
				i--;
				lps[i] = ' ';
			}
		}
	}

	return lps;

}

// NOTE: Maximum of TWO (2) successive calls
//	before overwrite!
LPTSTR	GetsszLine( LPAFILE paf, DWORD max )
{
	LPTSTR	lps;
	DWORD	len, dwi;
	LPTSTR	lpb;
	TCHAR	tc;
	static TCHAR sszLN[256*2];
	static int	isszLN;

	isszLN++;
	if( isszLN > 1 ) isszLN = 0;
	lps = &sszLN[ (isszLN * 256) ];
	*lps = 0;
	if( ( paf ) &&
		( len = max ) )
	{
		if( len > 255 ) len = 255;
		lpb = &paf->lpBuf[paf->Next.c_dwBgn];
		for( dwi = 0; dwi < len; dwi++ )
		{
			tc = lpb[dwi];
			if( ( tc < ' ' ) &&
				( tc != '\t' ) )
			{
				break;
			}
			lps[dwi] = tc; 
		}
		lps[dwi] = 0;
	}
	return lps;
}

LPTSTR	GetsszNX( WS )
{
	LPTSTR	lps;
	static TCHAR sszNX[64];

	lps = &sszNX[0];
	*lps = 0;
	if( g_bSkipSpaces )   // = ( g_dwNotExact & x_SkipSpaces )
		strcat(lps, "1" );
	if( g_bIgnoreCase )
		strcat(lps, "2" );

	return lps;
}
// LNK1104
// eof - Fc4Comp.c
