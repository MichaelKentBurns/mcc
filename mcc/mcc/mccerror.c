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
  va_start(args, n);
  
  char linebuf[500];
  sprintf(linebuf,fmt,args);
  if (fd == MCCLISTF && mcclistInPmode)
    {
     mcclistInPmode = FALSE;
     fputs("\n",fd);
    }
  fputs(linebuf,fd);
}
