/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME: mccterm.c                                                   */
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
/* PURPOSE: terminate the compiler                                   */
/*                                                                   */
/*  Internal functions in this module                                */
/* INCLUDE .h                                                        */
#include <stdlib.h>                                          
#include "mcc.h"   
/* GLOBAL DECLARATIONS:                                              */
/* EXTERNAL INTERFACES:                                              */
/* HISTORY:                                                          */
/*                                                                   */
/* NOTES:                                                            */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/


/********************************************************** mccterm  */
/*-------------------------------------------------------------------*/
/* NAME: mccterm                                                     */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: terminate the compiler                                   */
/* USAGE:                                                            */
 int mccterm (pgmname)                 
/* PARAMETERS:                                                       */
 char *pgmname;  /* argv[0] from main. */
{
/* RETURNS:                                                          */
/*   the number of compile errors                                    */
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
long int endpos;
/********************************************************** mccterm **/
if (mccerrcount + mccwarningcount)
     MCCWRITE(MCCERRF,
          mccerrmsg[MCCSUMMARY],4,
          pgmname,mccerrcount,mccwarningcount,mccerrmsg[MCCVERSION]);

if (mccerrpath != NULL)
  {             
    endpos = ftell(MCCERRF);
    if (fclose(MCCERRF))
       MCCABORT(MCCECERF);
    if (endpos == mccerrbegin)
        MCCDELETEFILE(mccerrpath);
  }
fflush(MCCLISTF);
 
return mccerrcount;
}
