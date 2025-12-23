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
  va_list copy4fail;     // a copy of that
  int int1, int2, int3, int4, int5, int6;
  char* str1, str2, str3, str4, str5, str6;
    
  va_copy (copy4fail, args);  // make the copy
  va_start(args, n);
    for ( int x = 0; x < n; x++ )
    {
        char* aString = va_arg ( args, char* );
    }
    va_end ( args );                  // Cleans up the list
    
  char linebuf[5000];
  switch (typePattern )
  {
      case mccwriteTypesSSNS:
          char *s1 = va_arg( copy4fail, char* );
          char *s2 = va_arg( copy4fail, char* );
          long n3 = va_arg( copy4fail, long );
          char *s4 = va_arg( copy4fail, char* );
          sprintf(linebuf,fmt,s1,s2,n3,s4);
      default:
          sprintf(linebuf,fmt,copy4fail);
  }

  if (fd == MCCLISTF && mcclistInPmode)
    {
     mcclistInPmode = FALSE;
     fputs("\n",fd);
    }
  fputs(linebuf,fd);
}

