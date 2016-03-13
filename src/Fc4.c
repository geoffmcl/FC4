
// Fc4.c

#include	"Fc4.h"
#ifdef WIN32
#include <lmerr.h>   // for NETWORK errors
#endif // WIN32

#define		MXIO			256
#define		DEF_MATCH		1		// Default is -M2

void	po( LPTSTR lps );
BOOL	GetFileInfo( LPMPSTR lpmps );
LPTSTR	GetsszDT( LPMPSTR lpmps ); // like 2001-04-01 13:50:08 (19 chars)
BOOL	ValidDateTime( LPMPSTR lpmps );

#define	VDT(a)		ValidDateTime(a)

// Work structure
LPFC4WS	glpWs = 0;	// &ws

HANDLE	hStdOut  = 0;
int		gIRet    = 0;

// ABANDONED TCHAR	szDiag[1024];
TCHAR	gcLastOut;

BOOL	wf( HANDLE hf, LPTSTR lpb, DWORD len )
{
	BOOL	flg = TRUE;
	DWORD	wtn;
	if( ( VH(hf) ) &&
		( lpb ) &&
		( len ) )
	{
		if( ( !WriteFile( hf,	// handle to file to write to
				lpb,	// pointer to data to write to file
				len,		// number of bytes to write
				&wtn,	// pointer to number of bytes written
				NULL ) ) ||	// pointer to structure for overlapped I/O
			( wtn != len ) )
		{
			flg = FALSE;
		}
	}
	return flg;
}

void	PgmExit( BOOL bExit )
{
   if( VERBAL8 )
   {
      LPTSTR lpd = gszDiag;
      sprintf(lpd, "Exiting with value %d."MEOR, gIRet );
      prt(lpd);
   }

	if( VH(ghOutFil) )
	{
		wf( ghOutFil, "\r\n", sizeof("\r\n") );
		CloseHandle(ghOutFil);
	}
	//printf( MCRLF );
	// 20080909 - no extra CRLF !!! po( MCRLF );
	ghOutFil = 0;

   KillLList( &gsList1 );
   KillLList( &gsList2 );

	if( glpWs )
		LocalFree(glpWs);
	glpWs = 0;

   if(bExit)
      exit( gIRet );

}

//#define		MXIO			256

void	wof( LPTSTR lpb, DWORD len )
{
	if( VH( ghOutFil ) )
	{
		if( !wf( ghOutFil, lpb, len ) )
		{
			CloseHandle( ghOutFil );
			ghOutFil = INVALID_HANDLE_VALUE;
		}
	}
}

void	oi( LPTSTR lps )
{
	TCHAR	buf[MXIO+8];
	int		i, j, k;
	char	c, d;

//		( VH(ghOutFil) ) )
	if( ( lps ) &&
		( ( i = strlen(lps) ) > 0) )
	{
		k = 0;
		for( j = 0; j < i; j++ )
		{
			c = lps[j];
			if( c == '\n' )
			{
				if( d != '\r' )
				{
					buf[k++] = '\r';
					if( k >= MXIO )
					{
						wof( buf, k );
						k = 0;
					}
				}
			}
			buf[k++] = c;
			if( k >= MXIO )
			{
				wof( buf, k );
				k = 0;
			}
			d = c;
		}	// for length of string
		// and write the buffer
		if( k )
		{
			wof( buf, k );
			k = 0;
		}
	}
}

void	po( LPTSTR lps )
{
	DWORD	dw = strlen(lps);
	if( hStdOut == 0 )
	{
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if( !VH(hStdOut) )
		{
			gIRet = Exit_Error;
			printf( MCRLF"ERROR: Failed to get STDOUT handle!" );
			PgmExit(TRUE);
		}
	}
	if( ( VH(hStdOut) ) &&
		 ( lps && dw   ) ) {
		wf( hStdOut, lps, dw );
		gcLastOut = lps[dw-1]; // set last char out
	}
}

void	prt( LPTSTR lps )
{
	if( VH(ghOutFil) ) {
		oi(lps);
	}
	po(lps);
}



// BINARY COMPARE
// ==============
#ifdef USE_INT64_TYPE
#define  BLOCK_SIZE  1024

char _file1_buff[BLOCK_SIZE];
char _file2_buff[BLOCK_SIZE];

