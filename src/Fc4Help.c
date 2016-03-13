
// Fc4Help.c
#include	"Fc4.h"

TCHAR	szCmd[MXCMDBUF];
int		CmdLen = 0;
TCHAR	szOut[MAX_PATH];

//	db	LOOSE6_COMP	; 6	   - Skip large BLOCK outputs
TCHAR HD_DATE[] = __DATE__;
TCHAR HD_TIME[] = __TIME__;

TCHAR	HD_MSG[] =
	"Informatique Rouge - FILE COMPARE UTILITY - "
	VERS;

TCHAR	HLP_MSG[] = MCRLF
	"Usage:      FC4  [/Switches]  FileName1  FileName2  [@InputFile]"MCRLF
	"Switches:   Must be preceeded by / or -, space separated, case insensitive."MCRLF
	" -? (or -H) = This simple help information."MCRLF
	" -B         = Simple Binary compare."MCRLF
#ifndef  NDEBUG   // ONLY IF DEBUG VERSION
	" -Dn        = Debug Bits (1=DBG1, 2=DBG2, 4=DBG3, -1=ALL) =g_dwDbgBits."MCRLF
	" -D:line    = Debug Line Number.(=g_dwDbgLine)"MCRLF
#endif   // !NDEBUG
#ifdef	ADDLOPT
	" -L[nnnn]   = Maximum lines of difference shown (Def=-L=No max.)"MCRLF
#endif	// ADDLOPT
	" -Mnnn      = Match nnn lines before output. Default=2"MCRLF
	" -N         = Line Numbers ON."MCRLF
	" -OOutFile  = To write output to a file."MCRLF
#ifdef	ADDPREV
	" -P[n]      = Previous n lines added to output. Only 1-3 allowed."MCRLF
#endif	// ADDPREV
	" -Q         = Quick exit, setting Errorlevel 1, on FIRST difference."MCRLF
	" -R         = Reverse compare 2 with 1."MCRLF
	" -V[n]      = Verbose Information n = 0:Silent 1:Default 8:Lots 9:Diags."MCRLF
   " -X[1|2|?]  = NOT Exact (1:Ignore Space, 2:Ignore CaSe, ?:Help)"MCRLF
	" Notes:"MCRLF
	" -> Errorlevel 1 if Different; 2 if ERROR and 3 if HELP shown, else 0."MCRLF
	" -> If used, the InputFile must contain line separated names or switches."MCRLF
	" -> Quick (-Q) forces -V0 ie zero verbosity and no output file."MCRLF
//	" -> Compare methods are 1=Ignore space 2=Ignore CaSe"MCRLF
//	"    3=Ignore Trailing Spaces 4=Trail COMMENTS 5=4 for Interline"MCRLF
//	"    6=Display [5+ Msg 5+] lines in place of BIG Difference"MCRLF
//	"    7=Suppress space (<=20H) differences; 8 = Ignore CASE"MCRLF
	"                                                "
	"  Bonne comparison...";

//	" -> Compare methods are 1=Ignore 1-Byte 2=Ignore COMMENTS Only"MCRLF
//	"    3=Ignore Trailing Spaces 4=Trail COMMENTS 5=4 for Interline"MCRLF
//	"    6=Display [5+ Msg 5+] lines in place of BIG Difference"MCRLF
//	"    7=Suppress space (<=20H) differences; 8 = Ignore CASE"MCRLF

TCHAR	HLPX_MSG[] = MCRLF
	"Usage:      FC4  [/Switches]  FileName1  FileName2  [@InputFile]"MCRLF
	"Switches:   Must be preceeded by / or -, space separated, case insensitive."MCRLF
   "HELP ON NOT EXACT -X SWITCH"MCRLF
   "            -X1 = Ignore space comparison."MCRLF
   "            -X2 = Ignore CaSe for comparision."MCRLF
	"                                                "
	"  Happy comparisons.";

TCHAR	szModule[] = "Dummy";


void  Show_CMD_Line( void )
{
  	LPTSTR	lpd = gszDiag; // big buffer
	if( VERBAL2 )
	{
		prt( HD_MSG );
      prt( MCRLF );  // FIX20081222
	}
	if( VERBAL9 )
	{
		strcpy( lpd, "Cmd:[" );
		strcat( lpd, szCmd );
		strcat( lpd, "]"MCRLF );
		prt( lpd );
      if( szCmd[0] == 0 )
         prt( "ERROR: NO COMMANDS GIVEN!"MCRLF );
	}
}



