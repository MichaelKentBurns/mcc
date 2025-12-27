/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME: mccmain.c                                                   */
/* DOCUMENT: none                                                    */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* TYPE:                                                             */
/* PRODUCT: mikes C compiler                                         */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo, IBM PC/RT, RS6000, Mac 26                        */
/* BRIDGED:  none                                                    */
/* SCRIPT:                                                           */
/* PURPOSE: initialize the compiler                                  */
/*                                                                   */
/*  Internal functions in this module                                */
#include <stdlib.h>                            
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
// #ifdef unix
#include <sys/types.h>
#include <sys/times.h>
// #endif
#include <time.h>
#include <setjmp.h>
#undef GREATER
#undef EQUAL
/* GLOBAL DECLARATIONS:                                              */
#include "mcc.h"
                                              
#include "mcccfg.h" 
/* EXTERNAL INTERFACES:                                              */
/* HISTORY:                                                          */
/*                                                                   */
/* NOTES:                                                            */
/* END                                                               */
/*-------------------------------------------------------------------*/
static jmp_buf crokbuf;
static int crokbufSet = FALSE;
void mcccroak(int);
void mccclosesrc(sourcep src);
int mccterm(char* pgmname);
/********************************************************** mccmain  */
/*-------------------------------------------------------------------*/
/* NAME: mccmain                                                     */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE:                                                          */
/* USAGE:                                                            */
 int main (int argc, char* argv[])
{
/* RETURNS:                                                          */
/*   0 if all ran ok, else errors occured                            */
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
int nfiles;  
ptr  arg;           
ptr  ext;      
ptr  fil;            
int  len;
char *ipath;
char argv2[200]; 
char temppath[200]; 
char listpath[200];     
char errpath[200];     
char pppath[200];     
sourcep origsrc;
sourcep testsrc;
int  docompile;
long start;
float cpuseconds;
long wallseconds;
#define TMSUNITSPERSECOND 60
#ifndef DOS
struct tms timesbuf;
#endif
char *pgmname = argv[0];
int ret;                               
FILE *savelist;                            
int startwarnings;
int starterrors;
int emitstats = TRUE;
int amcpp = FALSE;
/********************************************************** mccmain **/
     
/* setup default file descriptors */
     mccerrf = stderr;
     mcclistf = stdout;
     
     
     printf("You have entered %d arguments:\n", argc);

        for (int i = 0; i < argc; i++) {
            printf("%s\n", argv[i]);
        }
     
start = times(&timesbuf);  // keeping time
if (argc <= 1)
  {
   char **pp;
   pp = mccusage;
   while (*pp)
      puts(*pp++);
   return 0;
  }
if (strcmp(argv[0],"cpp") == 0)
  {
   ipath = argv[2];
   sprintf(argv2,"-i%s",ipath);
   argv[2] = argv2;
   mccoption[MCCOPTPPONLY] = '-';
#if 0
   emitstats = FALSE;
#endif
   amcpp = TRUE;
  }
if (strcmp(argv[0]+strlen(argv[0])-8,"mccdebug") != 0)
  {
#ifdef SIGINT
   signal(SIGINT,mcccroak);
#endif
#ifdef SIGQUIT
   signal(SIGQUIT,mcccroak);
#endif
#ifdef SIGILL
   signal(SIGILL,mcccroak);
#endif
#ifdef SIGFPE
   signal(SIGFPE,mcccroak);
#endif
#ifdef SIGBUS
   signal(SIGBUS,mcccroak);
#endif
#ifdef SIGSEGV
   signal(SIGSEGV,mcccroak);
#endif
  }

if (!mccinit(argc,argv)) {
   MCCRETURN(MCCINITERRS,1);
}

if (mccoption[MCCOPTVERBOSE]) 
 {
  int opt,subopt,optval,suboptval,i;
  char *secondary;
  int optset;

  MCCWRITE(MCCERRF,"Options summary: \n",0);
  MCCWRITE(MCCERRF,
"  - indicates option on, ~ indicates option off, . indicates option ignored\n",0);
  for (opt = 0; opt <= '~'; opt++)
    {
      optval = mccoption[opt];
      if (optval != ' ')
        {
          MCCWRITE(MCCERRF," %c%c",(optval) ? optval : '~',opt);
          secondary = NULL;
          switch(opt)
           {    
            case MCCOPTOUTPUT:   
              MCCWRITE(MCCERRF," %s",1,mccoutpath);
              break;
            case MCCOPTPPOUTPUT:   
              MCCWRITE(MCCERRF," %s",1,mccppopath);
              break;
            case MCCOPTERRFILE:
              MCCWRITE(MCCERRF," %s",1,mccerrpath);
              break;
#if 0
            case MCCOPTZSYSIDIR:
              MCCWRITE(MCCERRF," %s",1,mccsysidir);
              break;
#endif
            case MCCOPTTRACE: secondary = mcctrcoption; break;
            case MCCOPTLISTING: secondary = mcclistoption; break;
            case MCCOPTPARSE: secondary = mccparseoption; break;
            case MCCOPTOPTIM: secondary = mccoptimoption; break;
            case MCCOPTPPONLY:   secondary = mccppoption; break;
           }
           if (secondary)
            for (suboptval = 1,subopt = 0; 
                 subopt <= '~';
                 secondary++,subopt++)
               {
                if (*secondary != ' ')       
                  {
                   if (suboptval && !*secondary)
                     {
                      MCCWRITE(MCCERRF,"~",0);
                      suboptval = FALSE;
                     }
                    else if (!suboptval && *secondary) 
                     {
                      MCCWRITE(MCCERRF,"-",0);
                      suboptval = TRUE;
                     }
                   MCCWRITE(MCCERRF,"%c",1,subopt);
                  }
               } 
        }
    }
   MCCWRITE(MCCERRF," %s %s",2,MCCOPTSYSIDIR,mccsysidir);
   for (i = 0; i < mccnidirs; i++)
     MCCWRITE(MCCERRF," -%c %s",2,MCCOPTIDIR,mccidirs[i]);
   MCCWRITE(MCCERRF," %c%s",2,(mccoptansi) ? '-' : '~', &MCCOPTANSI[1]);
  MCCWRITE(MCCERRF,"\n",0);
 }   

for (argno = 1,nfiles = 0;
     argno <= argc; 
     argno++)
  {            
   docompile = TRUE;
   if (argno == argc)
     {
       if (nfiles == 0 && amcpp)
           arg = "-";
         else
           break;
     }
    else
      arg = argv[argno];
   if (arg != NULL && strlen(arg))
     {         
       savelist = mcclistf;
       for (ext = arg + strlen(arg); /* point ext to terminator */
            ext >= arg && *ext != MCCEXTSEP && *ext != MCCFILSEP;
            ext--)
           ;                  
       /* get name of directory (if any) that .c file is in */
       for (fil = ext; /* point fil to begin of file */
            fil >= arg && *fil != MCCFILSEP;
            fil--)
           ;     
       if (fil < arg)
          fil = arg;
       len = fil - arg;
       memcpy(mcchomedir,arg,len);
       mcchomedir[len] = (char) NULL;
       
       /* ext probably points to last seperator character, else begining of arg */
       if (ext < arg && strcmp(arg,"-") != 0)
         {
          strcpy(temppath,arg);       
          arg = temppath;
          ext = temppath + strlen(temppath);
          strcat(temppath,MCCEXTSRC); 
         }
       if (strcmp(ext,MCCEXTSRC) == 0 
           || strcmp(ext,MCCEXTPP) == 0 
           || strcmp(arg,"-") == 0)
         {     
           if (mccoption[MCCOPTLISTING])
             {                   
              if (mcclistpath == NULL)
                {                      
                 char *t;
                 for (t = ext; t > arg; )
                    if ( t[-1] != MCCFILSEP )
                      t--;
                     else
                      break;
                 strncpy(listpath,t,(ext-t));
                 listpath[ext-t] = 0; 
                 strcat(listpath,MCCEXTLST);
                 if ((mcclistf = fopen(listpath,"w")) == NULL)
                    {           
                     mcclistf = savelist;
                     MCCWRITE(MCCERRF,mccerrmsg[MCCEOLSTF],1,listpath);
                     mccerrcount++;
                     docompile = FALSE;
                    }
                }
             }
           if (mccoption[MCCOPTPPONLY] || mccoption[MCCOPTPPSTDOUT]
                    || mccppopath)
             {                   
                {                      
                 char *t;
                 if (mccppopath)
                    strcpy(pppath,mccppopath);
                  else
                    {
                     for (t = ext; t > arg; )
                        if ( t[-1] != MCCFILSEP )
                          t--;
                         else
                          break;
                     strncpy(pppath,t,(ext-t));
                     pppath[ext-t] = 0; 
                     strcat(pppath,MCCEXTPP);
                    }
                 if (mccoption[MCCOPTPPSTDOUT] ||
                       strcmp(pppath,"-") == 0)
                      mccppof = stdout;
                  else
                    if ((mccppof = fopen(pppath,"w")) == NULL)
                       {
                        MCCWRITE(MCCERRF,mccerrmsg[MCCEOPPF],1,pppath);
                        mccerrcount++;  
                        docompile = FALSE;
                       }
                }
             }
           if (mccoption[MCCOPTERRFILE] && !(mccerrpath && mccerrpath[0]))
             {                   
                {                      
                 char *t;
                 for (t = ext; t > arg; )
                    if ( t[-1] != MCCFILSEP )
                      t--;
                     else
                      break;
                 strncpy(errpath,t,(ext-t));
                 errpath[ext-t] = 0; 
                 strcat(errpath,MCCEXTERR);
                 if ((mccerrf = fopen(errpath,"w")) == NULL)
                    {
                     MCCWRITE(MCCERRF,mccerrmsg[MCCEOERF],1,errpath);
                     mccerrcount++;  
                     docompile = FALSE;
                     mccerrf = stderr;
                    }
                }
             }
           origsrc = mccopensrc(arg,MCCREADSIZE+MCCALMOSTEMPTY+1,MCCNOSEARCH);                                      
           starterrors = mccerrcount;
           startwarnings = mccwarningcount;
           if (origsrc == NULL)
              MCCWRITETYPES(MCCERRF,mccerrmsg[MCCEOSRCF],mccwriteTypesS,1,arg);
            else
             {                         
              if (docompile)
                 {
                  crokbufSet = TRUE;
                  if (setjmp(crokbuf) == 0)
                     mcccompile();
                    else
                     {
                       MCCWRITE(MCCERRF,
                          "SEVERE INTERNAL ERROR DURING COMPILE.\n",0);
                       mccerrcount++;
                     }
                  crokbufSet = FALSE;
                 }
              while (mccsrc && mccsrc != origsrc)
                {
                  MCCTRC("End of compilation unit but sources still open")
                  mccclosesrc(NULL);
                }                   
              if (mccsrc == origsrc)
                 mccclosesrc(origsrc);
             }
          if (mccoption[MCCOPTLISTING] && mcclistpath != NULL)
            {
             MCCWRITE(MCCLISTF,mccerrmsg[MCCSUMMARY],3,mccerrcount-starterrors,
                                                     mccwarningcount-startwarnings,
                          mccerrmsg[MCCVERSION]);
             if (fclose(MCCLISTF))
               {
                MCCWRITE(MCCERRF,mccerrmsg[MCCECLSTF],1,listpath);
                mccerrcount++;
               }
             mcclistf = savelist;
            }
          if (mccoption[MCCOPTPPONLY] && mccppof 
                && !mccoption[MCCOPTPPSTDOUT])
             if (fclose(mccppof))
               {
                MCCWRITE(MCCERRF,mccerrmsg[MCCECPPF],1,pppath);
                mccerrcount++;
               }
          mccppof = NULL;
          if (mccoption[MCCOPTERRFILE] && mccerrf != stderr )
           {
             if (fclose(mccerrf))
               {
                MCCWRITE(stderr,mccerrmsg[MCCECERF],1,errpath);
                mccerrcount++;
               }
             mccerrf = stderr;
           }
         }
        else
         {
           MCCWRITE(MCCERRF,mccerrmsg[MCCBADFTYPE],1,arg);
         }
       nfiles++;
     }
  }             
if (nfiles == 0)
  MCCERR(MCCENOFILES);

fflush(stdout);
fflush(stderr);
ret = mccterm(argv[0]);         
              
wallseconds = 1 + time(NULL) - start;
#ifndef DOS
times(&timesbuf);
cpuseconds = ((float)(timesbuf.tms_utime + timesbuf.tms_stime))
                / TMSUNITSPERSECOND;
#else
cpuseconds = wallseconds;
#endif

if (cpuseconds > 0.0 && emitstats)
  {                             
   FILE *fd;
   if (mccoption[MCCOPTPPSTDOUT])
     fd = MCCERRF;
    else
     fd = MCCLISTF;
   MCCWRITE(fd,
"\
%s %s %d files, %d lines, %d tokens, %d bytes \n\
     in %.3f cpu-seconds (%ld wallclock).\n",
        8,
        argv[0],
        (mccoption[MCCOPTPPONLY]) ? "PREPROCESSED" : "COMPILED",
        mcctotfiles,mcctotlines,mccntokens,mcctotbytes,cpuseconds,wallseconds);
   MCCWRITE(fd,"\
     %d lines/cpu-second, %d bytes/cpu-second\n",2,
        (int) (mcctotlines/cpuseconds),(int) (mcctotbytes/cpuseconds));
   if (mcctrcoption['a'])
     {
      int nfreesbefore;
      int slotno;
      mcccleanup = TRUE;
      nfreesbefore = mccnfrees;
      mcctokfreelist(mccfreetoks);
      MCCWRITE(fd,
         "%d tokens remain on free list.\n",1,mccnfrees-nfreesbefore);
      MCCWRITE(fd,
         "Memory usage: peak=%d bytes, final=%d bytes.\n",2,
           mccmempeak,mccmemtot);     
      MCCWRITE(fd,
         "         %d bigchunk allocations.\n",1,mccbigchunks);
      MCCWRITE(fd,
         "         %d allocations, %d went to malloc (%%%d)\n",3,
              mccnmallocs,mccnrealmallocs,(mccnrealmallocs*100)/mccnmallocs); 
      MCCWRITE(fd,
         "         %d frees, %d went to free (%%%d)\n",3,
              mccnfrees,mccnrealfrees,(mccnrealfrees*100)/mccnfrees); 
      for (slotno = 0; slotno < NMEMSIZES; slotno++)
        if (mccfbusage[slotno])
          {
           fprintf(fd," free mem slot %d: %d usages, %d currently.\n",
                  slotno,mccfbusage[slotno],mccfbcur[slotno]);
          }
     }
  }
if (ret && amcpp && strcmp(ipath,"-") != 0)
   unlink(ipath);
if (nfiles && (mccoption[MCCOPTPPSTDOUT] ||
     strcmp(pppath,"-") == 0) && mccppoption['f'])
  {
   int nlines = 100;
   while (nlines--)
     fprintf(stdout,
"                                                                          \n");
  }
fflush(stdout);
fflush(stderr);
exit( ret) ;
}

void mcccroak(sig)
  int sig;
 {
   MCCWRITE(MCCERRF,"\n Signal %d caught during compile.\n",1,sig);
   if (mccsrc)
      {
       MCCWRITE(MCCERRF,"source file='%s', line=%d, column=%d.\n",3,
               (mccsrc->pathname) ? mccsrc->pathname : "NULL",
                mccsrc->lineno,mccsrc->curchar - mccsrc->linestart + 1);
      }
   if (crokbufSet)
     longjmp(crokbuf,sig);
    else
      {
       mccterm("mcc");
       exit(sig);
      }
 }

