extern char *malloc();           
#define mlbfix
#include "mcc.h"
#include <time.h>
/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME:     mccpp                                                   */
/* DOCUMENT: none                                                    */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* TYPE:                                                             */
/* PRODUCT:                                                          */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* SCRIPT:                                                           */
/* PURPOSE: parse out and preprocess c language tokens               */
/*                                                                   */
/*  Internal functions in this module                                */
        tokenp mccendline ( int, line ;
        tokenp mccppexp ( tokenp );
/* INCLUDE .h                                                        */
/* GLOBAL DECLARATIONS:                                              */
/* EXTERNAL INTERFACES:                                              */
/* HISTORY:                                                          */
/*                                                                   */
/* NOTES:                                                            */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/


/********************************************************** mccpptok */
/*-------------------------------------------------------------------*/
/* NAME: mccpptok                                                      */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: get a token after preprocessing                          */
/* USAGE:                                                            */
 tokenp mccpptok( recursive )
/* PARAMETERS:                                                       */
int recursive; /* 0 except when mccpptok calls itself */
{
/* RETURNS:                                                          */
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
                             
tokenp tok,flagtok,keytok,nexttok,firstarg;
tokenp toklist;
tokenp maybeglue;
tokenp second; 
static int    file = -1;
static int    line;
static int    column;                   
int got_one;  
tokenp macargs;   /* linked list of macro formal args */ 
tokenp macrepl;   /* linked list of macro replacement tokens */
tokenp macname;   /* token which is name of macro */   
tokenp currepl;   /* current replacement token */
int    curlevel;  /* current level of nesting in macnest */   
char   *text;   
int    argno;                                                   
tokenp temp;   
tokenp last;
tokenp templist;                           
tokenp littok;
int    litlen;
int    litcurline;
int    litcurcol;
int    dolit;
int    litmacnestlevel;
int    errors;

static mccpptok_spanlines = TRUE;
static macnestlevel = 0;
static struct {
                tokenp  macname;
                tokenp  currepl;           
                tokenp  curargs;
                tokenp  curargtok; 
                sloc    sourceLoc;
                sloc    startLoc;
                sloc    lasttoksloc;
                tokenp  glue_left;
              } macnest[MCCMACNESTINV];              

#if 0
static int   mccppiflevel = 0;
static char  mccppifnest[ANSINESTPPIF];
#endif
static sloc  mccppifnestSloc[ANSINESTPPIF];
static int curif = IFTHEN;
static int curiflevel= 0;

static int   isinited = 0;
/* static macro definition for __FILE__ */                    
static char __file__text[MCCMAXPATHLEN+1];
static token __file__repl 
#if INITU
  = { SIGTOK, NULL, NULL, 0, 0, __file__text, 
         { NULL }, { 0,0,0 } , SLIT }
#endif
;
static token __file__ 
#if INITU 
 =  { SIGTOK, NULL, NULL, 0, 0, "__FILE__", 
         { NULL }, { 0,0,0 } , ID }
#endif
;     

/* static macro definition for __LINE__ */
static char  __line__text[10];
static token __line__repl 
#if INITU 
   { SIGTOK, NULL, NULL, 0, 0, __line__text, 
         { NULL }, { 0,0,0 } , ILIT }
#endif 
;
static token __line__ 
#if INITU 
 = { SIGTOK, NULL, NULL, 0, 0, "__LINE__", 
         { NULL }, { 0,0,0 } , ID }
#endif
;

/* static macro definition for __STDC__ */
static token __stdc__repl 
#if INITU
= { SIGTOK, NULL, NULL, 0, 0, "1", 
         { NULL }, { 0,0,0 } , ILIT }
#endif
;
static token __stdc__
#if INITU
 = { SIGTOK, NULL, NULL, 0, 0, "__STDC__", 
         { NULL }, { 0,0,0 } , ID }
#endif
;

/* static macro definition for __DATE__ */
static char  __date__text[12];
static token __date__repl 
#if INITU
= { SIGTOK, NULL, NULL, 0, 0, __date__text, 
         { NULL }, { 0,0,0 } , SLIT }
#endif
;
static token __date__
#if INITU
 = { SIGTOK, NULL, NULL, 0, 0, "__DATE__", 
         { NULL }, { 0,0,0 } , ID }
#endif
;

/* static macro definition for __TIME__ */
static char  __time__text[10];
static token __time__repl 
#if INITU
= { SIGTOK, NULL, NULL, 0, 0, __time__text, 
         { NULL }, { 0,0,0 } , SLIT }
#endif
;
static token __time__ 
#if INITU
= { SIGTOK, NULL, NULL, 0, 0, "__TIME__", 
         { NULL }, { 0,0,0 } , ID }
#endif
;

extern tokenp mcctok();
/*-------------------------------------------------------------------*/
                        
if (!isinited)
  {                                       
    struct tm *nowtm;
    long       now;
    int        errors;   
    static  char *mon_name[] = 
        { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
                      
    errors = 0;
#if !INITU
    __file__repl.sig = SIGTOK;
    __file__repl.left = NULL;
    __file__repl.right = NULL;
    __file__repl.id = 0;
    __file__repl.refcnt = 0;
    __file__repl.text = __file__text;
    __file__repl.value.ival = 0;
    __file__repl.sourceLoc.file = 0;
    __file__repl.sourceLoc.line = 0;
    __file__repl.sourceLoc.column = 0;
    __file__repl.ttype = SLIT;
    __file__ = __file__repl;
    __file__.text = "__FILE__";
    __file__.ttype = ID;
    __line__repl = __file__repl;
    __line__repl.text = __line__text;
    __line__repl.ttype = ILIT;
    __line__ = __line__repl;
    __line__.text = "__LINE__";
    __line__.ttype = ID;
    __stdc__repl = __line__repl;
    __stdc__repl.text = "1";
    __stdc__ = __line__;
    __stdc__.text = "__STDC__";
    __stdc__.ttype = ID;
    __date__repl = __line__repl;
    __date__repl.text = __date__text;
    __date__repl.ttype = SLIT;
    __date__ = __stdc__;
    __date__.text = "__DATE__";
    __time__repl = __date__repl;
    __time__repl.text = __time__text;
    __time__ = __date__;
    __time__.text = "__TIME__";
#endif
    now = time(NULL);
    nowtm = localtime(&now);                         
    sprintf(__date__text,"%.3s %.2d 19%.2d",mon_name[nowtm->tm_mon],
                nowtm->tm_mday, nowtm->tm_year);
    sprintf(__time__text,"%.2d:%.2d:%.2d", nowtm->tm_hour,
                    nowtm->tm_min, nowtm->tm_sec);
    __file__.value.pval = (char *) &__file__repl;
    __line__.value.pval = (char *) &__line__repl;
    __stdc__.value.pval = (char *) &__stdc__repl;
    __date__.value.pval = (char *) &__date__repl;
    __time__.value.pval = (char *) &__time__repl;
    if ((tokenp ) ustrAddConst(mccppsym,__file__.text,&__file__)
               != &__file__)
       errors++;
    if ((tokenp ) ustrAddConst(mccppsym,__line__.text,&__line__)
               != &__line__)
       errors++;
    if ((tokenp ) ustrAddConst(mccppsym,__stdc__.text,&__stdc__)
               != &__stdc__)
       errors++;
    if ((tokenp ) ustrAddConst(mccppsym,__date__.text,&__date__)
               != &__date__)
       errors++;
    if ((tokenp ) ustrAddConst(mccppsym,__time__.text,&__time__)
               != &__time__)
       errors++;
    if (errors)
       MCCTRC("error adding predefined macros to mac name table");
    isinited = TRUE;
  }
 
got_one = dolit = FALSE; 
littok = NULL;
if (file == -1)
  {
    if (mccsrc)
      {
       file = mccsrc->id;
       line = /* mccsrc->lineno */ 0 ;
       column = mccsrc->curchar - mccsrc->linestart + 1;
      }
     else
      {
        file = -1;
        line = -1;
        column = -1;
      }
  }
                  
do {                  
#if DEBUG
       if (curif != IFTHEN && curif != IFELSE && curif != IFNEVER)
        {
        MCCWRITE(MCCLISTF,"INTERNAL ERROR: mccppiflevel=%d, curif=%d, mccpp.c:%d\n",
             mccppiflevel,curif,__LINE__);
        }
#endif
#if DEBUG
       if (mccppiflevel < 0)
        {
        MCCWRITE(MCCLISTF,"INTERNAL ERROR: mccppiflevel=%d, mccpp.c:%d\n",
             mccppiflevel,__LINE__);
        }
#endif
     if (mccppiflevel > 0)
       {                
 curiflevel = mccppiflevel - 1;
 if (mccppiflevel >= ANSINESTPPIF)
             curif = IFNEVER;
           else
     curif = mccppifnest[curiflevel];
       }                            
     else
       {
         curif = IFTHEN;
       }
#if DEBUG
       if (curif < IFNEVER || curif > IFTHEN)
        {
        MCCWRITE(MCCLISTF,"INTERNAL ERROR: mccppiflevel=%d, curif=%d, mccpp.c:%d\n",
             mccppiflevel,curif,__LINE__);
        }
#endif
#if DEBUG
       if (macnestlevel < 0)
        {
        MCCWRITE(MCCLISTF,"INTERNAL ERROR: macnestlevel=%d, mccpp.c:%d\n",
             macnestlevel,__LINE__);
        }
#endif
     if (macnestlevel > 0)
       {                                            
        curlevel = macnestlevel - 1;
        macname = macnest[curlevel].macname;
        macrepl = macnest[curlevel].currepl;
        macargs = macnest[curlevel].curargs;
        tok = macnest[curlevel].curargtok;
#       if DEBUG && 1                                               
           if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
             {
              MCCWRITE(MCCLISTF,"got macro expansion level %d tok=%08x\n",curlevel,tok);
               MCCWRITE(MCCLISTF,"macname=%s, macrepl=%08x, macargs=%08x\n",
               macname->text,macrepl,macargs);       
             }
#       endif
        if (tok)
           {  /* got tokens left in current macro argument expansion */
             macnest[curlevel].curargtok = tok->right;
             if (tok->right)
                 tok->right->left = NULL;
             tok->right = tok->left = NULL;
             macnest[curlevel].sourceLoc.column += tok->sourceLoc.column;
             tok->sourceLoc = macnest[curlevel].sourceLoc;
             macnest[curlevel].sourceLoc.column 
                     += strlen(tok->text) + ((tok->ttype==SLIT) ? 2 : 0);
           }
        if (!tok && macrepl)
           {  /* expand the next replacement token from macro def */
            if (macrepl->ttype == PPARG)
              {
               templist = NULL;
               for (argno = 1;
                    argno < macrepl->value.ival && macargs;
                    argno++, macargs = macargs->left)
                 ;
#              if DEBUG
                 if (argno != macrepl->value.ival)
                   MCCTRCd("trouble finding macro argument",
                          macrepl->value.ival)
                 else
#              endif
                   { /* copy the token list from macro argument */
                     for (last = NULL;
                          macargs && macargs->ttype != NULLTOK;
                          macargs = macargs->right)
                       {
                        temp = (tokenp) mcctokcopy(macargs);
                        if (!templist)     
                          {
                           templist = temp;
                           temp->left = NULL;
                          } 
                         else
                          {
                           temp->left = last;
                           last->right = temp;
                          }
                        last = temp;
                       } 
                   }
                tok = templist;  /* grab first expanded tok*/
                /* and put the rest on list for later */
                macnest[curlevel].curargtok = templist->right;
              }
            else if (macrepl->ttype == PPOP && macrepl->value.ival == PPLIT)
              {
                dolit = TRUE;
                litmacnestlevel = macnestlevel;
                tok = NULL;
              }
            else
               tok = (tokenp) mcctokcopy(macrepl);
            /* skip current replace pointer past this repl token */
            if (macrepl)
               macnest[curlevel].currepl = macrepl->right;
             else
               macnest[curlevel].currepl = NULL;
           }
        macrepl = macnest[curlevel].currepl;
        if (tok && tok->ttype == COMMENT)
          {
           mcctokfree(tok);
           tok = NULL;
          }
#if 0
        /* look for glue following this token */
        if (tok && macrepl && macrepl->ttype == PPOP
                && macrepl->value.ival == PPGLUE)
           {
             MCCTRC("dont know how to glue yet");
           }
#endif
         if (tok
             &&(   (tok->sourceLoc.file != macnest[curlevel].sourceLoc.file)
                || (tok->sourceLoc.line != macnest[curlevel].sourceLoc.line)))
           {
            if (tok->sourceLoc.line)
               macnest[curlevel].sourceLoc.column++;
            else
               macnest[curlevel].sourceLoc.column += tok->sourceLoc.column;
            tok->sourceLoc = macnest[curlevel].sourceLoc;
            macnest[curlevel].sourceLoc.column += strlen(tok->text)
                       + ((tok->ttype==SLIT) ? 2 : 0);
          }
         if (   macnest[curlevel].currepl == NULL
             && macnest[curlevel].curargtok == NULL)
           {                  
             tokenp next;
             for (macargs = macnest[curlevel].curargs;
                  macargs;
                  macargs = next)
               {
                 next = macargs->left;
                 mcctokfreelist(macargs);
               }
             if (macnestlevel > 0)
               {
#               if DEBUG && 1                                               
                  if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
                      MCCWRITE(MCCLISTF,"finished expansion (last token) of macro '%s'!\n",
                         macname->text);
#               endif          
                macnestlevel--;  /* finished with macro expansion, pop off */
               }
           }
       }
      else
       {  
        tok = mcctok(mccpptok_spanlines);
       }
     if (!tok || (tok && tok->ttype == EOFILE))
       {
        if (tok && tok->ttype == EOFILE)
            mccclosesrc(NULL);
        if (mccsrc == NULL)
          {
         /*  if (curif != IFTHEN) */
             {
      while (mccppiflevel > 0)
                {
                 char line[200];
                 mccppiflevel--;
                 mccwarningcount++;
                 sprintf(line,
"\
WARNING: Encountered end of compilation unit with unterminated \n\
         #if/#ifdef/#ifndef ... #endif which began at %d:%d.\n\
         (current location is %d:%d).\n",
  mccppifnestSloc[mccppiflevel].file,
  mccppifnestSloc[mccppiflevel].line,
                      tok->sourceLoc.file,tok->sourceLoc.line);
                 MCCWRITE(MCCERRF,line);
                 if (mccoption[MCCOPTLISTING])
                   {
                     MCCWRITE(MCCLISTF,line);
                   }
                }
             }
           break;
          }
       }        
     else if (tok && tok->ttype == PPOP)
       {
        if (tok->value.ival == PPGLUE)
           got_one = TRUE;  /* for now return GLUE's up to mcccompi */
          else
            if (tok->value.ival == PPLIT && 
              ( tok->sourceLoc.file != file ||
                (tok->sourceLoc.file == file 
                  && tok->sourceLoc.line != line ) ) )
             tok->ttype = PPFLAG;
        if (tok->ttype == PPFLAG)
           {
            flagtok = tok;
            tok = NULL;
            toklist = mccendline(flagtok->sourceLoc.line);
            keytok = toklist;     
            if (!keytok) break;
            toklist = nexttok = keytok->right;
            keytok->right = NULL; 
            if (keytok->sourceLoc.line != flagtok->sourceLoc.line)
              mccpushtok(keytok);
             else
              {
               if (keytok->ttype == ID || keytok->ttype == KEY)
                 {                               
                  if (strcmp(keytok->text,"include") == 0)
                    {  
                     if (curif == IFTHEN)
                       {
                        if (nexttok)
                          {   
                            int  searchtype;  
                            char *path;
                            char pathbuf[200]; 
                            if (nexttok->ttype == SLIT) 
                               {
                                path = nexttok->text;
                                searchtype = MCCHOMESEARCH;
                               }
                              else                   
                                if (nexttok->ttype == EOP
#if 1
                                    && nexttok->text && *nexttok->text == '<')
#else
                                    && nexttok->value.ival == LESS)
#endif
                                  {
                                   char *s,*d;             
                                   searchtype = MCCSYSSEARCH;
                                   path = pathbuf;
                                   *path = 0;
                                   for (s = nexttok->text+1,d=path; *s; )
                                        *d++ = *s++;
                                   for (nexttok = nexttok->right;
                                        nexttok && !(nexttok->ttype == EOP 
                                                && nexttok->value.ival == GREATER);
                                       )
                                      {         
                                        for (s = nexttok->text; *s; )
                                             *d++ = *s++;
                                         nexttok = nexttok->right;
                                      }                          
                                   *d = 0;
                                   if (!nexttok) 
                                     {
                                       mccerr(MCCINCFSYNTAX,NULL);
                                       *path = 0;
                                     }
                                  }
                               else
                                  {
                                   mccerr(MCCINCFSYNTAX,nexttok);
                                   path = NULL;
                                  }
                            if (path && *path)
                               if (!mccopensrc(path,0,searchtype))
#                                 if 0
                                     MCCWRITE(MCCERRF,mccerrmsg[MCCOINCF],path);
#                                 else 
                                     ;
#                                 endif
                          }
                        else
                          {
                            mccerr(MCCNOINCF,keytok); 
                          }
                       }
                    }
                  else if (strcmp(keytok->text,"define") == 0)
                   {
                    if (curif == IFTHEN)
                     {                               
                      int pnest; 
                      int argno;
                      tokenp scan;  
                      int  expectId;                       
                      int  faListOk;              
                      char *name;
                      int tempcolumn;
                      int curcolumn;
                      int lastline;

                      errors = 0;                      
                      toklist = NULL; 
                      macname = nexttok;        
                      macargs = nexttok->right;
                      name = nexttok->text;
                      if (name[0] == '_'
                           && (  strcmp(name,__line__.text) == 0
                               ||strcmp(name,__file__.text) == 0
                               ||strcmp(name,__stdc__.text) == 0
                               ||strcmp(name,__date__.text) == 0
                               ||strcmp(name,__time__.text) == 0))
                         mccwarning(MCCPPNOREDEF,nexttok);
                       else
                         {
                           if (macargs && macargs->ttype == PUNCT
                                    && macargs->value.ival == LPAREN 
                                    && macargs->sourceLoc.column ==
                                       macname->sourceLoc.column + 
                                       strlen(macname->text))
                             { /* function type macro */ 
#                              if DEBUG && 1                                               
                                 if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
                                    MCCWRITE(MCCLISTF,"funct type macro named '%s'\n",
                                             macname->text);
#                              endif
                               macargs = macargs->right; /* skip lparen */
                               mcctokfree(macargs->left); /* free lparen */
                               macrepl = NULL; 
                               faListOk = FALSE;    
                               if (macargs && macargs->ttype == PUNCT && macargs->value.ival == RPAREN)
                                  {  
                                    macrepl = macargs->right;
                                    if (macrepl)
                                        macrepl->left = NULL;   
                                    macargs->right = NULL;
                                    macname->right = macargs; /* no arguments to macro, gotta do something */
                                    free(macargs->text);
                                    macargs->text = "";
                                    faListOk = TRUE;
                                  }
                                else
                                 for ( scan = macargs, argno = 1, expectId = TRUE;
                                       scan;
                                       )
                                   {
                                     scan->sourceLoc.file = 
                                          scan->sourceLoc.line = 
                                             scan->sourceLoc.column = 0;
                                     if ((expectId && 
                                          (scan->ttype == ID 
                                             || scan->ttype == KEY))
                                        || (!expectId && scan->ttype == PUNCT))
                                      {
                                       if (expectId)        
                                         {
                                          scan->value.ival = argno++;
#                                         if DEBUG && 1                                               
                                           if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
                                              MCCWRITE(MCCLISTF,"macro arg %d is '%s'\n",
                                                 scan->value.ival,scan->text);
#                                         endif
                                         }          
                                      else
                                         {     
                                           temp = scan;      
                                           if (temp->value.ival == RPAREN)
                                              { /* done with arg list */
                                                macrepl=temp->right;
                                                mccppspecial(temp);
                                                mcctokfree(temp);      
                                                faListOk = TRUE;
                                                break;  /* quit loop */
                                              }                        
                                            else if (temp->value.ival == COMMA)
                                              { /* remove comma */
                                               scan = temp->left;
                                               temp->left->right = temp->right;
                                               temp->right->left = temp->left;
                                               mcctokfree(temp);
                                              }
                                            else 
                                              {
                                               errors++;
                                               mccerr(MCCBADPPFA,temp);
                                              }
                                         }    
                                       expectId = !expectId;
                                      }
                                     else /* unrecognized token in macro */
                                      {
                                       errors++;
                                       mccerr(MCCBADPPFA,scan);
                                      }
                                     scan = scan->right;
                                   }
                                if (faListOk && !errors)
                                 { /* fixup repl list */            
                                   macname->value.pval = (ptr) macrepl;
                                   curcolumn = macrepl->sourceLoc.column;
                                   lastline = macrepl->sourceLoc.line;
                                   for (scan = macrepl;
                                        scan; 
                                        scan = scan->right)
                                     {
                                       tempcolumn = scan->sourceLoc.column;
                                       if (scan->sourceLoc.line 
                                           && scan->sourceLoc.line == lastline)
                                           scan->sourceLoc.column -= curcolumn; 
                                         else
                                           scan->sourceLoc.column = 1;
                                       lastline = scan->sourceLoc.line; 
                                       curcolumn = tempcolumn 
                                            + strlen(scan->text) 
                                            + ((scan->ttype==SLIT) ? 2 : 0);
                                       scan->sourceLoc.file = 
                                               scan->sourceLoc.line = 0;
                                       if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
                                           MCCWRITE(MCCLISTF,
                           "repl tok '%s' follows %d blanks, curcolumn=%d\n",
                                             scan->text,
                                             scan->sourceLoc.column,
                                             curcolumn);
                                       if (scan->ttype == ID 
                                              || scan->ttype == KEY)
                                         {
                                           for (temp = macargs, argno = 1; 
                                                temp; 
                                                temp = temp->right, argno++)
                                             if (strcmp(scan->text,temp->text)==0)
                                               {
                                                scan->ttype = PPARG;
                                                scan->value.ival = argno;
#                                               if DEBUG && 1                                               
                                                 if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
                                                   MCCWRITE(MCCLISTF,"macro repl token '%s' is arg %d\n",
                                                           scan->text,scan->value.ival);
#                                               endif
                                                break;
                                               }
                                         }
                                     }
                                 }                                         
                                else
                                 {                         
                                   mcctokfree(macname);
                                   mcctokfreelist(macargs);
                                   mcctokfreelist(macrepl);
                                 }
                             }                          
                           else
                             { /* object type macro */
#                              if DEBUG && 1                                               
                                 if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
                                   MCCWRITE(MCCLISTF,"object type macro named '%s'\n",
                                             macname->text);
#                              endif
                               macrepl = macargs;
                               macargs = NULL;
                             }           
                           if (!errors)
                             {     
                              macname->value.pval = (ptr) macrepl;
                              macname->right = macargs;
                              if ((tokenp ) ustrAddConst(mccppsym,macname->text,macname)
                                   != macname)
                                {
                                 tokenp cur,currepl,newrepl;
                                 int mismatch;
                                 
                                 cur = (tokenp) ustrId(mccppsym,nexttok->text);
                                 if (cur)
                                    {               
                                      for (currepl = (tokenp) cur->value.pval,
                                           newrepl = macrepl, mismatch = FALSE;
                                           currepl || newrepl;
                                           currepl = currepl->right, newrepl = newrepl->right)
                                         {
                                           if (!currepl || !newrepl
                                               || currepl->ttype != newrepl->ttype
                                               || strcmp(currepl->text,newrepl->text) != 0)
                                             {
                                               mismatch = TRUE;
                                               break;
                                             }
                                         }                     
                                      if (mismatch)
                                        { 
                                          mccwarning(MCCPPREDEFN,macname);
                                        }                                   
                                      /* free old definition, replace with new */    
                                      /* free old repl list and args */
                                      mcctokfreelist(cur->value.pval);
                                      mcctokfreelist(cur->right);
                                      /* replace old defn root with value new */
                                      cur->value.pval = macname->value.pval;                                 
                                      cur->right = macname->right;
#if 0
                                      cur->sourceLoc = macname->sourceLoc;
#else
                                      cur->sourceLoc.file = 
                                          cur->sourceLoc.line = 0;
#endif
                                      cur->id = macname->id; 
                                      /* now free the new root struct */
                                      mcctokfree(macname);
                                    }
                                   else
                                     MCCTRC("error adding macro to mac name table");
                                }
#                             if DEBUG && 1                                               
                                if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
                                  MCCWRITE(MCCLISTF,"mccpp: added '%s' to macro pool\n",macname->text);
#                             endif        
                             }
                         }
                     }
                    }
                  else if (strcmp(keytok->text,"pragma") == 0)
                    {
                      if (curif == IFTHEN && nexttok)
                        {                                    
                         if (strcmp(nexttok->text,"listoff") == 0)
                            mccliston = FALSE;
                         else if (strcmp(nexttok->text,"liston") == 0)
                            mccliston = TRUE;
                         else if (strcmp(nexttok->text,"traceon") == 0)
                            mccoption[MCCOPTTRACE] = TRUE;
                         else if (strcmp(nexttok->text,"traceoff") == 0)
                            mccoption[MCCOPTTRACE] = FALSE;
                         else if (strcmp(nexttok->text,"trace") == 0)
                            {
                              char opt;
                              int val = TRUE;
                              char *p;
                              if (nexttok->right && nexttok->right->text)
                                {
                                  p = nexttok->right->text;
                                  while (opt = *p++)
                                     {
                                       switch(opt)
                                         {
                                           case '!': val = FALSE; break;
                                           case '-': val = TRUE; break;
                                           default: 
                                             mcctrcoption[opt] = val;
                                         }
                                     }
                                }
                            }
                          else if (strcmp(nexttok->text,"ppdump") == 0)
                            {
                              MCCWRITE(MCCLISTF,"curif = %d, if level=%d\n",
                                       curif,mccppiflevel);
                            }
                        }
                    }
                  else if (strcmp(keytok->text,"elif") == 0
                           || (strcmp(keytok->text,"if") == 0))
                    {    
#ifndef NOFIX1
                      int iselif =
                           (strcmp(keytok->text,"elif") == 0);
#endif
                      int save_spanlines = mccpptok_spanlines;
                      mccpptok_spanlines = FALSE;
#ifndef NOFIX1
                      if ((iselif && curif == IFELSE) ||
                          (!iselif && curif == IFTHEN))
#else
                      if (curif == IFTHEN)
#endif
                         {               
                          mccppifnest[curiflevel] = IFTHEN;
                          if (nexttok)   
                              {
#if 0
                               if (nexttok->ttype == ILIT)
                                  curif = (nexttok->value.ival) ? IFTHEN : IFELSE;
                                 else
#endif
                                   {
                                    tokenp saveq;
                                    tokenp end;
                                    tokenp explist = NULL;
                                    tokenp lasttok = NULL;
                                    tokenp tp;
#                                   define UNKNOWN -5
                                    int andval = UNKNOWN;
                                    int orval = UNKNOWN;
                                    
                                    saveq = mccsrc->tokq;
                                    mccsrc->tokq = nexttok;
                                    for (tp = nexttok; tp && tp->right;)
                                       tp = tp->right;
                                    tp->right = end = (tokenp) malloc(sizeof(token));
                                    *end = *nexttok;
                                    end->ttype = MARKER;
                                    end->right = NULL;
                                    end->text = NULL;
                                    toklist = NULL;
                                    nexttok = NULL;
                                    do {
                                        while (TRUE)
                                           {
                                            nexttok = mccpptok(recursive+1);  /* 1 */ 
                                            if (nexttok 
                                                && nexttok->ttype == COMMENT)
                                                 mcctokfree(nexttok);
                                              else
                                                 break;
                                           } 
                                        if (nexttok && nexttok->ttype != MARKER)
                                          {
                                             if (nexttok->ttype == ID
                                                 || nexttok->ttype == KEY)
                                               {
                                                if (strcmp(nexttok->text,"defined") == 0)
                                                   {
                                                     mcctokfree(nexttok);
                                                     while (TRUE)
                                                        {      /* 2 */
#ifdef NOFIX2
                                                         nexttok = mccpptok(recursive+1);
#else
                                                          if (mccsrc->tokq)
                                                            {
                                                              nexttok = mccsrc->tokq;
                                                              mccsrc->tokq = mccsrc->tokq->right;
                                                              if (mccsrc->tokq)
                                                                 mccsrc->tokq->left = NULL;
                                                            }
                                                           else
                                                             nexttok = mccpptok(recursive+1);

#endif
                                                         if (nexttok 
                                                             && nexttok->ttype
                                                               == COMMENT)
                                                            mcctokfree(nexttok);
                                                           else
                                                              break;
                                                        } 
                                                     if (nexttok && nexttok->ttype == PUNCT
                                                         && nexttok->value.ival == LPAREN)
                                                        {
                                                         if (macnestlevel)
                                                           mccwarning(MCCNOIFEXP,nexttok);
                                                          else if (mccsrc->tokq)
                                                            {
                                                              mcctokfree(nexttok);
                                                              nexttok = mccsrc->tokq;
                                                              mccsrc->tokq = mccsrc->tokq->right;
                                                              if (mccsrc->tokq)
                                                                 mccsrc->tokq->left = NULL;
                                                              if (nexttok->ttype == ID
                                                                  || nexttok->ttype == KEY)
                                                                {
                                                                 nexttok->ttype = ILIT;
                                                                 nexttok->value.ival =
                                                                    (ustrId(mccppsym,nexttok->text)) ?
                                                                           TRUE : FALSE;
                                                     while (TRUE)
                                                        {   /* 3 */
                                                         tp = mccpptok(recursive+1);
                                                         if (tp 
                                                             && tp->ttype
                                                               == COMMENT)
                                                            mcctokfree(tp);
                                                           else
                                                              break;
                                                        } 
                                                                 if (!(tp && 
                                                                     tp->ttype == PUNCT
                                                                     && tp->value.ival
                                                                          == RPAREN))
                                                                     mccerr(MCCSYNTAX,nexttok);
                                                                 if (tp)
                                                                    mcctokfree(tp);
                                                                }
                                                               else
                                                                mccerr(MCCSYNTAX,nexttok);      
                                                            }
                                                        }
                                                      else
#ifdef NOFIX3
                                                         mccerr(MCCSYNTAX,nexttok);
#else
                                                        {
                                                              if (nexttok->ttype == ID
                                                                  || nexttok->ttype == KEY)
                                                                {
                                                                 nexttok->ttype = ILIT;
                                                                 nexttok->value.ival =
                                                                    (ustrId(mccppsym,nexttok->text)) ?
                                                                           TRUE : FALSE;
                                                                }
                                                              else
                                                                mccerr(MCCSYNTAX,nexttok);
                                                        }
#endif
                                                   }
                                                  else
                                                    curif = IFELSE;
                                               }
                                          }
                                        if (nexttok && nexttok->ttype != MARKER) 
                                          {  /* add it to the expression list */
                                           if (lasttok == NULL)
                                               explist = nexttok;
                                             else
                                               lasttok->right = nexttok;
                                           nexttok->left = lasttok;
                                           nexttok->right = NULL;
                                           lasttok = nexttok;
                                          }
       } while (nexttok && nexttok->ttype != MARKER);
                                    if (explist)
                                        explist = mccppexp(explist);
                                    if (explist && explist->right)
                                        mccerr(MCCIREDPPEXP,explist->right); 
                                    if (explist)
                                       {
                                        curif = (explist->value.ival) ? IFTHEN : IFELSE;
                                        mcctokfreelist(explist);
                                       }
                                      else
                                        mccerr(MCCNULLPPEXP,keytok);
                                    toklist = mccsrc->tokq;
                                    if (toklist)
      mccwarning(MCCNOIFEXP,toklist);
    if (macnestlevel)
mccwarning(MCCNOIFEXP,nexttok);
                                    if (nexttok)
                                        mcctokfree(nexttok);
    while(macnestlevel > 0)
       {
tokenp next;
curlevel = macnestlevel - 1;
mcctokfreelist(
macnest[curlevel].currepl);
mcctokfreelist(
      macnest[curlevel].curargtok);
for (macargs =
macnest[curlevel].curargs;
     macargs;
     macargs = next)
    {
     next = macargs->left;
     mcctokfreelist(macargs);
    }
macnestlevel--;
       }
                                    mccsrc->tokq = saveq;
                                   }
                              }
                           else
                             curif = IFELSE;
                         }
                       else
                         curif = IFNEVER;                                
#ifdef NOFIX1
                      if (strcmp(keytok->text,"if") == 0)
#else
                      if (!iselif)
#endif
                        {
          if (mccppiflevel >= ANSINESTPPIF)
                              mccerr(MCCPPIFOFLOW,keytok);
                            else 
                              {
       mccppifnest[mccppiflevel] = curif;
       mccppifnestSloc[mccppiflevel] = keytok->sourceLoc;
                              }
          mccppiflevel++;
                        }
                       else
                        {
         if (mccppiflevel)
                           {            
    if (!(mccppiflevel < ANSINESTPPIF
        && mccppifnest[curiflevel] == IFTHEN))
                               curif = IFNEVER;                                
                             mccppifnest[curiflevel] = curif;
                           }
                          else
                           mccerr(MCCPPIFUFLOW,keytok);
                       }
                     mccpptok_spanlines = save_spanlines;
                    }
                  else if (strcmp(keytok->text,"undef") == 0)
                    {        
                      tokenp id;                                
                      char   *name;
                      if (curif == IFTHEN)
                         {         
                          name = nexttok->text;      
                          if (nexttok && 
                              (nexttok->ttype == ID || nexttok->ttype == KEY)
                                && name)
                             { 
                              if (name[0] == '_'
                                   && (  strcmp(name,__line__.text) == 0
                                       ||strcmp(name,__file__.text) == 0
                               /*      ||strcmp(name,__stdc__.text) == 0 */
                                       ||strcmp(name,__date__.text) == 0
                                       ||strcmp(name,__time__.text) == 0))
                                 mccwarning(MCCPPNOUNDEF,nexttok);
                               else
                                {
                                 id = (tokenp) ustrRemText(mccppsym,nexttok->text);
                                 if (id)
                                   {
                                    mcctokfreelist(id->value.pval);
                                    id->value.pval = NULL;
                                    mcctokfreelist(id->right);
                                    id->right = NULL;           
                                    mcctokfree(id);
                                   }
                                }
                             }
                         }
                    }
                  else if (strcmp(keytok->text,"ifdef") == 0)
                    {                                    
                      if (curif == IFTHEN)
                         {               
                          curif = (nexttok && 
                                   (nexttok->ttype == ID 
                                          || nexttok->ttype == KEY)
                                  && ustrId(mccppsym,nexttok->text)) ? IFTHEN : IFELSE;
                         }
                       else
                         curif = IFNEVER;                                
      if (mccppiflevel >= ANSINESTPPIF)
                          mccerr(MCCPPIFOFLOW,keytok);
                        else
                          {
   mccppifnest[mccppiflevel] = curif;
   mccppifnestSloc[mccppiflevel] = keytok->sourceLoc;
                          }
      mccppiflevel++;
                    }
                  else if (strcmp(keytok->text,"ifndef") == 0)
                    {                                    
                      if (curif == IFTHEN)
                         {               
                          curif = (nexttok && (nexttok->ttype == ID
                                   || nexttok->ttype == KEY)
                                  && ! ustrId(mccppsym,nexttok->text)) ? IFTHEN : IFELSE;
                         }
                       else
                         curif = IFNEVER;                                
      if (mccppiflevel >= ANSINESTPPIF)
                          mccerr(MCCPPIFOFLOW,keytok);
                        else
                          {
   mccppifnest[mccppiflevel] = curif;
   mccppifnestSloc[mccppiflevel] = keytok->sourceLoc;
                          }
      mccppiflevel++;
                    }
                  else if (strcmp(keytok->text,"else") == 0)
                    {                  
      if (mccppiflevel)
                        {            
 if (mccppiflevel < ANSINESTPPIF
     && mccppifnest[curiflevel] != IFNEVER)
    mccppifnest[curiflevel] = !mccppifnest[curiflevel];
                        }
                       else
                        mccerr(MCCPPIFUFLOW,keytok);
                    }
                  else if (strcmp(keytok->text,"endif") == 0)
                    {                                             
      if (mccppiflevel > 0)
                         {
                           int forcenote = FALSE;
                           mccppiflevel--;
                           if (mccppifnestSloc[mccppiflevel].file !=
                                 keytok->sourceLoc.file)
                             {
                                mccwarning(MCCPPIFWRFILE,keytok);
                                forcenote = TRUE;
                              }
                           if (mccoption[MCCOPTLISTING] && mcclistoption['#']
                                || forcenote)
                              {
                               MCCWRITE(MCCLISTF,
"\
NOTE: End of #if/#ifdef/#ifndef ... #endif which began at %d:%d.\n",
  mccppifnestSloc[mccppiflevel].file,
  mccppifnestSloc[mccppiflevel].line);
                               MCCWRITE(MCCLISTF,"note: nestlevel=%d\n",
                                   mccppiflevel);
                              }
                         }
                       else
                         mccerr(MCCPPIFUFLOW,keytok);
                    }
                  else if (strcmp(keytok->text,"line") == 0)
                    {
                      if (nexttok && nexttok->ttype == ILIT)
                         mccsrc->lineno = nexttok->value.ival;  
                    }
                  else if (strcmp(keytok->text,"error") == 0)
                    {
                      char message[500];
                      char *save;
                      tokenp tok,tp;
                      tok = (nexttok) ? nexttok : keytok;
                      save = tok->text;
                      message[0] = 0;
                      for (tp = nexttok; tp ; tp = tp->right)
                         if (tp->text)
                           {
                            strcat(message,tp->text);
                            strcat(message," ");
                           }
                       tok->text = message;
                      mccerr(MCCERRANY,tok);
                      tok->text = save;
                    }
                  else
                     mccerr(MCCBADPPKEY,keytok);
                 }
                else
                 {
                  mccerr(MCCBADPPKEY,keytok);
                 }               
              } 
             mcctokfree(keytok);
             mcctokfreelist(toklist);
             mcctokfree(flagtok);
           }
       }
     else if (curif == IFTHEN
              && tok && (tok->ttype == ID || tok->ttype == KEY)
              && (macname = (tokenp) ustrId(mccppsym,tok->text)))
        {
#         if DEBUG && 1                                               
            if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
              MCCWRITE(MCCLISTF,
                   "got invocation of macro '%s' at column %d!\n",
                       tok->text,tok->sourceLoc.column);
#         endif          
          if (macname == &__file__)
             strcpy(__file__text,mccsrc->pathname);
          else if (macname == &__line__)
             sprintf(__line__text,"%d",mccsrc->lineno);
                                   
          errors = 0;
          firstarg = NULL;

          if (macname->right)  /* macro that has args */
             {
              nexttok = mccpptok(recursive+1);
              if (nexttok && nexttok->ttype == PUNCT 
                     && nexttok->value.ival == LPAREN
                     && (mccppoption['b']  
                         || nexttok->sourceLoc.column == 
                             tok->sourceLoc.column + strlen(tok->text)))
                { /* leftparen immediately follows name */
                  int parenlevel;                           
                  tokenp lastarg;
                  tokenp lasttok; 
                  int    curcolumn;
                  int    tempcolumn;
                  int    ttype;                          
                  int    value;

                  mcctokfree(nexttok);
                  for (parenlevel = 0, argno = 0, 
                           lastarg = NULL, lasttok = NULL;
       (nexttok = mccpptok(recursive+1)) && nexttok->ttype != EOFILE;
      )
                    {     
                      ttype = nexttok->ttype;
                      value = nexttok->value.ival;                          
#if 1
                      nexttok->sourceLoc.file = 
                        nexttok->sourceLoc.line = 0;
#endif
                      if (parenlevel <= 0 && ttype == PUNCT
                            &&   (value == RPAREN || value == COMMA))
                        {                                 
                          argno++; 
                          mcctokfree(nexttok);
                          /* fix for mccbug7, handle empty argument */
                          if (lasttok == NULL)
                            {
                              nexttok = (tokenp) mcctokcopy(NULL);
                              if (lastarg)
                                  lastarg->left = nexttok;
                                else
                                  firstarg = nexttok;
                              lastarg = nexttok;
                            }
#                         if DEBUG && 1                                               
                             if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
                               {
                                 MCCWRITE(MCCLISTF,
                                  "finished argument %d: ",argno);
                                 for (nexttok = lastarg;
                                      nexttok;
                                      nexttok = nexttok->right)
                                   MCCWRITE(MCCLISTF,
                                    "%s ",(nexttok->text) ? 
                                       nexttok->text : "***EMPTY***");
                                 MCCWRITE(MCCLISTF,"\n");
                               }
#                         endif
                          lasttok = NULL; /* end of current argument */
                          if (value == RPAREN)
                             break;
                        }
                      else
                        {     
                         if (ttype == PUNCT)
                           {                                     
                            if (value == LPAREN)
                               parenlevel++;
                            else if (value == RPAREN)
                              {
                                if (parenlevel)
                                   parenlevel--;
                                else
                                   break;
                              }
                           }
                         tempcolumn = nexttok->sourceLoc.column; 
                         if (lasttok) 
                            {  /* add to existing argument */
                              lasttok->right = nexttok;
                              nexttok->left = lasttok;
                             /* make column = number of preceeding blanks */
                              nexttok->sourceLoc.column -= curcolumn;
                            }
                           else  
                            {  /* first in a new argument */
                             nexttok->sourceLoc.column = 0;
                             if (lastarg)
                                lastarg->left = nexttok;
                              else
#ifdef mlbfix
                                firstarg = nexttok;
#else
                                macnest[curlevel].curargs = nexttok;
#endif
                             lastarg = nexttok;       
                             nexttok->left = NULL;
                            }  
                         curcolumn = tempcolumn + strlen(nexttok->text);
                         lasttok = nexttok;
                        }          
                    }
                  if (nexttok && nexttok->ttype == EOFILE)
                    {
                     mccerr(MCCPPARGEOF,tok);
                    }
#                 if DEBUG && 1                                               
                     if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
                       MCCWRITE(MCCLISTF,"got %d actual args\n",argno);
#                 endif       
                  /* validate same number of args as formals */
                  for (lastarg = macname->right;
                       lastarg;
                       lastarg = lastarg->right)
                    argno--;
                  if (argno)                             
                    {  /* argument number mismatch, dont expand */
                     tokenp temp,next;
                     mccerr(MCCPPARGNO,tok);        
                     if (macnestlevel > 0)
                           macnestlevel--;
                     for (next = macnest[macnestlevel].curargs;
                          next;
                          next = temp)
                       {
                         temp = next->left;
                         mcctokfreelist(temp);
                       }                                     
                    }
                  nexttok = NULL; /* ?? */
                }
               else
                {   /* no paren immediately following name */
#                if DEBUG && 1                                               
                  if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
                    MCCWRITE(MCCLISTF,
                      " mac with args not followed immediately by paren.\n");
#                endif          
                 if (nexttok && !(nexttok->ttype == PUNCT 
                                  && nexttok->value.ival == LPAREN))
                    mccwarning(MCCPPMISSARGS,tok);
                 if (macnestlevel > 0) 
                      macnestlevel--; 
                 if (nexttok)
                   mccpushtok(nexttok);
                 got_one = TRUE;  /* send the name on as a token */
                }
             }
#ifdef mlbfix
          if (!got_one)
            {
             curlevel = macnestlevel++;
             macnest[curlevel].macname = macname;
             macnest[curlevel].currepl = (tokenp) macname->value.pval;
             macnest[curlevel].curargs = firstarg;
             macnest[curlevel].sourceLoc =
                      macnest[curlevel].startLoc = tok->sourceLoc;
             macnest[curlevel].glue_left = NULL;
            }
#endif
        }
     else          
       if (tok)
           got_one = TRUE;

     if (curif != IFTHEN)
        got_one = FALSE;
       
     if (!got_one && tok)
       mcctokfree(tok);
     if (tok && dolit)
       {     
        if (littok)
           {
            char *newtext;
            int nblanks;
            char *sp,*dp;
            if (tok->sourceLoc.line == litcurline
                  && tok->sourceLoc.column >= litcurcol)
               nblanks = tok->sourceLoc.column - litcurcol;
             else
               { 
                 nblanks = 1;
               }
            litlen += nblanks + strlen(tok->text);
            newtext = malloc(litlen+1);
            strcpy(newtext,littok->text);
            for (sp = littok->text, dp = newtext;
                 *sp; )
               *dp++ = *sp++;
            while (nblanks--)
               *dp++ = ' ';
            litcurline = tok->sourceLoc.line;
            litcurcol = tok->sourceLoc.column;
            for (sp = tok->text;
                 *sp; )
              {
               *dp++ = *sp++;
               litcurcol++;
              }
            *dp = 0;
            free(littok->text);
            littok->text = newtext; 
            mcctokfree(tok);
           }
          else
           {
            tok->ttype = SLIT;
            littok = tok;
            litcurline = tok->sourceLoc.line;
            litlen = strlen(tok->text);
            litcurcol = tok->sourceLoc.column + litlen;
           }
        if (macnestlevel != litmacnestlevel
             || (macnest[macnestlevel-1].curargtok == NULL))
          {
           tok = littok;
           got_one = TRUE;
           dolit = FALSE;
          }
         else
          {
           tok = NULL;
           got_one = FALSE;
          }
       }       
   } while (!got_one);

if (tok)
  {
   file = tok->sourceLoc.file;
   line = tok->sourceLoc.line;
   column = tok->sourceLoc.column;
#  if DEBUG && 1                                               
    if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
       MCCWRITE(MCCLISTF,"returning token at %08x, text='%s' %d:%d:%d\n",tok,
          (tok->text) ? tok->text : (tok->ttype == MARKER) ? "MARKER" : "NONE",
          tok->sourceLoc.file,tok->sourceLoc.line,tok->sourceLoc.column);
#  endif
  }                               
 else                                                                           
  {
#  if DEBUG && 1                                               
    if (mccoption[MCCOPTTRACE] && mcctrcoption['p'])
     MCCWRITE(MCCLISTF,"no tok to return, why are we here?\n");
#  endif
    file = -1;
    line = -1;
    column = -1;
  }

return tok;
}                           

tokenp mccendline(line)
 int line;
{
 tokenp firsttok,prevtok,newtok;
            
 firsttok = NULL;           
 prevtok = NULL;
 while( (newtok = mcctok(FALSE)) /* && newtok->sourceLoc.line == line */)
    {
     if (firsttok == NULL)
       firsttok = newtok;
      else
       prevtok->right = newtok;
     newtok->left = prevtok;
     newtok->right = NULL;
     prevtok = newtok;
    }    
 if (newtok)             
   mccpushtok(newtok);
 return firsttok;
}


void mccppfree(id)
  long id;
{
  tokenp tok;
  tokenp temp;

  if (id && id > 256) 
    {
     tok = (tokenp) id;  
     temp = (tokenp) tok->value.pval;
     if(temp)
       mcctokfreelist(temp);
     mcctokfreelist(tok);
    }
return;
} 

void mccppdump(id)
  long id;          
{
  tokenp tok;
  tokenp temp;
  ptr fd = (ptr) MCCLISTF;

  if (id && id > 256) 
    {
     tok = (tokenp) id;  
     fprintf(fd,"%03d:%05d:%03d #define %s",
         tok->sourceLoc.file,tok->sourceLoc.line,tok->sourceLoc.column,
         tok->text);
     if (tok->right)
       {
         fputc('(',fd);
         for (temp = tok->right; 
              temp;
              temp = temp->right)
           {
            fputs(temp->text,fd);
            if (temp->right)
              fputc(',',fd);
             else
              fputc(')',fd);
           }                
       }
     for (temp = (tokenp) tok->value.pval;
          temp;
          temp = temp->right)
       {
         fputc(' ',fd);
         if (temp->ttype == SLIT)
           fputc('"',fd);
         fputs(temp->text,fd);
         if (temp->ttype == SLIT)
           fputc('"',fd);
       }
     fputc('\n',fd);
    }
return;
}

int mccppspecial( tok )
 tokenp tok;
{
 tokenp l,r;
                
 l = tok->left;
 r = tok->right;
 if (l)
   l->right = NULL;
 if (r)
   r->left = NULL;

}

tokenp mccppexp( exp )
  tokenp exp;
{
  static int inited = FALSE;
  static char prec[MCCMAXRESERVED];
  int precedence;
  tokenp subexp;
  tokenp cur;
  tokenp tail;

  if (!inited)
    {
     for (precedence = 0; precedence < sizeof(prec); precedence++)
        prec[precedence] = 0;
     prec[NOT] = prec[ANEG] = prec[BNEG] = 3;
     prec[MULT] = prec[DIVIDE] = prec[MODULUS] = 4;
     prec[PLUS] = prec[MINUS] = 5;
     prec[LSHIFT] = prec[RSHIFT] = 6;
     prec[LESS] = prec[LESSEQ] = prec[GREATER] = prec[GREATEREQ] = 7;
     prec[EQUAL] = prec[NOTEQUAL] = 8;
     prec[ADDRESS] = prec[BAND] = 9;
     prec[XOR] = 10;
     prec[BOR] = 11;
     prec[LAND] = 12;
     prec[LOR] = 13;
     inited = TRUE;
    }

#ifdef DEBUG
  if (mccoption[MCCOPTTRACE] && mcctrcoption['e'])
    {
      MCCWRITE(MCCLISTF,"mccppexp: ");      
      for (cur = exp; cur ; cur = cur->right)
MCCWRITE(MCCLISTF," %08x,%s,%d,%d ",cur,cur->text,cur->ttype,cur->value.ival);
      MCCWRITE(MCCLISTF,"\n");
    }
#endif

  for (cur = exp; cur ; cur = cur->right)
    {
     /* apparently the preprocessor ought to treat undefined names as ilit 0 */
     if (cur->ttype == ID || cur->ttype == KEY)
{
 cur->ttype = ILIT;
 cur->value.ival = 0;
 if (mccoption[MCCOPTLISTING])
     MCCWRITE(MCCLISTF,"NOTE: treating undefined identifier '%s' as FALSE.\n",
                  (cur->text) ? cur->text : "**EMPTY**");
} 

     if (cur->ttype == PUNCT && cur->value.ival == LPAREN)
{
 int plevel = 1;
#ifdef DEBUG
  if (mccoption[MCCOPTTRACE] && mcctrcoption['e'])
    MCCWRITE(MCCLISTF,"found a lparen @ %08x, search for rparen\n",cur);      
#endif
 /* cur now points to a left paren */
 for (tail = cur->right; tail && plevel > 0 ; tail = tail->right)
    {
      if (tail->ttype == PUNCT )
{
 if (tail->value.ival == LPAREN)
   plevel++;
 else if (tail->value.ival == RPAREN)
   plevel--;
}             
      if (plevel == 0)
 {
#ifdef DEBUG
  if (mccoption[MCCOPTTRACE] && mcctrcoption['e'])
       MCCWRITE(MCCLISTF,"found matching rparen @ %08x\n",tail);      
#endif
   /* got a subexpression, cut out of exp and evaluate */
   /* tail points at the right paren */
   subexp = cur->right;
   subexp->left = NULL;
   tail->left->right = NULL;  /* terminate the sub exp */
   /* skip over and free the right paren */
   if (tail->right)
     {
      tail = tail->right;
      mcctokfree(tail->left);
      tail->left = NULL;
     }
    else
     {
      mcctokfree(tail);
      tail = NULL;
     }
   if (subexp)
       {
 if (subexp->right)
    subexp = mccppexp(subexp);
       }
      else
{
 mccerr(MCCNULLSUBEXP,cur);
 return(exp);
}
   /* glue the reduced expression on to list */
   /* discarding the left paren */
   if (cur->left)
      cur->left->right = subexp;
    else
      exp = subexp;
   subexp->left = cur->left;
   mcctokfree(cur);
   /* find the end of the sub expression */
   while(subexp->right)
      subexp = subexp->right;
   /* and glue the tail on the end */
   subexp->right = tail;
   if (tail)
      tail->left = subexp;
   /* point cur at end of result for continued () scanning */
   cur = subexp;
   break; /* terminate search for end of subexp. */
 } /* if plevel == 0 found matching rparen */
    } /* for search for rparen */ 
} /* if LPAREN */
    }  /* for search through tokens */
 /* all subexpressions are reduced, evaluate operators in precedence order */
 for ( precedence = 1; precedence <= 15; precedence++)
  { 
    int ltor; 
    cur = exp;
    if (precedence == 2 || precedence == 13 || precedence == 14)
      {
       ltor = FALSE;
       while ( cur && cur->right )
   cur = cur->right;
      }
     else
       ltor = TRUE;
 
    for (; cur ; cur = (ltor) ? cur->right : cur->left)
      if ((cur->ttype == PUNCT || cur->ttype == EOP)
    && prec[cur->value.ival] == precedence)
{
 int binop = TRUE, preop = FALSE, postop = FALSE;
 tokenp left,right;
 int  val,lval,rval;
 int  operator;     

 operator = cur->value.ival;
 left = cur->left;
 right = cur->right;
 lval = left->value.ival;
 rval = right->value.ival;
 if (operator == ADDRESS && left->ttype != EOP && left->ttype != PUNCT)
             cur->value.ival = operator = BAND;

         cur->ttype = ILIT;
         switch(operator)
          {
            
            case NOT:  
            case ANEG: 
            case BNEG:
                 if (right->ttype != ILIT)
                   {
                    mccerr(MCCBADOPERTYPE,cur);
                    cur->value.ival = FALSE;
                   }
                  else
                   {
                    cur->value.ival = (operator == NOT) ? !rval
                                    : (operator == ANEG) ? -rval
                                    : /* (operator == BNEG) ? */ ~rval;
                   } 
                 preop = TRUE ;
                 binop = FALSE;
                 break;
            case MULT:    val = lval * rval; break;
            case DIVIDE:  val = lval / rval; break;
            case MODULUS: val = lval % rval; break;
            case PLUS:    val = lval + rval; break;
            case MINUS:   val = lval - rval; break;
            case LSHIFT:  val = lval << rval; break;
            case RSHIFT:  val = lval >> rval; break;
            case LESS:    val = lval < rval; break;
            case LESSEQ:  val = lval <= rval; break;
            case GREATER: val = lval > rval; break;
            case GREATEREQ: val = lval >= rval; break;
            case EQUAL:   val = lval == rval; break;
            case NOTEQUAL:  val = lval != rval; break;
            case BAND:    val = lval & rval; break;
            case XOR:     val = lval ^ rval; break;
            case BOR:     val = lval | rval; break;
            case LAND:    val = lval && rval; break;
            case LOR:     val = lval || rval; break;
            default:
              mccerr(MCCNOIMP,cur);
#if DEBUG
              if (mccoption[MCCOPTTRACE] && mcctrcoption['e'])
               {
                  MCCWRITE(MCCLISTF,"binop %s,%d,%d  %s,%d,%d  %s,%d,%d = %d\n",
                          (left->ttype == ILIT) ? "ILIT" : left->text,
                                  left->ttype,left->value.ival,
                          cur->text,cur->ttype,cur->value.ival,
                          (right->ttype == ILIT) ? "ILIT" : right->text,
                             right->ttype,right->value.ival,val);
               }
#endif
              return(exp);
          }
#if DEBUG
        if (mccoption[MCCOPTTRACE] && mcctrcoption['e'])
          {
           if (binop)
             MCCWRITE(MCCLISTF,"binop %s,%d,%d  %s,%d,%d  %s,%d,%d = %d\n",
                     (left->ttype == ILIT) ? "ILIT" : left->text,
                           left->ttype,left->value.ival,
                     cur->text,cur->ttype,cur->value.ival,
                     (right->ttype == ILIT) ? "ILIT" : right->text,
                         right->ttype,right->value.ival,val);
           else if (preop)
             MCCWRITE(MCCLISTF,"preop %s,%d,%d  %s,%d,%d = %d\n",
                     cur->text,cur->ttype,cur->value.ival,
                     (right->ttype == ILIT) ? "ILIT" : right->text,
                          right->ttype,right->value.ival,cur->value.ival);
           else if (postop)
             MCCWRITE(MCCLISTF,"postop %s,%d,%d  %s,%d,%d = %d\n",
                     (left->ttype == ILIT) ? "ILIT" : left->text,
                         left->ttype,left->value.ival,
                     cur->text,cur->ttype,cur->value.ival,cur->value.ival);
           else MCCWRITE(MCCLISTF,"noop %s,%d,%d\n",
                     cur->text,cur->ttype,cur->value.ival);
          }
#endif
         if (binop)
           {
            if (right->ttype != ILIT || left->ttype != ILIT)
              {
               mccerr(MCCBADOPERTYPE,cur);
               cur->value.ival = FALSE;
              }
            else
              {
               cur->value.ival = val;
               cur->ttype = ILIT;
              }
            /* remove and destroy left operand */
            if (left->left)
               left->left->right = cur;
              else
               exp = cur;
            cur->left = left->left;
            mcctokfree(left);
            /* remove and destroy right operand */
            if (right->right)
               right->right->left = cur;
            cur->right = right->right;
            mcctokfree(right);
           }
         else if (preop)
           {
            /* remove and destroy right operand */
            if (right->right)
               right->right->left = cur;
            cur->right = right->right;
            mcctokfree(right);
           }
         else if (postop)
           {
            /* remove and destroy left operand */
            if (left->left)
               left->left->right = cur;
              else
               exp = cur;
            cur->left = left->left;
            mcctokfree(left);
           }
        }
  }

 return exp;
}

