#include "mcc.h"
#include <ctype.h>
#include <string.h>

/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME:     mcc i/o services                                        */
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
/* PURPOSE: perform file io for mcc compiler                         */
/*                                                                   */
/*  Internal functions in this module                                */
/* INCLUDE .h                                                        */
/* GLOBAL DECLARATIONS:                                              */
/* EXTERNAL INTERFACES:                                              */
/* HISTORY:                                                          */
/*                                                                   */
/* NOTES:                                                            */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/
 void mcclistline(char *p);
 void mccnewline(char *p);
 void mccreadsrc __ARGS((sourcep src, char* file, int line ));                                     
 static char *makepath __ARGS((char *dir, char *file, char *buf));
 static char *makepath(dir,file,buf)
   char *dir;
   char *file;
   char buf[];
{
 char *s,*d;
 d = buf;
 if (*dir) 
   {
     for (s = dir;
          *s; 
          )
        *d++ = *s++;
     *d++ = MCCFILSEP;
   }
 for (s = file;
      *s;
     )
    *d++ = *s++;
 *d = 0;
 return buf;
}
/********************************************************** mccopensrc */
/*-------------------------------------------------------------------*/
/* NAME: mccopensrc                                                  */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: open a new source file on top of stack                   */
/* USAGE:                                                            */
 sourcep mccopensrc(pathname,bufsize,searchtype)