void	GenError( WS )
{
	prt( "ERROR: Invalid command line."MCRLF );
	gIRet = Exit_Error;
	PgmExit(TRUE);
}

void prt_help_message(void)
{
   // added FIX20060709
   prt( "Compiled: " );
	prt( HD_DATE );
   prt( " at " );
	prt( HD_TIME );
   prt( MCRLF );
   // =================
	prt( HD_MSG );
	prt( HLP_MSG );
}

void	Use( WS )
{
   prt_help_message();
	gIRet = Exit_Help;		// 3;
	PgmExit(TRUE);
}

void	UseX( void )
{
	prt( HD_MSG );
	prt( HLPX_MSG );
	gIRet = Exit_Help;		// 3;
	PgmExit(TRUE);
}

BOOL	IsNum( TCHAR c )
{
	BOOL	flg = FALSE;
	if( ( c >= '0' ) &&
		( c <= '9' ) )
	{
		flg = TRUE;
	}
	return flg;
}

BOOL	IsHex( TCHAR c )
{
	BOOL	flg = FALSE;
	if( ( c >= 'A' ) &&
		( c <= 'F' ) )
	{
		flg = TRUE;
	}
	return flg;
}

DWORD	GetHex( LPTSTR lps )
{
	DWORD	iNum = 0;
	DWORD		i, j, k, neg;
	char		c;

	iNum = 0;
	k = 0;
	neg = 0;
	if( ( lps ) &&
		( j = strlen(lps) ) )
	{
		for( i = 0; i < j; i++ )
		{
			c = toupper( lps[i] );
			if( IsNum(c) )
			{
				if( k == 0 )
				{
					iNum = (c - '0');
				}
				else
				{
					iNum = (iNum * 16) + (c - '0');
				}
				k++;
			}
			else if( IsHex(c) )
			{
				if( k == 0 )
				{
					iNum = (c - 'A' + 10);
				}
				else
				{
					iNum = (iNum * 16) + (c - 'A' + 10);
				}
				k++;
			}
			else
			{
				if( k )
					break;
			}
		}
	}
	return iNum;
}

void	FixBit( WS, LPTSTR lpb )
{
	LPTSTR	lps;
	DWORD	iNum, iBit;
	if( ( lps = lpb ) != 0 )
	{
		while( ( *lps ) && ( !IsNum(*lps) ) )
		{
			lps++;
		}
		iNum = 0;
		while( ( *lps ) && ( IsNum(*lps) ) )
		{
			iNum = ( iNum * 10 ) + ( *lps - '0' );
			lps++;
		}
		if( ( iNum ) && ( iNum < 32 ) && ( *lps == '=' ) )
		{
			lps++;
			if( IsNum( *lps ) )
			{
				iBit = 1;
				iNum--;
				iBit = (iBit << iNum);
				if( *lps == '0' )
				{
					g_dwDbgBits &= ~( iBit );
				}
				else if( *lps == '1' )
				{
					g_dwDbgBits |= iBit;
				}
			}
		}
	}
}

// DEBUG ONLY
#ifndef  NDEBUG
DWORD	GetNum2( WS, LPTSTR lps )
{
	DWORD		iNum;
	DWORD		i, j, k, neg;
	char		c, d;

	iNum = 0;
	k = 0;
	neg = 0;
	if( ( lps              ) &&
		 ( j = strlen(lps) ) )
	{
		for( i = 0; i < j; i++ )
		{
			c = lps[i];
			if( IsNum(c) )
			{
				if( k == 0 )
				{
					iNum = (c - '0');
				}
				else
				{
					iNum = (iNum * 10) + (c - '0');
				}
				k++;
			}
			else
			{
				if( k )
				{
					// if started number, then this is the END
					// EXCEPT
					if( ( k == 1 ) &&
						( iNum == 0 ) &&
						( ( c == 'x' ) || ( c == 'X' ) ) )
					{
						iNum = GetHex( &lps[ (i + 1) ] );
					}
					else
					{
						break;
					}
				}
				else
				{
					// no number started
					d = toupper(c);
					if( d == '-' )
					{
						neg = 1;
					}
					else if( ( d == 'B' ) &&
						( j >= 6 ) &&	// got say BIT6=1
						( (i + 5) < j ) &&
						( toupper( lps[i+1] ) == 'I' ) &&
						( toupper( lps[i+2] ) == 'T' ) )
					{
						// ok, enough
						FixBit( pWs, &lps[i] );
						return 0;
					}
				}
			}
		}
	}
	if( neg )
	{
      if(iNum)
         iNum--;  // back up one
		iNum = ~( iNum ); // and get negative
	}
	return iNum;
}

