#include <stdarg.h>
#include "mcc.h"

void mccerr(int code, tokenp tok)
{
 char *text;
 static char *fmt = "ERROR: \"%s\" %d:%d : ";
 if (tok && tok->text)
   text = tok->text;
  else
   text = "";
 MCCWRITE(MCCERRF,fmt, 3, mccsrc->pathname, tok->sourceLoc.line,
                    tok->sourceLoc.column);
 MCCWRITE(MCCERRF,mccerrmsg[code], 1, text);
 if (mccoption[MCCOPTLISTING])
   {
     MCCWRITE(MCCLISTF,fmt, 3, mccsrc->pathname,tok->sourceLoc.line,
                    tok->sourceLoc.column);
     MCCWRITE(MCCLISTF,mccerrmsg[code],1, text);
   }
 mccerrcount++;
}

void mccwarning(int code, tokenp tok)
{
 char *text;
 static char *fmt = "WARNING: \"%s\" %d:%d : ";
 if (tok && tok->text)
   text = tok->text;
  else
   text = "";
 MCCWRITE(MCCERRF,fmt, 3,       
                 (mccsrc) ? mccsrc->pathname : "",
                 tok->sourceLoc.line,tok->sourceLoc.column);
 MCCWRITE(MCCERRF,mccerrmsg[code],1,text);
 if (mccoption[MCCOPTLISTING])
   {
     MCCWRITE(MCCLISTF,fmt, 3,
                 (mccsrc) ? mccsrc->pathname : "",
                 tok->sourceLoc.line,tok->sourceLoc.column);
     MCCWRITE(MCCLISTF,mccerrmsg[code],1,text);
   }
 mccwarningcount++;
}

void mccwrite(FILE *fd, char *fmt, int n, ...)
{
  va_list args;
    va_list myargs;
  va_start(args, n);
    for ( int x = 0; x < n; x++ )
    {
        char* aString = va_arg ( args, char* );
    }
    va_end ( args );                  // Cleans up the list
  
  char linebuf[5000];
  sprintf(linebuf,fmt,args);
  if (fd == MCCLISTF && mcclistInPmode)
    {
     mcclistInPmode = FALSE;
     fputs("\n",fd);
    }
  fputs(linebuf,fd);
}

void mccwriteTypes(FILE *fd, char *fmt, int typePattern, int n, ...)
{
  va_list args;          // incoming arg list
  int int1, int2, int3, int4, int5, int6;
  long  long1, long2, long3;
  char *str1, *str2, *str3, *str4, *str5, *str6;
    
  va_start(args, n);
    
  char linebuf[5000];
  switch (typePattern )
  {
      case mccwriteTypesS:
        {
          str1 = va_arg( args, char* );
          sprintf(linebuf,fmt,str1);
          break;
        }
      case mccwriteTypesSS:
  {
          str1 = va_arg( args, char* );
          str2 = va_arg( args, char* );
          sprintf(linebuf,fmt,str1,str2);
        }
          break;
      case mccwriteTypesSSNS:
      {
          str1 = va_arg( args, char* );
          str2 = va_arg( args, char* );
          long3 = va_arg( args, long );
          str4 = va_arg( args, char* );
          sprintf(linebuf,fmt,str1,str2,long3,str4);
      }
          break;
      default:
          sprintf(linebuf,fmt,args);
  }
  va_end ( args );                  // Cleans up the list


  if (fd == MCCLISTF && mcclistInPmode)
    {
     mcclistInPmode = FALSE;
     fputs("\n",fd);
    }
  fputs(linebuf,fd);
}