// this CompFilesB2() does a binary compare,
// reading in the file in BLOCK_SIZE blocks.
int	CompFilesB2( WS, LPMPSTR lpmps1, LPMPSTR lpmps2 )
{
	int		bRet = Exit_Diff;	// assume DIFFERENT
	off64_t	dws1 = 0;
   off64_t  dws2 = 0;
   DWORD    dwr1, dwr2;
   DWORD    dwread1, dwread2;
   DWORD    dwmax, dwi;
   off64_t  dwk = 0;
   char * b1 = _file1_buff;
   char * b2 = _file2_buff;
	LPTSTR	lpd = gszDiag;
   DWORD    err = 0;
   off64_t  offdone = 0;
   LARGE_INTEGER  li;
   LPMPSTR lpmps;
   PTSTR    ptmp;

	if(( lpmps1 ) &&
      ( lpmps2 ) ) {
      dws1 = lpmps1->mp_Sz;
		dws2 = lpmps2->mp_Sz;
   }
   // they are different, if either is zero length
   g_bDnHeader = FALSE;
   if( dws1 && dws2 )
   {
		if( ( g_bQuick ) &&
			( dws1 != dws2 ) )
		{
			return Exit_Diff;
		}
      // must read in a block from each file,
      // and compare each block of data
      while( dws1 && dws2 )
      {
         if( dws1 > BLOCK_SIZE )
            dwr1 = BLOCK_SIZE;
         else
            dwr1 = (DWORD)dws1;
         if( dws2 > BLOCK_SIZE )
            dwr2 = BLOCK_SIZE;
         else
            dwr2 = (DWORD)dws2;
         if(( ReadFile( ghFile1, b1, dwr1, &dwread1, NULL ) ) &&
            ( ReadFile( ghFile2, b2, dwr2, &dwread2, NULL ) ) &&
            ( dwr1 == dwread1 ) && ( dwr2 == dwread2))
         {
            if( dwr1 < dwr2 )
               dwmax = dwr1;
            else
               dwmax = dwr2;
            for(dwi = 0; dwi < dwmax; dwi++)
            {
      			if( b1[dwi] != b2[dwi] )
		      	{
				      dwk++;
				      if( g_bQuick )
					      break;
				      if( VERBAL )
				      {
					      if( !g_bDnHeader )
					      {
						      g_bDnHeader = TRUE;
                        //        "1234567890123456
						      prt( MCRLF"  Offset:         F1 F2 (all in hex)" );
					      }
                     li.QuadPart = (offdone + dwi);
					      sprintf( lpd,
						      MCRLF"%08x%08x: %02X %02X",
						      li.HighPart, li.LowPart,
						      (b1[dwi] & 0xff),
						      (b2[dwi] & 0xff) );
					      prt( lpd );
				      }
               }
            }  // for the BLOCK of binary data
		      if( dwk && g_bQuick )
               break;

            // reduce counts to do ...
            dws1 -= dwr1;
            dws2 -= dwr2;
            offdone += dwr1;  // bump the DONE count
         }
         else
         {
            // what to do when READ fails
            prt( MCRLF"ERROR: File READ failed!"MCRLF );
            err = 1;
            break;
         }
      }  // while file bytes

      if( !err ) // && (dwk == 0) )
      {
         if(( dws1 == 0 )&&( dws2 == 0 ))
         {
            // they are EXACTLY the SAME
            bRet = Exit_Same;
            if( VERBAL )
               prt( "Files are exactly the SAME!"MCRLF );
         }
         else
         {
            // FIX20080909 - fix some binary compare output
            // one or the other ran out of bytes
   		   off64_t offk = Diff( dws1, dws2 );
			   if( VERBAL )
			   {
				   if( dws1 < dws2 )
               {
                  lpmps = lpmps2;
					   dwi = 2;
               }
				   else
               {
                  lpmps = lpmps1;
					   dwi = 1;
               }
               if( dwk )
               {
                  // had some differences
                  sprintf(lpd, MCRLF"In addition to the above %llu differences listed,"MCRLF,
                     dwk );
                  prt(lpd);
                  ptmp = get_nice_number( offk, 0 );
		   		   sprintf( lpd, " some %s byte%s (%s) remain"MCRLF
                     " in File %d: %s"MCRLF,
                     ptmp,
                     ((offk > 1) ? "s" : ""),
                     get_k_num(offk,0,0),
                     dwi, lpmps->mp_Nm );
				      prt(lpd);
               }
               else
               {
                  // they were the same,
                  // ie, no difference for length of smallest
                  sprintf(lpd, "Files were the SAME for %llu bytes,"MCRLF,
                     offdone );
                  prt(lpd);
                  ptmp = get_nice_number( offk, 0 );
		   		   sprintf( lpd, " but %s byte%s (%s) remain"MCRLF
                     " in File %d: %s"MCRLF,
                     ptmp,
                     ((offk > 1) ? "s" : ""),
                     get_k_num(offk,0,0),
                     dwi, lpmps->mp_Nm );
				      prt(lpd);
               }
			   }
         }
      }
   }
   return bRet;
}

