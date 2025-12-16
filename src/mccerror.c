
#include "mcc.h"

void mccerr(int code, tokenp tok)
{
 char *text;
 static char *fmt = "ERROR: \"%s\" %d:%d : ";
 if (tok && tok->text)
   text = tok->text;
  else
   text = "";
 MCCWRITE(MCCERRF,fmt,mccsrc->pathname,tok->sourceLoc.line,
                    tok->sourceLoc.column);
 MCCWRITE(MCCERRF,mccerrmsg[code],text);
 if (mccoption[MCCOPTLISTING])
   {
     MCCWRITE(MCCLISTF,fmt,mccsrc->pathname,tok->sourceLoc.line,
                    tok->sourceLoc.column);
     MCCWRITE(MCCLISTF,mccerrmsg[code],text);
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
 MCCWRITE(MCCERRF,fmt,
                 (mccsrc) ? mccsrc->pathname : "",
                 tok->sourceLoc.line,tok->sourceLoc.column);
 MCCWRITE(MCCERRF,mccerrmsg[code],text);
 if (mccoption[MCCOPTLISTING])
   {
     MCCWRITE(MCCLISTF,fmt,
                 (mccsrc) ? mccsrc->pathname : "",
                 tok->sourceLoc.line,tok->sourceLoc.column);
     MCCWRITE(MCCLISTF,mccerrmsg[code],text);
   }
 mccwarningcount++;
}

void mccwrite(FILE *fd, char *fmt, struct { unsigned v[30]; } args)
{
  char linebuf[500];
  sprintf(linebuf,fmt,args);
  if (fd == MCCLISTF && mcclistInPmode)
    {
     mcclistInPmode = FALSE;
     fputs("\n",fd);
    }
  fputs(linebuf,fd);
}