#endif   // ifndef  NDEBUG

int	GetNum( LPTSTR lps )
{
	int		iNum;
	int		i, j, k;
	char	c;
	iNum = 0;
	k = 0;
	if( ( lps ) &&
		( j = strlen(lps) ) )
	{
		for( i = 0; i < j; i++ )
		{
			c = lps[i];
			if( IsNum(c) )
			{
				if( k == 0 )
				{
					iNum = (c - '0');
				}
				else
				{
					iNum = (iNum * 10) + (c - '0');
				}
				k++;
			}
			else
			{
				if( k )
					break;
			}
		}
	}
	return iNum;
}

/* ===================================================
   BOOL	GetX( WS, LPTSTR lps )
   
   PURPOSE: Get the NOT EXACT request
      Set g_dwNotExact with bit flags

   Presently 2 macros to check flags
   g_bSkipSpace  = x_SkipSpace
   g_bIgnoreCase = x_IgnoreCase

   =================================================== */
BOOL	GetX( WS, LPTSTR lps )
{
	BOOL	flg = TRUE;
	DWORD	i, j;
	TCHAR	tc;

   j = 0;
   if( lps )
      j = strlen(lps);
	if(j)
	{
		flg = FALSE;
		for( i = 0; i < j; i++ )
		{
			tc = lps[i];
			switch( tc )
			{
			case '1':
				g_dwNotExact |= x_SkipSpaces;
				break;

			case '2':
				g_dwNotExact |= x_IgnoreCase;
				break;

         case '?':
            // help with NOT EXACT
            UseX();  // actually NEVER returns
				//giRet = Exit_Help;
				//PgmExit(TRUE);
            break;

			default:
				flg = TRUE; // set ERROR
				break;

			}
			if( flg )
				break;
		}
	}
	return flg;
}

HANDLE	OpenForRead( LPTSTR lpf )
{
	HANDLE	hFile;
    int i = is_file_or_directory(lpf);
    if (i != 1)
        return 0;
#ifdef WIN32
	hFile = CreateFile( lpf,	// pointer to name of the file
		GENERIC_READ,			// access (read-write) mode
		0,						// share mode
		NULL,					// pointer to security attributes
		OPEN_EXISTING,			// how to create
		FILE_ATTRIBUTE_NORMAL,	// file attributes
		NULL );					// handle to file with attributes to 
                               // copy
#else
    hFile = fopen(lpf,"r");
#endif
	return hFile;
}

#if 0   // 0000000000000000000000000000000000000000000
// FIX20080819 - add some 64-bit support
#ifdef USE_INT64_TYPE  // FIX20080819 - add some 64-bit support
off64_t	RetFileSize( HANDLE hFile )
{
	off64_t	dwRet = 0;
	if( VH( hFile ) )
	{
      LARGE_INTEGER li;
		li.LowPart = GetFileSize( hFile, &li.HighPart );
		dwRet = li.QuadPart;
	}
	return dwRet;
}
#else // !USE_INT64_TYPE
DWORD	RetFileSize( HANDLE hFile )
{
	DWORD	dwRet = 0;
	if( VH( hFile ) )
	{
      LARGE_INTEGER li;
		li.LowPart = GetFileSize( hFile, &li.HighPart );
		dwRet = li.LowPart;
	}
	return dwRet;
}
#endif // USE_INT64_TYPE y/n
#endif // 0000000000000000000000000000000000000000000000