// this CompFilesB2() does a binary compare,
// using the file mapped buffer
int	CompFilesB( WS, LPMPSTR lpmps1, LPMPSTR lpmps2 )
{
	int		bRet = Exit_Same;	// assume SAME
	LPTSTR	lpb1, lpb2;
	off64_t	dws1, dws2;
	off64_t	dwi, dwj, dwk, dws;
	LPTSTR	lpd;

	dwk = dws = 0;
	lpd = gszDiag;
	if( ( lpmps1 ) &&
		( lpmps2 ) &&
		( lpb1 = lpmps1->mp_Pv ) &&
		( lpb2 = lpmps2->mp_Pv ) &&
		( dws1 = lpmps1->mp_Sz ) &&
		( dws2 = lpmps2->mp_Sz ) )
	{
		if( ( g_bQuick ) &&
			( dws1 != dws2 ) )
		{
			return Exit_Diff;
		}
		if( dws1 < dws2 )
			dwj = dws1;
		else
			dwj = dws2;
		for( dwi = 0; dwi < dwj; dwi++ )
		{
			if( lpb1[dwi] != lpb2[dwi] )
			{
				dwk++;
				if( g_bQuick )
					break;
				if( VERBAL )
				{
					if( !g_bDnHeader )
					{
						g_bDnHeader = TRUE;
						prt( MCRLF"  Offset: F1 F2 (all in hex)" );
					}
					sprintf( lpd,
						MCRLF"%08x: %02X %02X",
						dwi,
						(lpb1[dwi] & 0xff),
						(lpb2[dwi] & 0xff) );
					prt( lpd );
				}
			}
			else
			{
				dws++;
			}
		}

      dwj = Diff( dws1, dws2 );
      *lpd = 0;
		if( dwj )   
		{
         // difference in SIZE
			if( VERBAL )
			{
				if( dws1 < dws2 )
					dwi = 2;
				else
					dwi = 1;
            if( dwk )
            {
               strcpy(lpd, MCRLF"Files are DIFFERENT!"MCRLF );
				   sprintf( EndBuf(lpd),
					   "And %u byte(s) remain in File %d"MCRLF,
					   dwj,
					   dwi );
            }
            else
            {
               strcpy(lpd, MCRLF"Files are nearly the same!"MCRLF );
				   sprintf( EndBuf(lpd),
					   "But %u byte(s) remain in File %d"MCRLF,
					   dwj,
					   dwi );
            }
			}
			if( dwk == 0 )
				dwk++;   // set to return 'different'
		}
      else if(dwk)
      {
         if( VERBAL )
            strcpy(lpd, MCRLF"Files are DIFFERENT!"MCRLF );
      }
      else
      {
         // they are EXACTLY the SAME
         if( VERBAL )
            strcpy(lpd, "Files are exactly the SAME."MCRLF );
      }
      if(*lpd)
         prt(lpd);
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

#else // !USE_INT64_TYPE
int	CompFilesB( WS, LPMPSTR lpmps1, LPMPSTR lpmps2 )
{
	int		bRet = Exit_Same;	// assume SAME
	LPTSTR	lpb1, lpb2;
	DWORD	dws1, dws2;
	DWORD	dwi, dwj, dwk, dws;
	LPTSTR	lpd;

	dwk = dws = 0;
	lpd = gszDiag;
	if( ( lpmps1 ) &&
		( lpmps2 ) &&
		( lpb1 = lpmps1->mp_Pv ) &&
		( lpb2 = lpmps2->mp_Pv ) &&
		( dws1 = lpmps1->mp_Sz ) &&
		( dws2 = lpmps2->mp_Sz ) )
	{
		if( ( g_bQuick ) &&
			( dws1 != dws2 ) )
		{
			return Exit_Diff;
		}
		if( dws1 < dws2 )
			dwj = dws1;
		else
			dwj = dws2;
		for( dwi = 0; dwi < dwj; dwi++ )
		{
			if( lpb1[dwi] != lpb2[dwi] )
			{
				dwk++;
				if( g_bQuick )
					break;
				if( VERBAL )
				{
					if( !g_bDnHeader )
					{
						g_bDnHeader = TRUE;
						prt( MCRLF"  Offset: F1 F2 (all in hex)" );
					}
					sprintf( lpd,
						MCRLF"%08x: %02X %02X",
						dwi,
						(lpb1[dwi] & 0xff),
						(lpb2[dwi] & 0xff) );
					prt( lpd );
				}
			}
			else
			{
				dws++;
			}
		}
		if( ( dwj = Diff( dws1, dws2 ) ) &&
			( !g_bQuick ) )
		{
			if( dwk == 0 )
				dwk++;
			if( VERBAL )
			{
				if( dws1 < dws2 )
					dwi = 2;
				else
					dwi = 1;

				sprintf( lpd,
					MCRLF"But %u byte(s) remain in File %d",
					dwj,
					dwi );
				prt(lpd);
			}
		}
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
#endif // !USE_INT64_TYPE



#define     MINFNC      40
#define     MINNUM      9

#define  SETMINSZ(a,b)  while( strlen(a) < b ) strcat(a," ")

void Show_File_Information( WS, LPTSTR lpd )
{
#ifdef ADD_FILE_INFO
	LONG	   lRet;
#endif // ADD_FILE_INFO
   LPTSTR   lptmp;

	*lpd = 0;
	//strcpy( lpd, MCRLF );
   lptmp = EndBuf(lpd);
	sprintf( EndBuf(lpd), 
		"Comparing [%s] ",
		gszFile1 );	// &msFile1.mp_Nm[0],
   SETMINSZ(lptmp, MINFNC);
#ifdef USE_INT64_TYPE  // FIX20080819 - add some 64-bit support
   sprintf(EndBuf(lpd), "%9llu ", gi64File1 );	// msFile1.mp_Sz,
#else // !USE_INT64_TYPE
   sprintf(EndBuf(lpd), "%9u ", gdwFile1 );	// msFile1.mp_Sz,
#endif // USE_INT64_TYPE y/n
   sprintf(EndBuf(lpd), "%s"MCRLF, GetsszDT(&gmsFile1) );     // // like 2001-04-01 13:50:08 (19 chars)

   lptmp = EndBuf(lpd);
	sprintf( EndBuf(lpd), 
		"with      [%s] ",
		gszFile2 );	// &msFile2.mp_Nm[0],
   SETMINSZ(lptmp, MINFNC);
#ifdef USE_INT64_TYPE  // FIX20080819 - add some 64-bit support
   sprintf(EndBuf(lpd), "%9llu ", gi64File2 );	// msFile2.mp_Sz
#else // !USE_INT64_TYPE
   sprintf(EndBuf(lpd), "%9u ", gdwFile2 );	// msFile2.mp_Sz
#endif // USE_INT64_TYPE y/n

#ifdef ADD_FILE_INFO
	if( ( VDT(&gmsFile1) ) &&
		( VDT(&gmsFile2) ) )
	{
		lRet = CompareFileTime( &gmsFile1.mp_sFI.ftLastWriteTime,
			&gmsFile2.mp_sFI.ftLastWriteTime );
		// pointer to first file time
		// pointer to second file time
// -1 First file time is less than second file time. 
//  0 First file time is equal to second file time. 
// +1 First file time is greater than second file time. 
		if( lRet == 0 )
		{
			strcat( lpd, "Same Date/Time!" );
		}
		else
		{
			strcat( lpd, GetsszDT(&gmsFile2) );
			if( lRet < 0 )
				strcat( lpd, " Later" );
			else
				strcat( lpd, " Earlier" );
		}
	}
	else
	{
		strcat( lpd, GetsszDT(&gmsFile2) );
		strcat( lpd, " ..." );
	}
#endif // ADD_FILE_INFO

	strcat( lpd, MCRLF );
	prt(lpd);

   *lpd = 0;

}

void Process_1_File( WS )
{
	LPTSTR	lpd = gszDiag; // big buffer
   INT      i;
	if( VERBAL8 )
	{
		sprintf( lpd,
			MCRLF"Openning file [%s] ...",
			gszFile1 );
		prt(lpd);
      *lpd = 0;
	}

	//GetFileInfo( &msFile1 );	// one option
	ghFile1 = OpenForRead( gszFile1 );
	GetFileInfo( &pWs->ws_msFile1 );
	if( VERBAL8 )
	{
		if( VH(ghFile1) )
		{
			sprintf( lpd,
				" Handle = %d",
				ghFile1 );
		}
		else
		{
			strcpy(lpd, " FAILED!" );
		}
		prt(lpd);
		sprintf( lpd,
			MCRLF"Openning file [%s] ...",
			gszFile2 );
		prt(lpd);
      *lpd = 0;
	}

	//pWs->ws_msFile2.mp_Hf = OpenForRead( gszFile2 );
	ghFile2 = OpenForRead( gszFile2 );
   if( !VH(ghFile2) )
   {
      // try harder to OPEN this file
      i = HasDot( gszFile2 ); // allow a FOLDER\., then add same file name
      if(!i)
         i = HasAst( gszFile2 );
      if(i)   
      {
         // as per gszFile1! And try that
         // FIX20060625 - messed up file name
         // only if successful - strcpy( &gszFile2[i-1], RetFileTitle(gszFile1) );
      	//ghFile2 = OpenForRead( gszFile2 );
         strcpy( gszDiag, gszFile2 ); // make a COPY
         strcpy( &gszDiag[i-1], RetFileTitle(gszFile1) );
        	ghFile2 = OpenForRead( gszDiag );
         if( VH(ghFile2) ) {
            strcpy( gszFile2, gszDiag );
         }
      }
      else
      {
         // what else to try

      }
   }

	GetFileInfo( &pWs->ws_msFile2 );
	if( VERBAL8 )
	{
//		if( VH(pWs->ws_msFile2.mp_Hf) )
		if( VH(ghFile2) )
		{
			sprintf( lpd,
				" Handle = %d",
				ghFile2 );	// pWs->ws_msFile2.mp_Hf );
		}
		else
		{
			strcpy(lpd, " FAILED!" );
		}
		prt(lpd);
      *lpd = 0;
	}
	if(( VH(ghFile1) ) &&
		( VH(ghFile2) ) )
	{

		if( VERBAL8 )
			prt( MCRLF"Get file sizes ..." );

//		if( ( msFile1.mp_Sz = RetFileSize( ghFile1 ) ) &&
//			( msFile2.mp_Sz = RetFileSize( ghFile2 ) ) )
		gi64File1 = RetFileSize( ghFile1 );
		gi64File2 = RetFileSize( ghFile2 );
		if(( gi64File1 ) &&
			( gi64File2 ) )
		{
			if( VERBAL8 )
				prt( " Mapping files ..." );

			if(( GetMapFile( &gmsFile1 ) ) &&
				( GetMapFile( &gmsFile2 ) ) )
			{
				if( VERBAL2 )
				{
					*lpd = 0;
               Show_File_Information( pWs, lpd );
				}
				if( g_bBinary )
				{
					gIRet = CompFilesB( pWs, &gmsFile1, &gmsFile2 );
				}
				else
				{
					gIRet = CompFiles( pWs, &gmsFile1, &gmsFile2 );
				}

				if( VERBAL && g_bBinary )
				{
					if( gIRet )
					{
						//prt( MCRLF"Exit - Files are different!"MCRLF );
						strcpy( lpd, MCRLF"Exit - Files are different!" );
					}
					else
					{
						//prt( MCRLF"Exit - Files are exactly the same."MCRLF );
						strcpy( lpd, MCRLF"Exit - Files are exactly the same." );
					}
               strcat(lpd,MCRLF);   //FIX20081222
				}
         } else {
#ifdef USE_INT64_TYPE  // FIX20080819 - add some 64-bit support
            if( g_bBinary )
            {
               if( VERBAL9 )
               {
                  strcpy( lpd, MCRLF"WARNING: Unable to MAP one or both files!"MCRLF );
                  if( gszLastErr[0] ) {
                     sprintf(EndBuf(lpd), "ERROR Indication: %s (%d)"MCRLF, gszLastErr, gdwLastError );
                     gszLastErr[0] = 0;
                     gdwLastError = 0;
                  }
                  strcat(lpd, "But proceeding with BINARY compare ..."MEOR );
                  prt(lpd);
                  *lpd = 0;
               }
				   if( VERBAL2 )
				   {
					   *lpd = 0;
                  Show_File_Information( pWs, lpd );
				   }
					gIRet = CompFilesB2( pWs, &gmsFile1, &gmsFile2 );
				   if( VERBAL )
				   {
					   if( gIRet )
					   {
						   //prt( MCRLF"Exit - Files are different!"MCRLF );
						   strcpy( lpd, MCRLF"Exit - Files are different!" );
					   }
					   else
					   {
						   //prt( MCRLF"Exit - Files are exactly the same."MCRLF );
						   strcpy( lpd, MCRLF"Exit - Files are exactly the same." );
					   }
                  strcat(lpd,MCRLF);   //FIX20081222
				   }
            }
            else
#endif // USE_INT64_TYPE
            {
               strcpy( lpd, MCRLF"ERROR: Exit - Unable to MAP one or both files!"MCRLF
                  "Try -b for binary compare."MCRLF );
               if( gszLastErr[0] ) {
                  sprintf(EndBuf(lpd), "ERROR Indication: %s (%d)"MCRLF, gszLastErr, gdwLastError );
                  gszLastErr[0] = 0;
                  gdwLastError = 0;
               }
               prt(lpd);
            }
         }
      } else {
         // one or the other is ZERO size
			strcpy( lpd, MCRLF"Exit - One or both files are empty!"MCRLF );
         prt(lpd);
      }

		// Clean up any MAPPING
		KillMapFile( &gmsFile1 );
		KillMapFile( &gmsFile2 );
		// ====================

	}
	else
	{
//		if( VERBAL )
//		{
			LPTSTR	lpf;
			if( VH(ghFile1) )
			{
				lpf = gszFile2;		// .mp_Nm[0];
			}
			else
			{
				lpf = gszFile1;	// &msFile1.mp_Nm[0];
			}
			sprintf( lpd,
				MCRLF"ERROR: Unable to open [%s] file!",
				lpf );
			prt( lpd );
			gIRet = Exit_Error;
//		}
	}

	if( VH(ghFile1) )
		CloseHandle(ghFile1);
	if( VH(ghFile2) )
		CloseHandle(ghFile2);
   ghFile1 = 0;
   ghFile2 = 0;
}

void Process_Files( WS )
{
	LPTSTR	lpd = gszDiag; // big buffer
   INT      i;
   PLE   ph1 = &gsList1;
   PLE   ph2 = &gsList2;
   PLE   pn1, pn2;
   PFL   pf1, pf2;
   PTSTR pb1, pb2, pb3;
   INT   icnt1, icnt2;
   INT   order = 0;

   // FIX20060910 - handle simple case of 2 files to compare
   ListCount2(ph1, &icnt1);
   ListCount2(ph2, &icnt2);
   if((icnt1 == 1) && (icnt1 == icnt2)) {
      order = 1;
   }
   pb3 = MALLOC(264);
   CHECKMEM(pb3);
   pf1 = 0;
   pf2 = 0;
   Traverse_List( ph1, pn1 )
   {
      pf1 = (PFL)pn1;
      pb1 = RetFileTitle( pf1->name );
      strcpy(pb3,pb1);
      i = 0;
      Traverse_List( ph2, pn2 )
      {
         pf2 = (PFL)pn2;
         pb2 = RetFileTitle( pf2->name );
         if( order > 0 ) {
            // we just have 1 and 1 ...
            i = 1;
            break;
         } else {
            if( STRCMPI(pb3,pb2) == 0 ) {
               i = 1;
               break;
            }
         }
      }
      if(( i > 0 )&&(pf1)&&(pf2)) {
         strcpy( gszFile1, pf1->name );
         strcpy( gszFile2, pf2->name );
         Process_1_File( pWs );
      } else {
         if(VERBAL) {
            if(pf1) {
               sprintf(lpd,"No match found for [%s] file ..."MEOR, pf1->name);
            } else {
               strcpy(lpd, "No files to process ..."MEOR);
            }
            prt(lpd);
         }
      }
   }
   MFREE(pb3);
}

char *
SetLastErrorText( DWORD dwLastError )
{
   LPSTR eb = gszLastErr;
#ifdef WIN32
   HMODULE hModule = NULL; // default to system source
   LPSTR MessageBuffer = NULL;
   DWORD dwBufferLength = 0;
   DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_FROM_SYSTEM ;

   //
   // If dwLastError is in the network range, 
   //  load the message source.
   //
   *eb = 0;
   if((dwLastError >= NERR_BASE) && (dwLastError <= MAX_NERR)) {
        hModule = LoadLibraryEx(
            TEXT("netmsg.dll"),
            NULL,
            LOAD_LIBRARY_AS_DATAFILE
            );

        if(hModule != NULL)
            dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
   }

   // Call FormatMessage() to allow for message 
   //  text to be acquired from the system 
   //  or from the supplied module handle.
   dwBufferLength = FormatMessageA(
        dwFormatFlags,
        hModule, // module to get message from (NULL == system)
        dwLastError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
        (LPSTR) &MessageBuffer,
        0,
        NULL
        );
   if( dwBufferLength && MessageBuffer )
   {
      strcpy( eb, MessageBuffer );
      // UGH: Why do they ADD Cr/Lf to message
      while( dwBufferLength-- )
      {
         if(eb[dwBufferLength] > ' ')
            break;
         eb[dwBufferLength] = 0;
      }
   }
   if( MessageBuffer )
   {
      // Free the buffer allocated by the system.
      LocalFree(MessageBuffer);
   }

   // If we loaded a message source, unload it.
   if(hModule != NULL)
      FreeLibrary(hModule);
#else
   // TODO: File error
   *eb = 0;
   strcpy(eb,"File Error");
#endif
   return eb;
}

// **************************************************
// BOOL	GetMapFile( LPMPSTR lpmps )
//
// ==================================================
BOOL	GetMapFile( LPMPSTR lpmps )
{
	BOOL	flg = FALSE;
#ifdef WIN32
	HANDLE	hf = 0;
	HANDLE	hmv;
	LPVOID	pmv;

   if( lpmps )
      hf = lpmps->mp_Hf;

   if( VH( hf ) ) {
      hmv = CreateFileMapping( hf, NULL, PAGE_READONLY, 0, 0, NULL );
      if(hmv) {
         pmv = MapViewOfFile( hmv, FILE_MAP_READ, 0, 0, 0 );
         if(pmv) {
            // Ok. got MAPPED FILE
            lpmps->mp_Hd = hmv;
            lpmps->mp_Pv = pmv;
            flg = TRUE;
         } else {
            gdwLastError = GetLastError();
            SetLastErrorText( gdwLastError );
            CloseHandle(hmv);
         }
      } else {
         gdwLastError = GetLastError();
         SetLastErrorText( gdwLastError );
      }
	}
#else
    // TODO: memory map file
#endif
	return flg;
}

BOOL	KillMapFile( LPMPSTR lpmps )
{
	BOOL	flg = FALSE;
#ifdef WIN32
	HANDLE		hmv;
	LPVOID		pmv;
	if( lpmps )
	{
		pmv = lpmps->mp_Pv;
		if( pmv )
		{
			UnmapViewOfFile( pmv );
			flg = TRUE;
		}
		hmv = lpmps->mp_Hd;
		if( VH(hmv) )
		{
			CloseHandle( hmv );
			flg = TRUE;
		}
		lpmps->mp_Pv = 0;
		lpmps->mp_Hd = 0;
	}
#else
    // TODO: Unmap file
#endif
	return flg;
}

// =============================================================
//typedef struct _BY_HANDLE_FILE_INFORMATION { // bhfi 
//    DWORD    dwFileAttributes; 
//    FILETIME ftCreationTime; 
//    FILETIME ftLastAccessTime; 
//    FILETIME ftLastWriteTime; 
//    DWORD    dwVolumeSerialNumber; 
//    DWORD    nFileSizeHigh; 
//    DWORD    nFileSizeLow; 
//    DWORD    nNumberOfLinks; 
//    DWORD    nFileIndexHigh; 
//    DWORD    nFileIndexLow; 
//} BY_HANDLE_FILE_INFORMATION; 
BOOL	GetFileInfo( LPMPSTR lpmps )
{
	BOOL	flg = FALSE;
#ifdef ADD_FILE_INFO
	HANDLE	hFile;
	if( ( lpmps ) &&
		( hFile = lpmps->mp_Hf ) &&
		( VH(hFile) ) )
	{
		flg = GetFileInformationByHandle( hFile,	// handle of file 
			&lpmps->mp_sFI );	// pointer to structure
		lpmps->mp_bGFI = flg;
	}
#endif // ADD_FILE_INFO
	return flg;
}

// **************************************************

#ifdef WIN32
//typedef struct  _SYSTEMTIME
//    {
//    WORD wYear;
//    WORD wMonth;
//    WORD wDayOfWeek;
//    WORD wDay;
//    WORD wHour;
//    WORD wMinute;
//    WORD wSecond;
//    WORD wMilliseconds;
//    }   SYSTEMTIME;
static	SYSTEMTIME		sst;

void	AddSysDate( LPTSTR lps, LPSYSTEMTIME lpsst )
{
	if( ( lps ) &&
		( lpsst ) )
	{
      //                   1234567890
		sprintf( lps,    // 2001-04-01
			"%04d-%02d-%02d",
			(lpsst->wYear & 0xffff),
			(lpsst->wMonth & 0xffff),
			(lpsst->wDay & 0xffff) );

	}
}
#else
void	AddSysDate( LPTSTR lps, time_t *ptt )
{
    if (ptt && *ptt) {
        struct tm *pt = localtime(ptt);
        if (pt) {
            sprintf(lps,
                "%02d/%02d/%04d",
                pt->tm_mday,
                pt->tm_mon,
                pt->tm_year);
        }
    }
}

#endif
LPTSTR	GetsszDate( void )
{
	static TCHAR sszDate[32];
	LPTSTR lps = &sszDate[0];
#ifdef WIN32
	GetSystemTime( &sst );
	AddSysDate( lps, &sst );   // put like "2001-04-01" - 10 chars
#else
    time_t tm = time(0);
	AddSysDate( lps, &tm );   // put like "2001-04-01" - 10 chars
#endif
	return lps;
}

#ifdef WIN32
void	AddSysTime( LPTSTR lps, LPSYSTEMTIME lpsst )
{
	if( ( lps ) &&
		( lpsst ) )
	{
		sprintf( lps,                //      12345678
			"%2d:%02d:%02d",           // like 13:50:08
			(lpsst->wHour & 0xffff),
			(lpsst->wMinute & 0xffff),
			(lpsst->wSecond & 0xffff) );
	}
}
#endif

void	AddSysTime2( LPTSTR lps, time_t *ptt )
{
    if (ptt && *ptt) {
        struct tm *pt = localtime(ptt);
        if (pt) {
            sprintf(lps,
                "%02d:%02d:%02d",
                pt->tm_hour,
                pt->tm_min,
                pt->tm_sec);
        }
    }

}

LPTSTR	GetsszTime( void )
{
	static TCHAR sszTime[32];
	LPTSTR lps = &sszTime[0];
#ifdef WIN32
	GetSystemTime( &sst );
	AddSysTime( lps, &sst );   // add 8 like "13:50:08"
#else
    time_t tm = time(0);
	AddSysTime2( lps, &tm );   // put like "12:34:56" - 8 chars
#endif
	return lps;
}

//FILETIME
//The FILETIME structure is a 64-bit value
// representing the number of 100-nanosecond intervals
// since January 1, 1601. 
//typedef struct _FILETIME { // ft 
//    DWORD dwLowDateTime; 
//    DWORD dwHighDateTime; 
//} FILETIME; 

BOOL	ValidDateTime( LPMPSTR lpmps )
{
	BOOL	flg = FALSE;
#ifdef ADD_FILE_INFO
	SYSTEMTIME	st;
	if( ( lpmps ) &&
		( lpmps->mp_bGFI ) &&
		( lpmps->mp_sFI.ftLastWriteTime.dwLowDateTime | lpmps->mp_sFI.ftLastWriteTime.dwHighDateTime ) &&
		( FileTimeToSystemTime( &lpmps->mp_sFI.ftLastWriteTime, &st ) ) )
	{
		flg = TRUE;
	}
#endif // ADD_FILE_INFO
	return flg;
}


LPTSTR	GetsszDT( LPMPSTR lpmps )
{
	static TCHAR sszDT[32];
	LPTSTR	lps = sszDT;
#ifdef ADD_FILE_INFO
	SYSTEMTIME	st;
#endif // ADD_FILE_INFO

#ifdef ADD_FILE_INFO
	if( ( VDT( lpmps ) ) &&
		( FileTimeToSystemTime( &lpmps->mp_sFI.ftLastWriteTime, &st ) ) )
	{
		AddSysDate( lps, &st );       // put like "2001-04-01" - 10 chars
		strcat( lps, " " );          // add space between
		AddSysTime( EndBuf(lps), &st );  // add 8 like "13:50:08"
      // total 19 characters
	}
	else
#endif // ADD_FILE_INFO
	{
		strcpy( lps, "<No DT!>" );
	}
	return lps;
}

// ***********************************************************


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : Init
// Return type: void 
// Argument   : WS
// Description: 
// Done only once at initialisation
// See PgmExit(BOOL) for done at end.
///////////////////////////////////////////////////////////////////////////////
void	Init( WS )
{
	g_bBinary = FALSE;
	g_iVerbal = 1;
	g_bQuick  = FALSE;
	gszFile1[0] = 0;
	gszFile2[0] = 0;
	NULLPMPSTR( &gmsFile1 );	//           memset(a,0,sizeof(MPSTR))
	NULLPMPSTR( &gmsFile2 );	//           memset(a,0,sizeof(MPSTR))
	// CmdLen = 0;
//	g_bInCmd = TRUE;
	ghOutFil = 0;
	szOut[0] = 0;

	g_bDiag1 = TRUE;

	g_dwMatchCnt = DEF_MATCH;

   InitLList( &gsList1 );
   InitLList( &gsList2 );
}



// eof - Fc4.c
