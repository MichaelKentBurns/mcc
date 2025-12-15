/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME: ustrpriv.h                                                  */
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
/* PURPOSE: unique string facitlities - private data structures      */
/*                                                                   */
/* NOTES:                                                            */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/
/*34567890123456789012345678901234567890123456789012345678901234567890*/
/*       1         2         3         4         5         6         7*/

typedef struct USTRNODE *ustrNodeP;
typedef struct USTRNODE 
     {
      ustrNodeP     left,right;
      unsigned long id;
      union {
#             define USTRSHORTLEN 12
              char shortText[USTRSHORTLEN];
              struct {
                       /* if a string is put in shortText, */
                       /* the first byte of it makes this TRUE */ 
                       boolean  isNotValid; 
                       boolean  isConst; /* TRUE if caller owns text */
                       ptr      text;
                     } longText;
            } t;
     } ustrNode;

typedef struct USTRPOOL *ustrPoolP;
typedef struct USTRPOOL 
     {                                        
      int        nquick;
      long       baseid;
      ustrNodeP  root;
      ptr        (*quicklist)[1];
      ustrNodeP  *topindex;
      int        nsearches;
      int        nsearchsteps;
      int        maxsearchdepth;
      int        curnnodes;
      int        maxnnodes;
     } ustrPool;
     

#define USTRNODETEXT(node) (((node)->t.longText.isNotValid) ? (node)->t.shortText \
                                                           : (node)->t.longText.text)

