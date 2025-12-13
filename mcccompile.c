/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME: mcccompile.c                                                   */
/* DOCUMENT: none                                                    */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* TYPE:                                                             */
/* PRODUCT: mikes C compiler                                         */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* SCRIPT:                                                           */
/* PURPOSE: all right, compile a file                                */
/*                                                                   */
/*  Internal functions in this module                                */
/* INCLUDE .h                                                        */
#include "mcc.h"
/* GLOBAL DECLARATIONS:                                              */
/* EXTERNAL INTERFACES:                                              */
/* HISTORY:                                                          */
/*                                                                   */
/* NOTES:                                                            */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/


static int tolist;
static void mytolist()
 {
   if (!mcclistInPmode)
      fputs("     > .i ",MCCLISTF);
   mcclistInPmode = TRUE;
 }

static int myfputc(c,stream)
  char c;
  char *stream;
 {
  int val;
  val = fputc(c,stream);
  if (tolist)
    {
      mytolist();
      fputc(c,MCCLISTF);
    }
 return val;
}
 
/********************************************************** mcccompile  */
/*-------------------------------------------------------------------*/
/* NAME: mcccompile                                                     */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: compile from stdin, listing to stdout                    */
/* USAGE:                                                            */
 boolean mcccompile ()                 
/* PARAMETERS:                                                       */
{
/* RETURNS:                                                          */
/*   true if succeeded in compiling something (ie no errors)         */
/* FATAL ERRORS:                                                     */
#ifdef DEBUG
/*                                                                   */
#endif
/* EXTERNAL INTERFACES:                                              */
/* HISTORY:                                                          */
/* NOTES:                                                            */
/*                                                                   */
/* ALGORITHM:                                                        */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/
int argno;                                                  
int starterrors;
tokenp  tok;
extern tokenp mcctok();
extern tokenp mccpptok();             
extern void   mccppfree();
extern void   mccppdump();
int curfile;
int curline;
int curcolumn;     
int lasttype;
int newfile;
int newline;
int newcolumn;
char *p;                                
int c;
int numtoks;
int slash;
int emitlinelabels;
int squash;
int got_glue;
/********************************************************** mcccompile **/
starterrors = mccerrcount;
tolist = (mcclistoption['p']);

if (mcctrcoption['g'] && mccgsym == NULL)
{
if ((mccgsym = ustrNewPool(MCCMAXRESERVED,0l,mccreserved)) == NULL)
   MCCRETURN(MCCENOMEM,FALSE);
ustrAddConst(mccgsym,"auto",AUTO     );
ustrAddConst(mccgsym,"break",BREAK    );
ustrAddConst(mccgsym,"case",CASE     );
ustrAddConst(mccgsym,"char",CHAR     );
ustrAddConst(mccgsym,"const",CONST    );
ustrAddConst(mccgsym,"continue",CONTINUE );
ustrAddConst(mccgsym,"default",DEFAULT  );
ustrAddConst(mccgsym,"do",DO       );
ustrAddConst(mccgsym,"double",DOUBLE   );
ustrAddConst(mccgsym,"else",ELSE      );
ustrAddConst(mccgsym,"enum",ENUM      );
ustrAddConst(mccgsym,"extern",EXTERN    );
ustrAddConst(mccgsym,"float",FLOAT     );
ustrAddConst(mccgsym,"for",FOR       );
ustrAddConst(mccgsym,"goto",GOTO      );
ustrAddConst(mccgsym,"if",IF        );
ustrAddConst(mccgsym,"int",INT       );
ustrAddConst(mccgsym,"long",LONG      );
ustrAddConst(mccgsym,"noalias",NOALIAS   );
ustrAddConst(mccgsym,"register",REGISTER  );
ustrAddConst(mccgsym,"return",RETURN    );
ustrAddConst(mccgsym,"short",SHORT     );
ustrAddConst(mccgsym,"signed",SIGNED    );
ustrAddConst(mccgsym,"sizeof",SIZEOF    );
ustrAddConst(mccgsym,"static",STATIC    );
ustrAddConst(mccgsym,"struct",STRUCT    );
ustrAddConst(mccgsym,"switch",SWITCH    );
ustrAddConst(mccgsym,"typedef",TYPEDEF   );
ustrAddConst(mccgsym,"union",UNION     );
ustrAddConst(mccgsym,"unsigned",UNSIGNED  );
ustrAddConst(mccgsym,"void",VOID      );
ustrAddConst(mccgsym,"volatile",VOLATILE  );
ustrAddConst(mccgsym,"while",WHILE     );     
ustrAddConst(mccgsym,"sizeof",SIZEOF        );
/* - - - - - - - - - - - - - - - - -  - - - - -*/
/* the following do not get added to ustr list, but do get added to quick table */
ustrAddQuickConst(mccgsym,"[",LSQUARE       );
ustrAddQuickConst(mccgsym,"]",RSQUARE       );
ustrAddQuickConst(mccgsym,"(",LPAREN        );
ustrAddQuickConst(mccgsym,")",RPAREN        );
ustrAddQuickConst(mccgsym,".",ELEM          );
ustrAddQuickConst(mccgsym,"->",ELEMP         );
ustrAddQuickConst(mccgsym,"++",INC           );
ustrAddQuickConst(mccgsym,"++",PREINC        );
ustrAddQuickConst(mccgsym,"++",POSTINC       );
ustrAddQuickConst(mccgsym,"--",DEC           );
ustrAddQuickConst(mccgsym,"--",PREDEC        );
ustrAddQuickConst(mccgsym,"--",POSTDEC       );
ustrAddQuickConst(mccgsym,"&",ADDRESS       );
ustrAddQuickConst(mccgsym,"*",PTR           );
ustrAddQuickConst(mccgsym,"+",PLUS          );
ustrAddQuickConst(mccgsym,"-",MINUS         );
ustrAddQuickConst(mccgsym,"-",ANEG          );
ustrAddQuickConst(mccgsym,"~",BNEG          );
ustrAddQuickConst(mccgsym,"!",NOT           );
ustrAddQuickConst(mccgsym,"/",DIVIDE        );
ustrAddQuickConst(mccgsym,"*",MULT          );
ustrAddQuickConst(mccgsym,"%",MODULUS       );
ustrAddQuickConst(mccgsym,"<<",LSHIFT        );
ustrAddQuickConst(mccgsym,">>",RSHIFT        );
ustrAddQuickConst(mccgsym,"<",LESS          );
ustrAddQuickConst(mccgsym,">",GREATER       );
ustrAddQuickConst(mccgsym,"<=",LESSEQ        );
ustrAddQuickConst(mccgsym,">=",GREATEREQ     );
ustrAddQuickConst(mccgsym,"==",EQUAL         );
ustrAddQuickConst(mccgsym,"!=",NOTEQUAL      );
ustrAddQuickConst(mccgsym,"^",XOR           );
ustrAddQuickConst(mccgsym,"|",BOR           );
ustrAddQuickConst(mccgsym,"&",BAND          );
ustrAddQuickConst(mccgsym,"||",LOR           );
ustrAddQuickConst(mccgsym,"&&",LAND          );
ustrAddQuickConst(mccgsym,"?:",TERNOP        );
ustrAddQuickConst(mccgsym,"=",ASG           );
ustrAddQuickConst(mccgsym,"*=",ASGMULT       );
ustrAddQuickConst(mccgsym,"/=",ASGDIVIDE     );
ustrAddQuickConst(mccgsym,"%=",ASGMODULUS    );
ustrAddQuickConst(mccgsym,"+=",ASGPLUS       );
ustrAddQuickConst(mccgsym,"-=",ASGMINUS      );
ustrAddQuickConst(mccgsym,"<<=",ASGLSHIFT     );
ustrAddQuickConst(mccgsym,">>=",ASGRSHIFT     );
ustrAddQuickConst(mccgsym,"&=",ASGAND        );
ustrAddQuickConst(mccgsym,"^=",ASGXOR        );
ustrAddQuickConst(mccgsym,"|=",ASGOR         );
ustrAddQuickConst(mccgsym,",",COMMA         );
ustrAddQuickConst(mccgsym,"#",PPSENTINAL    );
ustrAddQuickConst(mccgsym,"#",PPLIT         );
ustrAddQuickConst(mccgsym,"##",PPGLUE        );
ustrAddQuickConst(mccgsym,"{",LCURLY        );
ustrAddQuickConst(mccgsym,"}",RCURLY        );
ustrAddQuickConst(mccgsym,":",COLON         );
ustrAddQuickConst(mccgsym,";",SEMICOLON     );
ustrAddQuickConst(mccgsym,"...",ELIPSIS       );
}

if (mccppsym)
   ustrFreePool(mccppsym,NULL,mccppfree);
if ((mccppsym = ustrNewPool(0,0l,NULL)) == NULL)
   MCCRETURN(MCCENOMEM,FALSE);

if (mcctrcoption['u'])
   if (ustrAddIndex(mccppsym))
      MCCRETURN(MCCENOMEM,FALSE);

/* if there is any default source to be processed, push on src stack now */
if (mccdefsrc)
  {
    mccdefsrc->right = mccsrc;
    mccsrc->left = mccdefsrc;
    mccdefsrc->left = NULL;
    mccdefsrc->curchar = mccdefsrc->linestart = mccdefsrc->buffer;
    mccsrc = mccdefsrc;
    mccreadsrc(mccdefsrc,__FILE__,__LINE__); 
  }

if (mccoption[MCCOPTPPONLY])  
  {        
   squash = mccppoption['q'];
   emitlinelabels = mccppoption['l']; 
   curfile = -1;  
   curline = 1;
   curcolumn = 1;
   numtoks = 0;
   lasttype = 0;
   got_glue = FALSE;
   do {
       tok = mccpptok(0);   
       if (tok && tok->ttype == PPOP && tok->value.ival == PPGLUE)
         {
          got_glue = TRUE;
          mcctokfree(tok);
          continue;
         }
       if (tok && tok->ttype != EOFILE && tok->text)
         {    
#if 0
           if (tok->ttype == ILLTOK)
              mccerr(MCCEILLTOK,tok);
#endif
           if (!got_glue)
          {
           newfile = tok->sourceLoc.file;
           newline = tok->sourceLoc.line;
           newcolumn = tok->sourceLoc.column;
           if (newfile != curfile)
             {                       
               if (emitlinelabels)                                
                 {
                   fprintf(mccppof,"%s#%s %d \"%s\"\n",
                            (curfile != -1) ? "\n" : "",
                            mccppoption['o'] ? "" : "line",
                            newline,mccsrc->pathname);
                  curline = newline;
                 }
                else
                 {
                  fprintf(mccppof,"\n");
                  curline = 1;
                 }
               curcolumn = 1;
             }                                               
           else if (newline != curline)
             {
              if ((newline > curline && (newline - curline) < 5) 
                   || (!emitlinelabels && !squash))
                 while(curline < newline)
                    {
                     fputc('\n',mccppof);
                     curline++;
                     curcolumn = 1;
                    }          
               else
                 { 
                  if (emitlinelabels)
                    {
                     if (mccppoption['o'])
                          fprintf(mccppof,"\n#%s %d \"%s\"\n",
                            mccppoption['o'] ? "" : "line",
                            newline,mccsrc->pathname);
                        else
                           fprintf(mccppof,"\n#line %d\n",newline);
                    }
                   else
                     fprintf(mccppof,"\n",newline);
                  curline = newline;
                  curcolumn = 1;
                 }
             }
           else if ((tok->ttype != EOP && tok->ttype != PUNCT)
                && (lasttype == KEY || lasttype == ID
                    || (lasttype == tok->ttype)))
             {
               myfputc(' ',mccppof);
               curcolumn++;
             }  
           lasttype = tok->ttype;

           while (curcolumn < newcolumn && !squash)
             {
               myfputc(' ',mccppof);
               curcolumn++;
             }  
          } /* end of if !got_glue */

           if (tok->ttype == SLIT)
             {
               myfputc('"',mccppof); 
              
               newcolumn++;
               for (p = tok->text; c = *p++;)
                {
                 slash = TRUE;
                 switch(c)
                   {
                    case '"':           break;
           /*       case '\a': c = 'a'; break; */
                    case '\b': c = 'b'; break;
                    case '\f': c = 'f'; break;
                    case '\n': c = 'n'; break;
                    case '\r': c = 'r'; break;
                    case '\t': c = 't'; break;
                    case '\v': c = 'v'; break;
                    case '\\': c = '\\'; break;
                    default: slash = FALSE;
                   }
                 if (slash) 
                   {
                    myfputc('\\',mccppof);                   
                    newcolumn++;
                   }
                 if (isprint(c))
                   {
                    myfputc(c,mccppof);
                    newcolumn++;
                   }
                   else
                   {
                    fprintf(mccppof,"\\x%02x",c);
                    if (tolist)
                      {
                       mytolist();
                       fprintf(mcclistf,"\\x%02x",c);
                      }
                    newcolumn += 4;
                   }
                }
               myfputc('"',mccppof); 
               newcolumn++;
             }
           else
              for (p = tok->text; (c = *p++);)     
                {                           
                 myfputc(c,mccppof);                   
                 newcolumn++;             
                 if (c == '\n' || ((newcolumn > 256) && tok->ttype == COMMENT))
                   {
                    newline++;
                    newcolumn = 1;
                   }
                }
          curfile = newfile;
          curline = newline;
          curcolumn = newcolumn;
          numtoks++;          
          mcctokfree(tok);
         }
#if 0
       if (tok)
           MCCWRITE(MCCLISTF,
                   "mcccompile: token @%08x, id= %d, text='%s', value=%ld, location=%d/%d/%d ttype=%d=%s\n",
                    tok, tok->id, tok->text, tok->value.lval,
                    tok->sourceLoc.file,tok->sourceLoc.line,tok->sourceLoc.column,
                    tok->ttype,mccttypenames[tok->ttype]);
#endif
        
        got_glue = FALSE;
      } while (tok != NULL && tok->ttype != EOFILE);             
    if (mccppof)
       fputc('\n',mccppof);                   

    if (mccoption[MCCOPTTRACE] && mcctrcoption['s'])
       MCCTRCd("Number of tokens output to pp file =",numtoks);
  }
else
  {
   MCCTRC("dont know how to do anything but -p as of yet");
  }             

#    if DEBUG && 1                                               
       if (mccoption[MCCOPTXREF])
         {
          MCCWRITE(MCCLISTF,"\n\nGLOBAL SYMBOLS: \n");
          ustrDumpPool((ptr) MCCLISTF,mccgsym,NULL,NULL);
          MCCWRITE(MCCLISTF,"\n\nPREPROCESSOR SYMBOLS: \n");
          ustrTraversePool(mccppsym,NULL,mccppdump);
         }
#endif
if (mccgsym)
   {
    if (mcctrcoption['a'])
       ustrPoolStats(mccgsym,MCCLISTF);
    ustrFreePool(mccgsym,NULL,NULL);
   }
if (mcctrcoption['a'])
   ustrPoolStats(mccppsym,MCCLISTF);
if (!mcctrcoption['k'] || mcctrcoption['a'])
  {
   ustrFreePool(mccppsym,NULL,mccppfree);
   mccppsym = NULL;
  }

return (mccerrcount == starterrors);
}