/* PARAMETERS:                                                       */
 char *pathname;
 int  bufsize;  /* 0 for default bufsize, -1 for no buffer           */
 int searchtype; /* MCCNOSEARCH, MCCHOMESEARCH, MCCSYSSEARCH         */
{
/* RETURNS:  pointer to source control block                         */
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
 sourcep  src;                        
 char     *path;               
 char     *newpath;
 int      newpathlen;
 int      pathnamelen;
 char     *dir;
 int      search; 
 FILE     *fd;                                                        
 char     pathtemp[MCCMAXPATHLEN];
 char     *p;
 int      absolute;               
 char     *mode = "r";
 int      depth; 
 int      dotrace;

#ifdef DEBUG
 dotrace = mccoption[MCCOPTTRACE] && mcctrcoption['i'];
#endif
        
 pathnamelen = strlen(pathname);
 fd = NULL;
 if (pathname)
   {          
       if (mccppoption['1'])
          for (src = mccoldsources; src; src = src->right )
            if (src->pathname && strcmp(pathname,src->inpath) == 0)
              { 
                if (mccoption[MCCOPTLISTING])
                   MCCWRITE(MCCLISTF,mccerrmsg[MCCSRCREPEAT],1,pathname);
                return NULL;
              }     
       for (src = mccsrc; src; src = src->right )
         if (src->pathname && strcmp(pathname,src->inpath) == 0)
           {
             if (mccoption[MCCOPTLISTING])
                MCCWRITE(MCCLISTF,mccerrmsg[MCCSRCLOOP],3,
                   pathname,mccsrc->pathname,mccsrc->lineno); 
             return NULL;
           }                    
       absolute = !(isalpha(*pathname) || *pathname == '_');  
       if (absolute || searchtype == MCCNOSEARCH)
         {
          path = pathname;
#ifdef DEBUG
           if (dotrace)
              MCCWRITE(MCCLISTF,"attempt to open pathname '%s'\n",1,path);
#endif
          if (strcmp(path,"-") == 0)
             fd = stdin;
           else
             fd = fopen(path,mode);
         }
        else
          {
           path = makepath( (searchtype == MCCSYSSEARCH)
                                    ?  mccsysidir 
                                    :  mcchomedir , /* else MCCHOMESEARCH */
                                 pathname,pathtemp);
#ifdef DEBUG
           if (dotrace)
              MCCWRITE(MCCLISTF,"attempt to open pathname '%s'\n",1,path);
#endif
           fd = fopen(path,mode);
           if (!fd)
             {
              for (search = 0; 
                     search < mccnidirs;
                    search++)             
                 {
                  path = makepath(mccidirs[search],pathname,pathtemp);
#ifdef DEBUG
           if (dotrace)
              MCCWRITE(MCCLISTF,"attempt to open pathname '%s'\n",1,path);
#endif
                  if ((fd = fopen(path,mode)) != NULL)
                     break;
                 } 
             }
           if (!fd)
             {
              if (searchtype == MCCSYSSEARCH)
                 path = makepath(mcchomedir,pathname,pathtemp); 
                else
                 path = makepath(mccsysidir,pathname,pathtemp);
#ifdef DEBUG
           if (dotrace)
              MCCWRITE(MCCLISTF,"attempt to open pathname '%s'\n",1,path);
#endif
               fd = fopen(path,mode);
             }
          }
    if (fd == NULL)
      {
       if (searchtype != MCCNOSEARCH)
        {
         MCCWRITE(MCCERRF,mccerrmsg[MCCOINCF],1,pathname); 
         mccerrcount++;
        }
#if 0
       else
         MCCWRITE(MCCERRF,mccerrmsg[MCCEOSRCF],1,pathname); 
#endif
       return(NULL);
      }
    newpathlen = strlen(path);
    newpath = (char *) malloc(newpathlen+1);
    if (newpath == NULL)
      {
       MCCERR(MCCENOMEM)
       if (fd != stdin)
          fclose(fd);
       return NULL;
      }
     else
       strcpy(newpath,path);
   }
   else
    newpath = NULL;
 src = (sourcep) malloc(sizeof(*src));
 if (src == NULL) 
   {   
    if (fd && fd != stdin)
      fclose(fd);
    free(newpath);           
    MCCERR(MCCENOMEM)
    return NULL;
   }          
 src->sig = SIGSRC;
 src->refcnt = 0;
 if (bufsize == 0)
   bufsize = MCCREADSIZE + MCCALMOSTEMPTY;
 if (bufsize > 0)
   {
    src->buffer = (char *) malloc(bufsize+1);
    if (src->buffer == NULL)
    if (src == NULL) 
       {   
        if (fd && fd != stdin)
          fclose(fd);
        if (newpath)
          free(newpath);           
        MCCERR(MCCENOMEM)
        return NULL;
       }          
   }             
 else
   src->buffer = NULL;
 src->pathname = newpath;
/* get a pointer to input portion of final pathname */
 src->inpath = newpath + (newpathlen-pathnamelen); 
 src->fd = fd;
 src->id = mccfileid++; 
 mcctotfiles++;
 src->bufsize = bufsize;
 src->curchar = src->linestart = src->end = src->buffer + bufsize;
 src->lineno = 1; 
 src->eof = FALSE;
 src->tokq = NULL;
 src->incdepth = (mccsrc == NULL) ? 0 : mccsrc->incdepth + 1;
 src->flags = (searchtype == MCCSYSSEARCH) ? MCCSYSH
               : (searchtype == MCCHOMESEARCH) ? MCCUSERH
               : 0;

 src->right = mccsrc;      
 if (src->right)
    src->right->left = src;
 src->left = NULL;
 mccsrc = src;              
 if (((mccsrc->flags & MCCSYSH) && !mcclistoption['s'])
     || ((mccsrc->flags & MCCUSERH) && !mcclistoption['u']))
      mccsrc->flags |= MCCNOLIST;

#if 0
 if (mccoption[MCCOPTLISTING] && src->pathname && src->id)
   {
     if (mccsrc->flags & MCCNOLIST)
       MCCWRITE(MCCLISTF,
           "NOTE: listing of file '%s' suppressed by -l~%c option.\n",2,
               src->pathname,(mccsrc->flags & MCCUSERH) ? 'u' : 's');
      else
       {
   MCCWRITE(MCCLISTF,
"           /---------------------- begin file: %s ---------------------\n",
           1,src->pathname);
   MCCWRITE(MCCLISTF,1,
"          /\n");
       }
    }
#endif 
#if 0
 if (src->buffer)     
   {
     mccreadsrc(src,__FILE__,__LINE__);
     if (searchtype != MCCNOSEARCH)
        mcclistline(src->curchar);
   }
#endif

  
 return(src);
}                                            

