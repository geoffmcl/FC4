
// Fc4Utils.c

#include "Fc4.h"

#define get_next_buffer GetNxtBuf
///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : GetNxtBuf
// Return type: LPTSTR 
// Argument   : VOID
// Description: Return a pointer to the "next" general buffer
//              
///////////////////////////////////////////////////////////////////////////////
PTSTR   GetNxtBuf( VOID )
{
   // (MXLINEB * MXLINES)
   INT   i = giLnBuf;      //     W.ws_iLnBuf
   i++;
   if( i >= MXLINES )
      i = 0;
   giLnBuf = i;
   return( &gszLnBuf[ (MXLINEB * i) ] );    // W.ws_szLnBuf
}
DWORD   GetNxtBufSize( VOID )
{
   return MXLINE;
}

void  Ensure_DOS_Separator( PTSTR pf )
{
   size_t len = strlen(pf);
   size_t i;
   for( i = 0; i < len; i++ )
   {
      int   c = pf[i];
      if( c == '/' ) {
         pf[i] = '\\';
      }
   }
}

PTSTR   RetFileTitle( LPTSTR lpf )
{
   PTSTR   lpb = GetNxtBuf();
   PTSTR   p, p2;
   strcpy(lpb,lpf);
   Ensure_DOS_Separator(lpb);
   p = strrchr(lpb, '\\');
   if(p)
      p++;
   else
      p = lpf;
   p2 = GetNxtBuf();
   strcpy(p2,p);
   return p2;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : InStr
// Return type: INT 
// Arguments  : LPTSTR lpb
//            : LPTSTR lps
// Description: Return the position of the FIRST instance of the string in lps
//              Emulates the Visual Basic function.
///////////////////////////////////////////////////////////////////////////////
INT   InStr( LPTSTR lpb, LPTSTR lps )  // extracted from FixFObj.c
{
   INT   iRet = 0;
   INT   i, j, k, l, m;
   TCHAR    c;
   i = strlen(lpb);
   j = strlen(lps);
   if( i && j && ( i >= j ) )
   {
      c = *lps;   // get the first we are looking for
      l = i - ( j - 1 );   // get the maximum length to search
      for( k = 0; k < l; k++ )
      {
         if( lpb[k] == c )
         {
            // found the FIRST char so check until end of compare string
            for( m = 1; m < j; m++ )
            {
               if( lpb[k+m] != lps[m] )   // on first NOT equal
                  break;   // out of here
            }
            if( m == j )   // if we reached the end of the search string
            {
               iRet = k + 1;  // return NUMERIC position (that is LOGICAL + 1)
               break;   // and out of the outer search loop
            }
         }
      }  // for the search length
   }
   return iRet;
}

INT  HasChar( PTSTR lpf, TCHAR c )
{
   INT      bRet = 0;
   INT      i;
   INT      ilen = strlen(lpf);
   for( i = 0; i < ilen; i++ )
   {
      if( lpf[i] == c )
         bRet = i + 1;
   }
   return bRet;
}

INT  HasDot( PTSTR lpf )
{
   INT   bRet = HasChar( lpf, '.' );
   return bRet;
}
INT  HasAst( LPTSTR lpf )
{
   INT   bRet = HasChar( lpf, '*' );
   return bRet;
}

INT  IsWild( PTSTR pf )
{
   if(HasChar( pf, '?') ||
      HasChar( pf, '*') ) {
      return 1;
   }
   return 0;
}

void nice_num( _TCHAR * dst, _TCHAR * src ) // get nice number, with commas
{
   size_t i;
   size_t len = STRLEN(src);
   size_t rem = len % 3;
   size_t cnt = 0;
   for( i = 0; i < len; i++ )
   {
      if( rem ) {
         *dst++ = src[i];
         rem--;
         if( ( rem == 0 ) && ( (i + 1) < len ) )
            *dst++ = ',';
      } else {
         *dst++ = src[i];
         cnt++;
         if( ( cnt == 3 ) && ( (i + 1) < len ) ) {
            *dst++ = ',';
            cnt = 0;
         }
      }
   }
   *dst = 0;
}


_TCHAR * get_nice_number( __int64 num, size_t width )
{
   size_t len, i;
   _TCHAR * pb1 = get_next_buffer();
   _TCHAR * pb2 = get_next_buffer();
   SPRINTF(pb1, "%lld", num);
   nice_num( pb2, pb1 );
   len = STRLEN(pb2);
   if( len < width )
   {
      *pb1 = 0;
      i = width - len;  // get pad
      while(i--)
         STRCAT(pb1, " ");
      STRCAT(pb1,pb2);
      pb2 = pb1;
   }
   return pb2;
}

void trim_float_buf( _TCHAR * pb )
{
   size_t len = STRLEN(pb);
   size_t i, dot, zcnt;
   for( i = 0; i < len; i++ )
   {
      if( pb[i] == '.' )
         break;
   }
   dot = i + 1; // keep position of decimal point (a DOT)
   zcnt = 0;
   for( i = dot; i < len; i++ )
   {
      if( pb[i] != '0' )
      {
         i++;  // skip past first
         if( i < len )  // if there are more chars
         {
            i++;  // skip past second char
            if( i < len )
            {
               size_t i2 = i + 1;
               if( i2 < len )
               {
                  if ( pb[i2] >= '5' )
                  {
                     if( pb[i-1] < '9' )
                     {
                        pb[i-1]++;
                     }
                  }
               }
            }
         }
         pb[i] = 0;
         break;
      }
      zcnt++;     // count ZEROS after DOT
   }
   if( zcnt == (len - dot) )
   {
      // it was ALL zeros
      pb[dot - 1] = 0;
   }
}

_TCHAR * get_k_num( __int64 i64, int ascii, int type )
{
   int dotrim = 1;
   _TCHAR * pb = get_next_buffer();
   _TCHAR * form = " bytes";
   unsigned __int64 byts = i64;
   double res;
   _TCHAR * ffm = "%0.20f";  // get 20 digits
   if( byts < 1024 ) {
      SPRINTF(pb, "%llu", byts);
      dotrim = 0;
   } else if( byts < 1024*1024 ) {
      res = ((double)byts / 1024.0);
      form = (type ? " KiloBypes" : " KB");
      SPRINTF(pb, ffm, res);
   } else if( byts < 1024*1024*1024 ) {
      res = ((double)byts / (1024.0*1024.0));
      form = (type ? " MegaBypes" : " MB");
      SPRINTF(pb, ffm, res);
   } else { // if( byts <  (1024*1024*1024*1024)){
      double val = (double)byts;
      double db = (1024.0*1024.0*1024.0);   // x3
      double db2 = (1024.0*1024.0*1024.0*1024.0);   // x4
      if( val < db2 )
      {
         res = val / db;
         form = (type ? " GigaBypes" : " GB");
         SPRINTF(pb, ffm, res);
      }
      else
      {
         db *= 1024.0;  // x4
         db2 *= 1024.0; // x5
         if( val < db2 )
         {
            res = val / db;
            form = (type ? " TeraBypes" : " TB");
            SPRINTF(pb, ffm, res);
         }
         else
         {
            db *= 1024.0;  // x5
            db2 *= 1024.0; // x6
            if( val < db2 )
            {
               res = val / db;
               form = (type ? " PetaBypes" : " PB");
               SPRINTF(pb, ffm, res);
            }
            else
            {
               db *= 1024.0;  // x6
               db2 *= 1024.0; // x7
               if( val < db2 )
               {
                  res = val / db;
                  form = (type ? " ExaBypes" : " EB");
                  SPRINTF(pb, ffm, res);
               }
               else
               {
                  db *= 1024.0;  // x7
                  res = val / db;
                  form = (type ? " ZettaBypes" : " ZB");
                  SPRINTF(pb, ffm, res);
               }
            }
         }
      }
   }
   if( dotrim > 0 )
      trim_float_buf(pb);

   STRCAT(pb, form);

   //if( ascii > 0 )
   //   Convert_2_ASCII(pb);

   return pb;
}

// eof - Fc4Utils.c
