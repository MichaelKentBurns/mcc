/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME: ustr.h                                                      */
/* DOCUMENT: none                                                    */
/* AUTHOR: Michael Burns                                             */
/* DATE: 24feb89                                                     */
/* SUPPORT: Michael Burns                                            */
/* TYPE:                                                             */
/* PRODUCT:  mikes c compiler                                        */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* SCRIPT:                                                           */
/* PURPOSE: unique string facitlities                                */
/*                                                                   */
/* NOTES:                                                            */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/
/*34567890123456789012345678901234567890123456789012345678901234567890*/
/*       1         2         3         4         5         6         7*/
                          
#include "mccmem.h"
#ifndef __ARGS
#include "proto.h"
#endif                                       
#ifndef TRUE
#include "types.h"
#endif

/* create a new ustr pool, and update pointer to quick table */
extern ptr     ustrNewPool  (int nquick, long baseid, ptr (*quicktab)[]);

/* add an optional top level index to the pool */
extern int     ustrAddIndex (ptr pool);               

/* print pool statistics to file */
extern void    ustrPoolStats (ptr pool, ptr fd);               

/* free a ustr pool , calling user function elfree for each element */
extern int   ustrFreePool (ptr pool, ptr node, void (*elfree)()); 

/* insert a new element given pointer to already allocated string and quick reference id */
/* return unique identifier for the new element */
extern long    ustrAddConst (ptr pool, ptr text, long id);

/* same as above, but does not actually add to the tree, just the quick table */
extern long    ustrAddQuickConst (ptr pool, ptr text, long id); 

/* insert a new element given pointer to text (copy will be made) and quick reference id */
/* return unique identifier for the new element */
extern long    ustrAdd      (ptr pool, ptr text, long id);
                           
/* return a pointer to the text of a ustr given its unique id, or NULL if not found */
extern ptr     ustrText     (ptr pool, int id );           

/* return the unique id given the text, or 0 if not found */
extern long    ustrId       (ptr pool, ptr text );

/* remove an element from the pool, given it's id , return id if ok, else NULL */
extern unsigned long ustrRem      (ptr pool, long id );

/* remove an element from the pool, given it's text, return id if ok, else NULL */
extern unsigned long ustrRemText  (ptr pool, ptr text );

/* dump all or a portion of a pool ( pass node as NULL to dump all) */
extern  int ustrDumpPool (ptr file, ptr pool, ptr node, void (*eldump)());

/* traverse all or a portion of a pool passing each Id to routine eldump */
extern  int ustrTraversePool (ptr pool, ptr node, void (*eldump)());

