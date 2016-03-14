/*\
 * Fc4Main.c
 *
 * Copyright (c) 2008 - 2016 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
#include "Fc4.h"

extern void Process_Files( WS );

// ================================

HANDLE	OpenOutFile( LPTSTR lpf )
{
#ifdef WIN32
	HANDLE	hFile = CreateFile( lpf,	// pointer to name of the file
		GENERIC_READ|GENERIC_WRITE,			// access (read-write) mode
		0,						// share mode
		NULL,					// pointer to security attributes
		CREATE_ALWAYS,			// how to create
		FILE_ATTRIBUTE_NORMAL,	// file attributes
		NULL );					// handle to file with attributes to 
                               // copy
#else
    HANDLE	hFile = fopen(lpf,"w");
#endif
	return hFile;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : main
// Return type: int 
// Arguments  : int argc
//            : char * argv[]
// Description: standard OS entry point
///////////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
	LPTSTR	lpd;
	LPFC4WS	pWs;
	INT      i;

   glpWs = MALLOC( sizeof(FC4WS) );
	if( glpWs )	{
		pWs = glpWs;
		ZeroMemory( pWs, sizeof(FC4WS) );
	} else {
		printf( "ERROR: Memory allocation FAILED!!!\r\n" );
		exit(-1);
	}

	gIRet = Exit_Same;
	lpd = gszDiag; // big buffer

	Init( pWs );

	ProcessCommand( pWs, argc, argv );

   giPath1 = is_valid_path( gszFile1 );
   giPath2 = is_valid_path( gszFile2 );
	if(( gszFile1[0] == 0 ) ||
		( gszFile2[0] == 0 ) ||
      ( giPath1 == 0     ) ||
      ( giPath2 == 0     ) )
	{
      g_iVerbal = 9;
      Show_CMD_Line();
        // FIX20061111
        if( gszFile1[0] ) {
            if( !giPath1 ) {
                prt("ERROR: [");
                prt( gszFile1 );
                prt("] is NOT a valid 1st FILE!"MCRLF );
            }
        } else {
           prt("ERROR: No valid file 1 found!"MCRLF);
        }
        if( gszFile2[0] ) {
            if( !giPath2 ) {
                prt("ERROR: [");
                prt( gszFile2 );
                prt("] is NOT a valid 2nd FILE!"MCRLF );
            }
        } else {
           prt("ERROR: No valid file 2 found!"MCRLF);
        }
      // FIX20060709 - FIX20080909 - DO NOT ADD HELP
      //prt_help_message();
		// prt( HD_MSG );
      prt( "ERROR: Must be given two files, or folders, to compare ;=))"MCRLF );
		//prt( MCRLF"Use -? for some help." );
		gIRet = Exit_Error;
		PgmExit(TRUE);
	}

	if( g_bReverse )
	{
		strcpy( lpd, gszFile1 );
		strcpy( gszFile1, gszFile2 );
		strcpy( gszFile2, lpd );
      i = giPath1;
      giPath1 = giPath2;
      giPath2 = i;
	}

	if( g_bQuick )
	{
		// IF be QUICK
		szOut[0] = 0;	// NO output file, and
		g_iVerbal = 0;	// ZERO verbosity.
	}

	if( szOut[0] ) {
		ghOutFil = OpenOutFile( szOut );
	}

   Show_CMD_Line();

	if(( VERBAL8  ) &&
		( szOut[0] ) )
	{
		if( VH(ghOutFil) )
		{
			sprintf( lpd,
				MCRLF"Output file: %s of %s at %s",
				&szOut[0],
				GetsszDate(),
				GetsszTime() );
		}
		else
		{
			strcat( lpd, MCRLF"Output file FAILED!" );
		}
		prt(lpd);
	}

   if( giPath1 == 2 ) {   // we have a FOLDER name
      strcpy(gszPath1, gszFile1);
      Expand_Folder( &gsList1, gszPath1 );
   } else {
      Add_2_List( &gsList1, gszFile1 );
   }
   if( giPath2 == 2 ) {   // we have a FOLDER name
      strcpy(gszPath2, gszFile2);
      Expand_Folder( &gsList2, gszPath2 );
   } else {
      Add_2_List( &gsList2, gszFile2 );
   }

   Process_Files( pWs );

	PgmExit(FALSE);   // return after cleanup

	return gIRet;
}

// Fc4Main.c