#define MY_MAX_INPUT    0xffff
BOOL	GetInputFile( WS, LPTSTR lpf )
{
	BOOL	bRet = TRUE;	// begin with error
	HANDLE	hFile;
	__int64	dwSize;
	DWORD	dwAlloc;
	LPSTR	lpb;
    int     i = is_file_or_directory(lpf);
    if (i != 1)
        return TRUE;    // FAILED
    dwSize = get_last_file_size();
    if (dwSize > MY_MAX_INPUT)
        return TRUE;    // TOO BIG FOR INPUT FILE!
	hFile = OpenForRead( lpf );
	if( VH( hFile ) )
	{
		{
			dwAlloc = (DWORD)( ( dwSize + 1 ) + ( MXARGS * sizeof(LPVOID) ) );
			lpb = MALLOC( dwAlloc );
            if (lpb)
			{
                DWORD dwSize2, dwSize1 = (DWORD)dwSize;
#ifdef WIN32
				if( ( ReadFile( hFile,	// handle of file to read
					lpb,	// pointer to buffer that receives data
					dwSize1,	// number of bytes to read
					&dwSize2,	// pointer to number of bytes read
					NULL ) ) &&
					( dwSize1 == dwSize2 ) )
#else
                if ( (dwSize2 = fread(lpb,1,dwSize1,hFile)) == dwSize1 )
#endif
				{

					LPSTR *	lpargv;
					LPSTR	lpcmd;
					int		argc, k, j, l;
					char	c;

					lpargv = (LPSTR *) (LPSTR)(lpb + dwSize1 + 1);
					argc = 0;
					lpargv[argc++] = &szModule[0];
					k = 0;
					for( dwSize2 = 0; dwSize2 < dwSize1; dwSize2++ )
					{
						c = lpb[dwSize2];
						if( c == ';' )
						{
							// Skip comments in file
Skip:
							dwSize2++;
							for( ; dwSize2 < dwSize1; dwSize2++ )
							{
								c = lpb[dwSize2];
								if( c == '\n' )
								{
									//dwSize2++;
									break;
								}
							}
						}
						else if( c > ' ' )
						{
							j = k;
							lpcmd = &lpb[k];
							lpb[k++] = c;
							dwSize2++;
							for( ; dwSize2 < dwSize1; dwSize2++ )
							{
								c = lpb[dwSize2];
								if( ( c == ';' ) ||
									( c < ' ' ) )
								{
									break;
								}
								else
								{
									lpb[k++] = c;
								}
							}
							l = k - 1;
							while( ( l > j ) &&
								( lpb[l] <= ' ' ) )
							{
								l--;
							}
							l++;
							lpb[l] = 0;
							k = l + 1;
							//lpargv[argc++] = &lpb[j];
							lpargv[argc++] = lpcmd;
							if( argc >= MXARGS )
							{
								prt( MCRLF"ERROR: Input file contains too many arguments!" );
								gIRet = Exit_Error;
								PgmExit(TRUE);
							}
							if( c == ';' )
								goto Skip;
						}
					}
					// processed all the file
					if( argc > 1 )
					{
						ProcessCommand( pWs, argc, lpargv );
						bRet = FALSE;
					}
					else
					{
						MFREE(lpb);
#ifdef WIN32
						CloseHandle(hFile);
#else
                        fclose(hFile);
#endif
						lpb = 0;
						hFile = 0;
						prt( "ERROR: Input file error!"MCRLF );
						gIRet = Exit_Error;
						PgmExit(TRUE);
					}
				}
				if( lpb )
					MFREE(lpb);
			}
		}

		if( hFile && (hFile != INVALID_HANDLE_VALUE) ) {
#ifdef WIN32
			CloseHandle( hFile );
#else
            fclose(hFile);
#endif
        }
	}

	return bRet;

}