/********************************************************** mccclosesrc */
/*-------------------------------------------------------------------*/
/* NAME: mccclosesrc                                                 */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: close a source file, and pop it off stack (if top)       */
/* USAGE:                                                            */
 void mccclosesrc(src)
/* PARAMETERS:                                                       */
 sourcep src;  /* if NULL then close (and pop) the top file on stack */
{
/* RETURNS: void                                                     */
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
 boolean wasfile;
 sourcep nextsrc;
 sourcep mainsrc;
 int     filenum;

 if (src == NULL)
   {
    src = mccsrc;
    mccsrc = src->right;
    if (mccsrc)
       mccsrc->left = NULL;
   }             
 wasfile = ( src && src->pathname);
 if (src == NULL)
    return;
 if (mccoption[MCCOPTLISTING] && (wasfile || src == mccdefsrc) 
     && !(src->flags & MCCNOLIST))
   {
   MCCWRITE(MCCLISTF,
"          \\\n",0);
   MCCWRITE(MCCLISTF,
"           \\------------------- %s %s ----------------\n",2,
              (src == mccdefsrc) ? "end command line switches"  
                                 : "end file:",
              (src == mccdefsrc) ? ""  
                                 :  src->pathname);
    }
 if (src->buffer)
    free(src->buffer);
 if (src->fd && src->fd != stdin)
    fclose(src->fd);
 if (wasfile)
   {
    src->right = mccoldsources;
    if (mccoldsources)
       mccoldsources->left = src;
    mccoldsources = src;         
    src->left = NULL;
  }
 if (mccsrc == NULL) 
    {
     /* source stack is empty, just finished .c file */
     /* search the list of files and print them in order. */
     if (mccoption[MCCOPTLISTING])
       {
        for (filenum = 0; filenum < mccfileid; filenum++)
         { 
          for (src = mccoldsources; src; src = src->right)
             if (src->id == filenum)
                {
                 if (src->pathname)
                   {
                     MCCWRITE(MCCLISTF,mccerrmsg[MCCFILELIST],6,
                       src->id,
                       src->incdepth * 2,
/*2345678901234567890123456789012345678901234567890123456789012345678901234567890*/
/*        1         2         3         4         5         6         7         8*/
"                                                                                 "
                          + (80 - (src->incdepth * 2)),
                       src->pathname,src->lineno-1);
                   }
                 break;
               }
         }           
       }
     /* then free all the old sources */
     for (src = mccoldsources; src ; src = nextsrc)
       {
        nextsrc = src->right;
        if (src->pathname)
          {
            free(src->pathname);
          }
        free(src);
       }          
     mccoldsources = NULL;
    }

 if (wasfile)
    if (mccoption[MCCOPTLISTING])
      {
        if (mccsrc && mccsrc->pathname
              && !(src->flags & MCCNOLIST))
          MCCWRITE(MCCLISTF,
"                                  resume file: '%s' \n",1,
              mccsrc->pathname);
      }

 return; 
}

/********************************************************** mccreadsrc */
/*-------------------------------------------------------------------*/
/* NAME: mccreadsrc                                                  */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: fill the current file's buffer with more data            */
/* USAGE:                                                            */
 void mccreadsrc( src,file,line )
/* PARAMETERS:                                                       */
sourcep src;  /* if null then read to top of stack */
char *file;  /* __FILE__ of caller */
int  line;   /* __LINE__ of caller */
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
int cursize;                          
int ntoread;
int roomleft;
int nread;
int curcolumn;
int newcurcolumn;
int firstread = FALSE;
int dotrace;
/*-------------------------------------------------------------------*/
if (src == NULL)
   src = mccsrc;
