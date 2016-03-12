// FC4Vers.h
#ifndef  _FC4Vers_h_
#define  _FC4Vers_h_

// HISTORY
#define	VERS    "December 22, 2008"  // FIX20081222 - add CRLF after header, and at exit

// VERS    "September 13, 2008"  // FIX20080913 - elevate Show_File_Info to VERBAL2

// VERS    "September 9, 2008"  // FIX20080909 - fix some binary compare output

//	VERS    "August 19, 2008"  // FIX20080819 - add some 64-bit support
#define  USE_INT64_TYPE
//	VERS    "January 27, 2007"  // FIX20070122 - remove some macros, and some
// minor fixes in the HELP output.
// VERS    "November 11, 2006"  // FIX20061111 - In Dell01, using MSVC8
#pragma warning( disable:4996 )
//	VERS    "September 10, 2006"  // minor fix for 1 on 1 compare - FIX20060910
//	VERS    "September 6, 2006"  // attempt a fix of too much output
// and openned fc4Help.c

//	VERS    "July 9, 2006"  // minor message output changes
//	VERS    "July 8, 2006"  // it may now be right ;=))
//	VERS    "June 25, 2006"  // small fix in eorr output
// it is now compiled with MSVC7 ...
// FIXED - FIX20060625 - In trying to allow just a '.' as the second name,
// it was messing up the gszFile2 buffer - fixed by 'trying' in gszDiag,
// and only IF successful, copy the gszDiag to gszFile2 buffer ...
// the other thing annoying was if there is data left in file 1 or file 2,
// then although this would be notes, it would NOT show the data
// change - only if ADD_DEBUG is defined, is debug added ...

//	VERS    "November 17, 2005"  // allow folder\. or * as second file
// VERS    "April 1, 2001"  // allow folder\. or * as second file
// Initial cut
// VERS    "October 2, 1998"
// Switch to using STDOUT, not printf, and
// thus MCRLF becomes the pair.
// VERS    "October 8, 1998"
//	VERS    "September 27, 2000"  // fix "End - Files are ..." message
//	VERS    "November 20, 2000"  // fix -x1 - seems to not include "{" as sig line

#endif // #ifndef  _FC4Vers_h_
// eof - FC4Vers.h