int ProcessCommand( WS, int argc, char * argv[] )
{
	int		i, j, k;
	char *	cp;
	char	c;
	LPTSTR	lpd, lps;
	DWORD	dwi;

	//if( argc < 3 )
	//	Use( pWs );
	j = 0;
	lpd = gszDiag;
    // FIX20061111 - preparse, just for VERBOSITY (-v?) command
	for( i = 1; i < argc; i++ )
	{
        cp = argv[i];
        c = *cp;
        if ( ( c == '/' ) || ( c == '-' ) ) {
            lps = cp + 1;
			c = toupper( *lps );
            if( c == 'V' ) {
                lps++;
                if( *lps ) {
                    g_iVerbal = GetNum(lps);
                }
            }
        }
    }
	for( i = 1; i < argc; i++ )
	{
		if( ( cp = argv[i] ) &&
			( c = *cp ) &&
			( k = strlen(cp) ) )
		{
			if( ( CmdLen + k + 1 ) < MXCMDBUF )
			{
				if( CmdLen )
					strcat( szCmd, " " );
				strcat( szCmd, cp );
				CmdLen = strlen( szCmd );
			}
			if( c == '"' )
			{
				cp++;
				strcpy( lpd, cp );
				if( ( k = strlen(lpd) ) &&
					( k > 1 ) )
				{
					k--;
					while( k )
					{
						if( lpd[k] == '"' )
						{
							lpd[k] = 0;
							break;
						}
						k--;
					}
				}
				else
				{
					GenError( pWs );
				}
			}
			else
			{
				strcpy( lpd, cp );
			}
			if( ( k = strlen(lpd) ) &&
				( c = *lpd ) )
			{
				switch( c )
				{
				case '/':
				case '-':
					{
						// a switch
						lps = lpd + 1;
						c = toupper( *lps );
						switch( c )
						{

						case '?':
						case 'H':
							Use( pWs );
							gIRet = Exit_Help;
							PgmExit(TRUE);
							break;

						case 'B':
							g_bBinary = TRUE;
							break;

#ifndef  NDEBUG
						case 'D':
							lps++;
							// accept multiple -D1 -D2 etc.
							if( *lps == ':' )
							{
								lps++;
								dwi = GetNum(lps);
								g_dwDbgLine = dwi;
							}
							else
							{
								g_dwDbgBits |= GetNum2( pWs, lps );
							}
							break;
#endif   // !NDEBUG

#ifdef		ADDLOPT
						case 'L':
							lps++;
							dwi = GetNum(lps);
							g_dwMaxLines = dwi;
							break;
#endif		// ADDLOPT

						case 'M':
							lps++;
							dwi = GetNum(lps);
							if( ( dwi ) &&
								( dwi < 256 ) )
							{
								dwi--;
								g_dwMatchCnt = dwi;
							}
							else
							{
								prt( MCRLF"ERROR: -Mnnn must be in range 1 - 255.!" );
								gIRet = Exit_Error;
								PgmExit(TRUE);
							}
							break;

						case 'N':
							g_bAddNums = TRUE;
							break;

						case 'O':
							lps++;
							strcpy( szOut, lps );
							break;

#ifdef	ADDPREV
						case 'P':
							lps++;
							g_iPrevCnt = GetNum(lps);
							break;
#endif	// ADDPREV
						case 'Q':
							g_bQuick = TRUE;
							break;

						case 'R':
							g_bReverse = TRUE;
							break;

						case 'V':
							// FIX20061111 - this option done in pre-parse
                            // lps++;
							// g_iVerbal = GetNum(lps);
							break;

						case 'X':
							lps++;
							if( GetX( pWs, lps ) )
							{
								prt( MCRLF"ERROR: Command [" );
								lps -= 2;
								prt(lps);
								prt( "] NOT supported!" );
								gIRet = Exit_Error;
								PgmExit(TRUE);
							}
							break;

						default:
							GenError( pWs );
							break;
						}
					}
					break;

				case '@':
					{
						lps = lpd + 1;
						if( GetInputFile( pWs, lps ) )
						{
							GenError( pWs );
						}
					}
					break;

				default:
					if( j == 0 )
					{
						strcpy( gszFile1, lpd );
						j++;
					}
					else if( j == 1 )
					{
						strcpy( gszFile2, lpd );
						j++;
					}
					else
					{
						GenError( pWs );
					}
				}
			}
			else
			{
				GenError( pWs );
			}
		}
		else
		{
			GenError( pWs );
		}
	}

	return gIRet;
}


int   is_valid_path( PTSTR pf )
{
#if (defined(WIN32) && defined(USE_NATIVE_WIN32))
   WIN32_FIND_DATA   fd;
   HANDLE   hFind = FindFirstFile( pf, &fd );
   if( VH(hFind) ) {
      FindClose(hFind);
      if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
         return 2;
      }
      return 1;
   }
   return 0;
#else
    return is_file_or_directory(pf);
#endif
}

// eof - Fc4Help.c
