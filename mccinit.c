/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME: mccinit.c                                                   */
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
/* PURPOSE: initialize the compiler                                  */
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


/********************************************************** mccinit  */
/*-------------------------------------------------------------------*/
/* NAME: mccinit                                                     */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE:                                                          */
/* USAGE:                                                            */
 boolean mccinit (argc,argv)                 
/* PARAMETERS:                                                       */
   int argc;
   char *argv[];
{
/* RETURNS:                                                          */
/*   true if succeeded in intializing ok                             */
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
char *temp;                    
int fatal = 0;                   
int value;
int i;
/********************************************************** mccinit **/

for (i=0; i < NMEMSIZES; i++)
  {
   mccfbs[i] = NULL;
   mccfbusage[i] = mccfbcur[i] = 0;
  }
/*- - - - - - - - - - - - - - - - - - - - - */                                                                  
if (argv[0] && strcmp(argv[0]+strlen(argv[0])-3 ,"c++") == 0)
   mccoption[MCCOPTCPLPL] = TRUE;
                                
for (argno = 1; argno < argc; argno++)
  {
    char *arg;
    arg = argv[argno];
    if (*arg == '-' || *arg == '~')  /* an option */
      {                                                              
        int boolvalue;
        int badopt = FALSE; 
        int optset = TRUE;
        int opt;
        char *secondary;

        argv[argno] = "";
        opt = arg[1];
        value = *arg;
        boolvalue =  (*arg == '-');
        if (isalpha(arg[1]) && arg[2] == 0 && mccoptiontype[opt] != MCCOPTTYPEMULTI) 
          { /* single char option */
           if (mccoption[opt] != ' ')
              {
               if (mccoptiontype[opt] == MCCOPTTYPEPATHNAME)
                 switch(opt)
                   {
                     case MCCOPTOUTPUT:  
                            mccoutpath = argv[++argno]; argv[argno] = "";
                         break;
                     case MCCOPTPPOUTPUT:  
                            mccppopath = argv[++argno]; argv[argno] = "";
                         break;
                     case MCCOPTZSYSIDIR: 
                            sprintf(mccsysidir,"%s/usr/include",
                                    argv[++argno]); 
                            argv[argno] = "";
                         break;
                   }
               mccoption[opt] = *arg;      
              }
             else
              if (mccoptiontype[opt] == MCCOPTTYPEIGNORE)
                   MCCWRITE(MCCERRF,mccerrmsg[MCCOPTIGNORED],opt);
                else  
                  badopt = TRUE;
          }  
         else
          { /* multi character option */
           secondary = NULL;
           switch (opt)
             {        
               case MCCOPTERRFILE:
                     mccerrpath = (arg[2] == '=') ? &arg[3] : &arg[2]; 
                   break;
               case MCCOPTOUTPUT: 
                     mccoutpath = (arg[2] == '=') ? &arg[3] : &arg[2];
                   break;
               case MCCOPTPPOUTPUT: 
                     mccppopath = (arg[2] == '=') ? &arg[3] : &arg[2];
                   break;
               case MCCOPTZSYSIDIR: 
                     sprintf(mccsysidir,"%s/usr/include",
                          (arg[2] == '=') ? &arg[3] : &arg[2]);
                   break;
               case MCCOPTTRACE: secondary = mcctrcoption; break;
               case MCCOPTLISTING: secondary = mcclistoption; break;
               case MCCOPTPARSE: secondary = mccparseoption; break;
               case MCCOPTOPTIM: secondary = mccoptimoption; break;
               case MCCOPTPPONLY: secondary = mccppoption; 
                            optset = FALSE; break;
               case MCCOPTDEF:
               case MCCOPTUNDEF:
                         if (mccdefsrc == NULL)
                           {
                             char *d;
                             mccdefsrc = mccopensrc(NULL,1000,0);
                             if (mccdefsrc == NULL)
                              {
                                MCCWRITE(MCCERRF,mccerrmsg[MCCENOMEM]);
                                fatal++;
                                return(!fatal);
                              }
                             d = mccdefsrc->buffer;  
#if 0
                             mccdefsrc->linestart = d;
                             strcat(d,"/* command line defines/undefines */\n");
                             while(*d) d++;
#endif
                             mccdefsrc->curchar = d;
                             mccdefsrc->eof = TRUE;
                           }
                         if (opt == MCCOPTDEF)
                           {
                            char *s,*d;
                            s = arg+2;
                            d = mccdefsrc->curchar;
                            strcat(d,"#define ");
                            while(*d) d++;
                            while(*s && *s != '=') 
                               *d++ = *s++;
                            if (*s == '=')
                              {
                                s++;
                                *d++ = ' ';
                                while(*s)
                                   *d++ = *s++;
                              }
                            *d++ = '\n';
                            mccdefsrc->curchar = d;
                           } 
                          else
                           {
                             char *d;
                             d = mccdefsrc->curchar;
                             strcat(d,"#undef ");
                             strcat(d,arg+2);
                             while(*d) d++;
                             *d++ = '\n';
                             mccdefsrc->curchar = d;
                           } 
                       break;
               case MCCOPTIDIR:   
                       {
                        if (arg[2] && arg[2] != ' ')
                          temp = arg + 2;
                         else
                          {
                            temp = argv[++argno];
                            argv[argno] = "";
                          }
                        if (mccnidirs < MCCIDIRMAX)
                          {                  
                            mccidirs[mccnidirs++] = temp;
                          }                              
                         else
                          {
                           MCCWRITE(MCCERRF,mccerrmsg[MCCMANYIDIRS],temp);
                           fatal++;
                          }
                       }
                     break;
               default:    
                 if (strcmp(arg,MCCOPTANSI) == 0)
                    {
                     mccoptansi = boolvalue;
                    }
                 if (strcmp(arg,MCCOPTSYSIDIR) == 0)
                    {
                     strcpy(mccsysidir,argv[++argno]);
                     argv[argno] = "";
                    }
                   else
                 if (strcmp(arg,MCCOPTCPLUSPLUS) == 0)
                    {
                     mccoption[MCCOPTCPLPL] = *arg;
                    }
                   else
                     if (mccoptiontype[opt] == MCCOPTTYPEIGNORE)
                         MCCWRITE(MCCERRF,mccerrmsg[MCCOPTIGNORED],opt);
                        else  
                          badopt = TRUE;
             }              
           if (secondary)
             {
               char *p;            
               char c;
               for (p = arg+2; c = *p; p++)
                 {
                  if (c == '-' || c == '~')
                     value = c;
                    else
                     if (c < 128 && secondary[c] != ' ')
                         secondary[c] = value;
                       else
                         {
                          MCCWRITE(MCCERRF,mccerrmsg[MCCBADSUBOPT],c,opt);
                          fatal++;
                         }
                 }
             }    
            if (!badopt && optset)
               mccoption[opt] = *arg;
          }
        if (badopt)                    
          {
            MCCWRITE(MCCERRF,mccerrmsg[MCCWBADOPTION],arg);
            fatal++;
          }
      }
  } 
                   
/* go through single char option array and set any that are turned off to FALSE */
  {
    char *opt;

    for (opt = mccoption; *opt; opt++)
       if (*opt == '!' || *opt == '~' )
          *opt = FALSE;
    for (opt = mcctrcoption; *opt; opt++)
       if (*opt == '!' || *opt == '~' )
          *opt = FALSE;
    for (opt = mcclistoption; *opt; opt++)
       if (*opt == '!' || *opt == '~' )
          *opt = FALSE;
    for (opt = mccoptimoption; *opt; opt++)
       if (*opt == '!' || *opt == '~' )
          *opt = FALSE;
    for (opt = mccparseoption; *opt; opt++)
       if (*opt == '!' || *opt == '~' )
          *opt = FALSE;
    for (opt = mccppoption; *opt; opt++)
       if (*opt == '!' || *opt == '~' )
          *opt = FALSE;
  }

if (!fatal && mccdefsrc != NULL)
  {
#if 0
   strcat(mccdefsrc->curchar,"/* end of command line defines/undefines */\n");
   mccdefsrc->curchar[0] = '\n';
#else
   mccdefsrc->curchar[1] = 0;
#endif
   mccdefsrc->end = mccdefsrc->curchar;
   while(*mccdefsrc->end)
        mccdefsrc->end++;
   mccdefsrc->curchar = mccdefsrc->buffer;
   mccsrc = NULL;
  }

if (!fatal && mccerrpath != NULL && mccerrpath[0])
  {
    if (freopen(mccerrpath,"w",MCCERRF) == NULL)
       MCCRETURN(MCCEOERF,FALSE);
    mccerrbegin = ftell(MCCERRF);
  }
return (!fatal);
}
/***************************************************** end mccinit  **/