dotrace = (mccoption[MCCOPTTRACE] && mcctrcoption['r']);
if (src->lineno <= 1)
  {
    firstread = TRUE;
    if (mccoption[MCCOPTLISTING] 
            && (src->pathname || src == mccdefsrc))
      {
        if (mccsrc->flags & MCCNOLIST)
          MCCWRITE(MCCLISTF,
           "NOTE: listing of file '%s' suppressed by -l~%c option.\n",2,
               src->pathname,(mccsrc->flags & MCCUSERH) ? 'u' : 's');
         else
          {
      MCCWRITE(MCCLISTF,
"           /----------------- %s %s ----------------\n",2,
              (src == mccdefsrc) ? "begin command line switches"  
                                 : "begin file:",
              (src == mccdefsrc) ? ""  
                                 :  src->pathname);
      MCCWRITE(MCCLISTF,
"          /\n",0);
          }
      }
  }
if (!(src->eof || src->fd == NULL))
  {

   cursize = src->end - src->curchar;
   curcolumn = src->curchar - src->linestart + 1;

   memcpy(src->buffer,src->curchar,cursize);
   src->curchar = src->buffer;                  
   src->linestart = src->curchar - (curcolumn - 1);
#ifdef DEBUG
   newcurcolumn = src->curchar - src->linestart + 1;
   if (newcurcolumn != curcolumn)
      MCCWRITE(MCCLISTF,"INTERNAL ERROR, linestart recalculation, %d %d \n",2,
        newcurcolumn,curcolumn);
#endif

   roomleft = src->bufsize - cursize - 1;
   if (roomleft > MCCREADSIZE)
      ntoread = MCCREADSIZE;
     else
      ntoread = roomleft;

   if (dotrace)
     {
      MCCTRCs("called from source file ",file) 
      MCCTRCd("called from source line ",line) 
      MCCTRCd("cursize =",cursize) 
      MCCTRCd("roomleft =",roomleft) 
      MCCTRCd("number of bytes to read =",ntoread) 
     }

   nread = fread(src->buffer+cursize,1,ntoread,src->fd);
   mcctotbytes += nread;
   src->end = src->buffer + cursize + nread;

   if (nread < ntoread)
     {
      if (dotrace)
         MCCTRC("read end of file");
      *src->end++ = MCCEOFCHAR;
      src->eof = TRUE;
     }
   *src->end = 0;         
   
   if (dotrace)
     {
      MCCTRCd("number of bytes read =",nread) 
#     if DEBUG
         src->buffer[cursize+ntoread+1] = '@';
         memdump(MCCLISTF,"CURRENT SOURCE BUFFER for file ",src->pathname,
              src->buffer,
              (mcctrcoption['b']) ? src->end - src->buffer : 128,1,2);
#     endif               
     }
  }
if (firstread)
  mcclistline(src->curchar);
                             
}


void mccnewline(p)
char *p;
 {                        
  sourcep src;
  int c;      
  src = mccsrc;
  if (src == NULL)
    return;
#if DEBUG
  if (p[-1] != '\n')
     MCCTRCx("somebody called newline but no newline char before ",p)
#endif
  src->curchar = p;
  src->linestart = p;
/*  MCCTRCx("new line @",p) */
  src->lineno++;     
  mcctotlines++;
  if (mccoption[MCCOPTLISTING])
     mcclistline(p);
 }

void mcclistline(p)
char *p;
 {           
  sourcep src;
  int c;      
  char marker;
  src = mccsrc;
  if (src == NULL)
    return;
  if (mcclistInPmode)
    {
     fputc('\n',MCCLISTF);
     mcclistInPmode = FALSE;
    }
  if (src->flags & MCCNOLIST)
     return;
  if (mccliston && mccoption[MCCOPTLISTING] && *p != MCCEOFCHAR)
    {
     if (mccppiflevel && mccppifnest[mccppiflevel-1] != IFTHEN)
        {
          marker = MCCELSEMARKER;
          if (!mcclistoption['e'])
            return;
        }
       else
        marker = ' ';
     if (*p == 0 && src->eof)  /* no text left in file */
        return;
     fprintf(MCCLISTF,"%03d:%05d%c",src->id,src->lineno,marker);
     do              
       {
         c = *p++;
         if (c)
           fputc(c,MCCLISTF);
       }
         while (c && c != '\n' && c != MCCEOFCHAR);
    }
 }

