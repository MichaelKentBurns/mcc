#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME:     mcc tokenization services                               */
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
/* PURPOSE: parse out c language tokens                              */
/*                                                                   */
#include "mcc.h"
#include "ustr.h" 
/*  Internal functions in this module                                */
    token *mcctoklist();     /* turn string into token list          */
    token *mcctok(int spanlines);         /* parse token from string              */
    tokenp mcctokcopy();     /* copy or make a new token             */
    void   mcctoktrace();    /* trace a new token                    */
    int    mccreduce();      /* reduce a token list                  */
    void   mcctoklstdump();  /* dump a list                          */
/* INCLUDE .h                                                        */

/* GLOBAL DECLARATIONS:                                              */
/* EXTERNAL INTERFACES:                                              */
/* HISTORY:                                                          */
/*                                                                   */
/* NOTES:                                                            */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/
/********************************************************** mcctoklstdump */
/*-------------------------------------------------------------------*/
/* NAME: mcctoklstdump                                               */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: dump out a token list                                    */
/* USAGE:                                                            */
 void mcctoklstdump( list )
/* PARAMETERS:                                                       */
  tokenp list;    /* (I) linked list of tokens to dump               */
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
                             
int n;
tokenp cur,previous;

/********************************************************** mcctoklstdump **/

if ( list != NULL )
  {   
#  if DEBUG
     fprintf(stderr,"mcctoklstdump: dump of list at %08x: \n",list);
     /* print final token list */                    
     for (cur = list, previous = NULL, n=1;
        cur != NULL;
        previous = cur, cur = cur->right,n++ )
       {                
        if (previous != cur->left)
          fprintf(stderr,"    ******* ERROR **** backward link fault (%08x != %08x)\n",
                                previous,cur->left);
        fprintf(stderr,
          "%d token @%08x, text='%s', value=%d, location=%d/%d/%d left=%08x,right=%08x\n",
                  n, cur, cur->text, cur->value, 
                     cur->sourceLoc.file,cur->sourceLoc.line,cur->sourceLoc.column,
                     cur->left, cur->right);
       }   
#  endif
  }
return;
}
/***************************************************** end mcctoklstdump **/


/********************************************************** mcctokremove */
/*-------------------------------------------------------------------*/
/* NAME: mcctokremove                                                */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: remove a token from the list and free it.                */
/* USAGE:                                                            */
 void mcctokremove( listp, dispose )
/* PARAMETERS:                                                       */
  tokenp *listp;  
  tokenp dispose;
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

/********************************************************** mcctokremove **/

   if (dispose != NULL)
     {
#     if DEBUG
       if (mccoption[MCCOPTTRACE] && mcctrcoption['f'])
         fprintf(stderr,"mcctokremove:  disposing of token @%08x\n",dispose);
#     endif     
      if (dispose->left == NULL)
         *listp = dispose->right;
       else
         dispose->left->right = dispose->right;
      if (dispose->right != NULL)
         dispose->right->left = dispose->left;
      if (dispose->text != NULL)
         free(dispose->text);
      free(dispose);
    }
return;
}
/***************************************************** end mccispose **/ 

/********************************************************** mcctokfree   */
/*-------------------------------------------------------------------*/
/* NAME: mcctokfree                                                  */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: free a token                                             */
/* USAGE:                                                            */
 void mcctokfree( dispose )
/* PARAMETERS:                                                       */
  tokenp dispose;
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

/********************************************************** mcctokfree **/

   if (dispose != NULL)
     {
#     if DEBUG               
      if (mccoption[MCCOPTTRACE] && mcctrcoption['f'])
          MCCWRITE(MCCLISTF,"mcctokfree:  freeing token @%08x\n",1,dispose);
#     endif     
      if (dispose->flags & TFmalloced)
         {
          if (dispose->text != NULL)
            free(dispose->text);
#if TRUE
          dispose->text = NULL;
          dispose->right = mccfreetoks;
          mccfreetoks = dispose;
#else
          free(dispose);
#endif
        }
    }
return;
}
/***************************************************** end mcctokfree **/ 

/********************************************************** mcctokfreelist   */
/*-------------------------------------------------------------------*/
/* NAME: mcctokfreelist                                                  */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: free a token                                             */
/* USAGE:                                                            */
 void mcctokfreelist( dispose )
/* PARAMETERS:                                                       */
  tokenp dispose;
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
tokenp temp;
/********************************************************** mcctokfreelist **/

   for (; dispose; dispose = temp)
     {                            
      temp = dispose->right;
#     if DEBUG               
      if (mccoption[MCCOPTTRACE] && mcctrcoption['f'])
          MCCWRITE(MCCLISTF,"mcctokfreelist:  freeing token @%08x\n",1,dispose);
#     endif     
      if (dispose->flags & TFmalloced)
         {
          if (dispose->text != NULL)
            free(dispose->text);
          if (mcccleanup)
            free(dispose);
           else
            {
             dispose->text = NULL;
             dispose->right = mccfreetoks;
             mccfreetoks = dispose;
            }
         }
    }
return;
}
/***************************************************** end mcctokfreelist **/ 

#if 0
/********************************************************** mccreduce */
/*-------------------------------------------------------------------*/
/* NAME: mccreduce                                                   */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: reduce the token list to one token if possible           */
/* USAGE:                                                            */
 int mccreduce( listp )
/* PARAMETERS:                                                       */
  tokenp *listp;
{
/* RETURNS:  number of tokens left after reduction                   */
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

tokenp cur,left,right;    
int   numtoks; 
tokenp subexp; 
tokenp point;  
tokenp dispose;
int    nest;                         
char  *oldtext;
int   errors;
/********************************************************** mccreduce **/

if ( *listp != NULL )
  {          
     errors = 0;
     /* reduce at precedence level 1; parenthesized sub expressions */ 
     for (cur = *listp;
        cur != NULL;
        cur = cur->right)
       {
        if (cur->precedence == 1)
          {
#          if DEBUG
           if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
              fprintf(stderr,"iereduce:  token '%s' is precedence 1\n",
                         cur->text);
#          endif     
           if (cur->value == IEBEGIN)
             {       
               subexp = cur->right;
               for (point = subexp, nest = 1; 
                    point != NULL && nest >= 1;
                    point = point->right
                   )
                 {
                   if (point->precedence == 1)
                     {
                      if (point->value == IEBEGIN)
                          nest++;             
                        else 
                         {
                          nest--;   
                          if (nest == 0)
                            break;
                         }
                     }
                 }   
               if (nest == 0)
                 {                   
                   /* snip off sub expression and reduce */                  
                   point->left->right = NULL; 
                   subexp->left = NULL;    
                   cur->right = point;
                   point->left = cur;
                   /* if successfully reduced */                               
                   if (iereduce(&subexp) == 1)
                      /* dispose of parens and replace left paren with token */
                      {                     
#                       if DEBUG
                        if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
                         {
                          fprintf(stderr,"        subexpr=%08x\n",subexp);
                          mcctoklstdump(*listp);
                         }
#                       endif
                        if (cur->left == NULL)
                           *listp = subexp;
                         else
                           cur->left->right = subexp;
                        subexp->left = cur->left;
                        mcctokfree(cur);
                        subexp->right = point->right;
                        if (subexp->right != NULL)
                           subexp->right->left = subexp;
                        mcctokfree(point);
                        oldtext = subexp->text;
                        if (oldtext != NULL)
                          {
                           subexp->text = malloc(strlen(oldtext)+3);
                           if (subexp->text != NULL)
                              sprintf(subexp->text,"(%s)",oldtext);
                           free(oldtext);
                          }
                        cur = subexp;
                      }
                    else
                      /* keep parens and insert reduced subexp in between */
                      { 
                        cur->right = subexp;
                        subexp->left = cur;
                        for (point->left = subexp;
                             point->left->right != NULL;
                            )
                           point->left = point->left->right;
                        point->left->right = point;
                        cur = point;
                      }
                 }
                else                                                    
                 {
                  fprintf(stderr,"ERROR: left parenthesis without matching right parenthesis.\n");
                  errors++;
                 }
             }
            else
             {
               fprintf(stderr,"ERROR: right parenthesis without matching left parenthesis.\n");
               errors++;
             }
          }
       }                         

     if (errors == 0)
       {
        /* reduce at precedence level 2; unary operators */ 
        /* these are evaluated right to left, scan to end of list */
        for (cur = *listp;
             cur != NULL && cur->right != NULL;   
            )
            cur = cur->right;
        for (;
           cur != NULL;
           cur = cur->left)
          {                                             
           if (cur->precedence == 2)
             {
#              if DEBUG
               if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
                 fprintf(stderr,"iereduce:  token '%s' is precedence 2\n",
                         cur->text);
#              endif     
               left = cur->left;
               right = cur->right;
               dispose = NULL;
               switch(cur->value)
                 {
                  case IEBNOT: 
                  case IEINVERT: 
                  case IENEG: 
                     if (right != NULL && right->precedence == IEINT)
                       {
                        dispose = cur;
                        oldtext = right->text;
                        if (oldtext != NULL)
                          {
                           right->text = malloc(strlen(oldtext)+strlen(cur->text)+1);
                           if (right->text != NULL)
                              sprintf(right->text,"%s%s",cur->text,oldtext);
                           free(oldtext);
                          }    
                        switch(cur->value)
                          {
                             case IEBNOT:
                                 right->value = !(right->value);
                               break;
                             case IEINVERT:
                                 right->value = ~(right->value);
                               break;
                             case IENEG:
                                 right->value = -(right->value);
                               break;
                          }
                        cur = right;
                       }                               
                     break;
                  case IEINCR: 
                  case IEDECR:               
                     if (left != NULL && 
                           (left->precedence == IEINT || left->precedence == IENAME))
                           point = left;
                        else            
                     if (right != NULL && 
                           (right->precedence == IEINT || right->precedence == IENAME))
                           point = right;
                       else
                          point = NULL;
                     if (point != NULL && point->precedence == IEINT)
                       {
                        dispose = cur;
                        oldtext = point->text;
                        if (oldtext != NULL)
                          {
                           point->text = malloc(strlen(oldtext)+strlen(cur->text)+1);
                           if (point->text != NULL)
                             {   
                                if (point == right)
                                  sprintf(point->text,"%s%s",cur->text,oldtext);
                                else
                                  sprintf(point->text,"%s%s",oldtext,cur->text);
                             }
                           free(oldtext);
                          }    
                        switch(cur->value)
                          {
                             case IEINCR:
                                 point->value++;
                               break;
                             case IEDECR:
                                 point->value--;
                               break;
                          }
                        cur = point;
                       }                               
                     break;
                 } 
               if (dispose != NULL)
                  mcctokremove(listp,dispose);

             }
          }
       }
     if (errors == 0)
       {
        /* reduce at precedence level 3; binary operators */ 
        for (cur = *listp;
           cur != NULL;
           cur = cur->right)
          {                                             
           if (cur->precedence == 3)
             {
#              if DEBUG
               if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
                 fprintf(stderr,"iereduce:  token '%s' is precedence 3\n",
                         cur->text);
#              endif     
               left = cur->left;
               right = cur->right;

               if (right != NULL && right->precedence == IEINT
                         && left != NULL && left->precedence == IEINT)
                 {
                        oldtext = cur->text;
                        if (oldtext != NULL && left->text != NULL 
                                 && right->text != NULL)
                          {
                           cur->text = malloc(
                                      strlen(oldtext)+strlen(left->text)+
                                      strlen(right->text)+1);
                           if (cur->text != NULL)
                              sprintf(cur->text,"%s%s%s",
                                        left->text,oldtext,right->text);
                           free(oldtext);
                          }    
                        switch(cur->value)
                          {
                             case IEMULT:
                                 cur->value = left->value * right->value;
                               break;
                             case IEDIV:
                                 cur->value = left->value / right->value;
                               break;
                             case IEMOD:
                                 cur->value = left->value % right->value;
                               break;
                          }
                        cur->precedence = IEINT;
                  mcctokremove(listp,left);
                  mcctokremove(listp,right);
                 }                               
             }
          }
        }
     if (errors == 0)
        {
        /* reduce at precedence level 4; more binary operators */ 
        for (cur = *listp;
           cur != NULL;
           cur = cur->right)
          {                                             
           if (cur->precedence == 4)
             {
               left = cur->left;
               right = cur->right;

               if (right != NULL && right->precedence == IEINT
                         && left != NULL && left->precedence == IEINT)
                 {
#                  if DEBUG
                     fprintf(stderr,"iereduce:  token '%s' is precedence 4\n",
                         cur->text);
#                  endif     
                        oldtext = cur->text;
                        if (oldtext != NULL && left->text != NULL 
                                 && right->text != NULL)
                          {
                           cur->text = malloc(
                                      strlen(oldtext)+strlen(left->text)+
                                      strlen(right->text)+1);
                           if (cur->text != NULL)
                              sprintf(cur->text,"%s%s%s",
                                        left->text,oldtext,right->text);
                           free(oldtext);
                          }    
                        switch(cur->value)
                          {
                             case IEPLUS:
                                 cur->value = left->value + right->value;
                               break;
                             case IEMINUS:
                                 cur->value = left->value - right->value;
                               break;
                          }                     
                       cur->precedence = IEINT;
                  mcctokremove(listp,left);
                  mcctokremove(listp,right);
                 }                               
             }
          }
        }
     if (errors == 0)
        {
        /* reduce at precedence level 5; still more binary operators */ 
        for (cur = *listp;
           cur != NULL;
           cur = cur->right)
          {                                             
           if (cur->precedence == 5)
             {
#              if DEBUG
               if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
                 fprintf(stderr,"iereduce:  token '%s' is precedence 5\n",
                         cur->text);
#              endif     
               left = cur->left;
               right = cur->right;

               if (right != NULL && right->precedence == IEINT
                         && left != NULL && left->precedence == IEINT)
                 {
                        oldtext = cur->text;
                        if (oldtext != NULL && left->text != NULL 
                                 && right->text != NULL)
                          {
                           cur->text = malloc(
                                      strlen(oldtext)+strlen(left->text)+
                                      strlen(right->text)+1);
                           if (cur->text != NULL)
                              sprintf(cur->text,"%s%s%s",
                                        left->text,oldtext,right->text);
                           free(oldtext);
                          }    
                        switch(cur->value)
                          {
                             case IELSHIFT:
                                 cur->value = left->value << right->value;
                               break;
                             case IERSHIFT:
                                 cur->value = left->value >> right->value;
                               break;
                          }                    
                        cur->precedence = IEINT;
                  mcctokremove(listp,left);
                  mcctokremove(listp,right);
                 }                               
             }
          }
       }
     if (errors == 0)
        {
        /* reduce at precedence level 10; fortran array dimension range */ 
        for (cur = *listp;
           cur != NULL;
           cur = cur->right)
          {                                             
           if (cur->precedence == 10)
             {
#              if DEBUG
               if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
                 fprintf(stderr,"iereduce:  token '%s' is precedence 10\n",
                         cur->text);
#              endif     
               left = cur->left;
               right = cur->right;

               if (right != NULL && right->precedence == IEINT
                         && left != NULL && left->precedence == IEINT)
                 {
                        oldtext = cur->text;
                        if (oldtext != NULL && left->text != NULL 
                                 && right->text != NULL)
                          {
                           cur->text = malloc(
                                      strlen(oldtext)+strlen(left->text)+
                                      strlen(right->text)+1);
                           if (cur->text != NULL)
                              sprintf(cur->text,"%s%s%s",
                                        left->text,oldtext,right->text);
                           free(oldtext);
                          }    
                        switch(cur->value)
                          {
                             case IERANGE:
                                 cur->value = right->value - left->value + 1;
                               break;
                          }                    
                        cur->precedence = IEINT;
                  mcctokremove(listp,left);
                  mcctokremove(listp,right);
                 }                               
             }
          }
       }        
     for (cur = *listp, numtoks = 0;
          cur != NULL;
          cur = cur->right 
         )
       {
         numtoks++;
       } 
  }           
else
  numtoks = 0;

return numtoks;
}
/***************************************************** end mccreduce **/
#endif
                                                                      
/********************************************************** mcctoklist*/
/*-------------------------------------------------------------------*/
/* NAME:  mcctoklist                                                 */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE:                                                          */
/*   to parse a string into a linked list of token structures        */
/*                                                                   */
/* USAGE:                                                            */
  tokenp mcctoklist( expression )
/* PARAMETERS:                                                       */
  char  *expression;
{
/* RETURNS:                                                          */
/*  pointer to a linked list of token structures                     */
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
tokenp left, cur, right, new; 
tokenp list;   
int   errors;
char *value;
char valbuf[200];
char *p;
/**********************************************************mcctoklist**/
                                 
   list = left = NULL; 
   errors = 0;
   for (p = expression; *p; )
     {
       cur = mcctok(FALSE);
       if (cur == NULL)
         errors++;
        else
         if (left == NULL)
             list = left = cur;
           else 
             {
              left->right = cur; 
              cur->left = left;
              left = cur;
             } 
     }
#  if DEBUG && 0        
    {
       if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
          fprintf(stderr,"mcctoklist: original list for expression '%s':\n",
              expression);
     mcctoklstdump(list);
    }
#  endif
#if 0
   /* must check for symbols and expand them if any found */                    
   for (right = NULL, cur = list;
        cur != NULL;
        left = cur, cur = cur->right)
     {
       if (cur->precedence == IENAME)  
         {              
           if (find(cur->text,value = valbuf,pptr))
             {
#             if DEBUG 
              if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
                 fprintf(stderr,"mcctoklist: symbol '%s' is '%s'.\n",
                          cur->text,value);
#             endif                                                            
               new = mcctoklist(value); 
               if (new != NULL) 
                 {   
                   /* make right point to left element of new list */      
                   for (right = new; right->right != NULL; right = right->right);
                   /* attach tail of old list to tail of new list */
                   right->right = cur->right;
                   if (cur->right != NULL)
                      cur->right->left = right;
                   /* insert new list into old list at current point */
                   /* and remove cur from list in the process */
                   if (cur->left == NULL)
                     {  
                       list = new;
                       new->left = NULL;
                     }                  
                   else
                     {
                       cur->left->right = new;
                       new->left = cur->left;
                     }       
                   /* now dispose of cur, its been replaced by new list */                
                   if (cur->text != NULL)
                      free(cur->text);
                   free(cur);
#                  if DEBUG && 0
                   if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
                       fprintf(stderr,"mcctoklist: full list after expansion:\n");
                       mcctoklstdump(list);
#                  endif
                 }
               else
                 {
                  errors++;
                 }
             }
            else
             {
#             if DEBUG 
                 fprintf(stderr,"ERROR: unknown symbol '%s'.\n",cur->text);
#             endif                                                            
              errors++;
             }
         }
     }                                                    

   /* must check for unary neg tokens that really are binary minus */                    
   for (cur = list, left = NULL;
        cur != NULL;
        left = cur, cur = cur->right)
     {
       if (cur->precedence == 2 && cur->value == IENEG && left != NULL)
         if (left->precedence == IEINT)
           {
            cur->value = IEMINUS;
            cur->precedence = 4;
           }
     } 
       
#  if DEBUG && 0         
   if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
    {
     fprintf(stderr,"mcctoklist: list after change negs to minus:\n");
     mcctoklstdump(list);
    }
#  endif                                               
#endif
   if (errors)  
     {
#      if DEBUG   
         fprintf(stderr,"mcctoklist: %d Syntax errors have occured!\n",errors);
#      endif
       for (left = list; left != NULL; left = cur)
         {
           cur = left->right;    
           if (left->text != NULL) 
              free(left->text);
           free(left);
         }
       return NULL;
     } 
    else
      return list;
}
/***************************************************** end mcctoklist**/

/********************************************************** mcctok   */
/*-------------------------------------------------------------------*/
/* NAME: mcctok                                                      */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: parse out a single token from a string.                  */
/* USAGE:                                                            */
  tokenp mcctok( spanlines )
int spanlines;
/* PARAMETERS:                                                       */
{
/* RETURNS:                                                          */
/*    pointer to new token structure allocated                       */
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
char *p;                /* scan pointer into expression */
char first;                 /* first character of token */
int  c;                 /* subsequent character of token */
int  nextc;                 /* subsequent character of token */
char *start;            /* pointer to start of token */
int    len;             /* length of token */
unsigned long   value;  /* value of token */                           
double dval;
int    islong;
int    ttype;           /* token type */
char   *text;
char   temp[3*ANSILENSTRLIT+1];
char   *tp;
tokenp tok;           /* pointer to token structure being built */
int    filenum;
int    linenum;
int    column;                                                    
int    incomment;           
int    newcomment;                           
int    escnewline;      
int    cplpl;        /* allow c++ syntax */
int    commentToEol;  /* current comment is c++ sytle // */
static int stillInComment = FALSE; /* finished last token in mid comment */
int    breakline;
int strlenwarning = FALSE;
int strlitNewline = FALSE;
int haveu;
/********************************************************** mcctok  **/
    
if (!mccsrc)
  return NULL;
if (mccsrc->tokq)
  {
    tok = mccsrc->tokq;
    if (tok->right)
      tok->right->left = NULL;
    mccsrc->tokq = tok->right;
    tok->right = NULL;
    return(tok);
  }
        
cplpl = mccoption[MCCOPTCPLPL];                                                         
p = mccsrc->curchar;  /* point into current source buffer */
MCCBUFCHECK(p);   /* make sure there is source in buffer */
     
start = NULL; /* set to !NULL in whitespace if comment */
/* printf("check whitespace, p=%08x *p = %c\n",p,*p);   */
/* skip over whitespace */
breakline = __LINE__+1;
for (incomment = newcomment = stillInComment, 
           escnewline = FALSE, commentToEol = FALSE; 
     TRUE;
     newcomment = FALSE , escnewline = FALSE) 
  {   
    c = *p & 0x7f;
    nextc = p[1] & 0x7f;
    newcomment = (c == '/' 
                  && ((nextc == '*') 
                      || (!incomment && cplpl && nextc == '/')) ) ;
    escnewline = (c == '\\' && nextc == '\n');
    if (c == 0)
      {
       if (mccsrc->eof || start )
          { breakline = __LINE__; break; }
        else
         {
          MCCBUFCHECK(p)
         }
      }
     else
       {
        if (c == '\n')
          {                             
            if (incomment)
              {
               if (commentToEol)
                 {
                   /* don't know how this will handle a mixture of old and c++ comments */
                  incomment = 0;
                  /* don't include the newline in the comment  p++; */
                  if (incomment == 0 && start != NULL)
                     { breakline = __LINE__; break; }
                 }
                else
                 {  /* comment that spans lines */
                   
                 }
              }
            if (spanlines || incomment)
              {
                p++;
                mccnewline(p);         
              }     
            else
              {
                mccsrc->curchar = p;
                return(NULL);
              }
          }
        else if (escnewline)  /* escaped newline in whitespace */
         {  
          p += 2;       /* skip over both */
          mccnewline(p);
         }
        else
         {
          if (!incomment && !newcomment && !isspace(c))
             { breakline = __LINE__; break; }
          p++;
          if (incomment && !commentToEol)
            {
              if (c == '*' && nextc == '/')
               {
                 incomment--;
                 p++; /* skip over the / too */
                 if (incomment == 0) /* just finished a comment */
                    {
                     if ( start != NULL)  /* need to return a token */
                        { breakline = __LINE__; break; }
                      else
                       {
                        MCCBUFCHECK(p)  /* else check buffer */
                       }
                    }
               }
            }                                   
          if (newcomment && (!incomment || mccparseoption['c']))
            {
             if (!incomment)
                {
                 if (mccoption[MCCOPTPPONLY] && mccppoption['c'])
                   {
                    filenum = mccsrc->id;
                    linenum = mccsrc->lineno; 
                    column = p - mccsrc->linestart + 1;
                    start = text = p-1;
                    ttype = COMMENT;
                   }
                 commentToEol = (nextc == '/');
                }
             incomment++;  
             p++; /* skip over * too */
            }
          else
            if (newcomment && mccparseoption['C'])
              {
               MCCWRITE(MCCERRF,mccerrmsg[MCCNESTCMT],mccsrc->id,
                    mccsrc->lineno,p - mccsrc->linestart);
               mccwarningcount++;
               if (mccoption[MCCOPTLISTING])
                  MCCWRITE(MCCLISTF,mccerrmsg[MCCNESTCMT],mccsrc->id,
                    mccsrc->lineno,p - mccsrc->linestart);
              } 
         }
       }
  }       
stillInComment = incomment;  /* may have finished a buffer in a comment */
            
value = 0;    
islong = FALSE;                    
haveu = FALSE;

if (start == NULL)
  {  /* did not get comment type token (only -p && -pc) */
     MCCBUFCHECK(p);  /* make sure there is sufficient source ready */
     start = text = p;     
                     
     filenum = mccsrc->id;
     linenum = mccsrc->lineno; 
     column = p - mccsrc->linestart + 1;
     c = (*p & 0x7f);
     if (mccsrc->eof && (c == 0 || c == MCCEOFCHAR))
        ttype = EOFILE;
       else
       switch (mcctokfirst[(first = c)])
        {        
         case MCCTOKID:
                 if ((c = (*p & 0x7f)) =='$' && !mccparseoption['$']) 
                   {
                     ttype = ILLTOK;
                     p++;
                   }
                  else
                   {
                     ttype = ID;  
                      while (isalnum(c = (*p & 0x7f)) || c == '_'
                           || (c == '$' && mccparseoption['$'])) 
                          p++; 
                   }
              break;
     
         case MCCTOKSTRLIT:     
         case MCCTOKCHRLIT:     
                {
                  int escape;              
                  int escnewline; 
                  int strlit;
                  int ch;    
                  strlit = (first == '"');
                  if (strlit)
                     {
                       ttype = SLIT;
                       text = tp = temp;
                     }
                    else
                     ttype = ILIT;    
                  p++; /* skip over first */
                  for (escape = FALSE, escnewline = FALSE;
                       (c = *p++) && ((c &= 0x7f) != first);
                       )   
                    {  
                      if (c == '\\')
                       {
                        c = *p++ & 0x7f; 
                        switch(c)
                          {
                            case '\n': 
                                  mccnewline(p);
                                  escnewline = TRUE;      
                                  c = 0;
                                  break;
                            case 'a': c = '\a'; break;
                            case 'b': c = '\b'; break;
                            case 'f': c = '\f'; break;
                            case 'n': c = '\n'; break;
                            case 'r': c = '\r'; break;
                            case 't': c = '\t'; break;
                            case 'v': c = '\v'; break;
                            case 'x':
                               {
                                ch = 0;
                                for (ch = 0; isxdigit(c = (*p & 0x7f)); p++)
                                  {
                                    if (isdigit(c))
                                       c -= '0';
                                     else
                                       c = (c - (isupper(c) ? 'A':'a')) + 10;
                                    ch = (ch << 4) + c;
                                  } 
                                c = ch;
                               }
                              break;
                            default:
                              if (c >= '0' && c <= '7')
                               {
                                int n;
                                ch = 0;
                                for (ch = 0,n = 0;
                                     n < 3 && ((c = (*p & 0x7f)) >= '0') && c <= '7';
                                     p++,n++)
                                  {
                                    ch = (ch << 3) + (c - '0');
                                  } 
                                c = ch;
                               }
                              break;
                          }
                       }                
                      else 
                        if (c == '\n')
                          {
                           if (mccparseoption['n'])
                               mccnewline(p);
                             else
                              {
                                strlitNewline = TRUE;
                                p--; /* backup to point to newline again */
                                break;
                              }
                          }
                      if (escnewline)
                         escnewline = FALSE;
                        else
                          if (strlit)
                             {
                              if ((tp - temp) < (sizeof(temp)-1))
                                   *tp++ = c;
                                else /* set flag, issue warning later */
                                   strlenwarning = TRUE;
                             }
                            else
                              value = (value << 8) + c;  
                    }              
                   if (strlit)
                      *tp = 0;
                    else
                     {
                      if ((c = *p) == 'U' || c == 'u')   
                        {
                         haveu = TRUE;
                         p++;
                        }
                      if ((c = *p) == 'L' || c == 'l')   
                        {
                         p++;
                         islong = TRUE;
                        }
                      if (islong && !haveu && ((c = *p) == 'U' || c == 'u'))   
                        {
                         haveu = TRUE;
                         p++;
                        }
                     }
                }
              break;

         case MCCTOKOP:                      
                 if (!(c == '.' && isdigit(p[1])))
                   {
                    int next;
                    ttype = EOP; 
#if 0
                    while (mcctokfirst[c = (*p & 0x7f)] == MCCTOKOP)
                       p++; 
#else
                    p++;
                    next = *p;
                    switch(c)
                      {
                       case '=': 
                       case '#': 
                           if (next == c) p++; 
                           break;
                       case '-': 
                           if (next == '-' || next == '>' || next == '=')
                              p++;
                           break;
                       case '+': 
                           if (next == '+' || next == '=')
                              p++;
                          break;
                       case '>': 
                       case '<': 
                           if (next == c)
                              p++;
                           if (*p == '=')
                              p++;
                           break;
                       case '&': 
                       case '|': 
                           if (next == c || next == '=')
                              p++;
                           break;
                       case '!': 
                       case '*': 
                       case '/': 
                       case '%': 
                       case '^': 
                           if (next == '=')
                              p++;
                           break;
                       case '.': 
                           if (next == '.')
                            {
                              p++;
                              if (*p == '.')
                                p++;
                            }
                           break;
                      }
#endif
                    break;
                   }
      
         case MCCTOKNUMLIT:     
              {
                 int hex = FALSE;
                 ttype = ILIT;                  
                 if (first == '0')
                    {       
                     p++; /* skip over 0 */ 
                     c = (*p & 0x7f);
                     if (c == 'x' || c == 'X')
                       {    
                         hex = TRUE;
                         p++; /* skip over x */
                         for (; isxdigit(c = (*p & 0x7f)); p++)
                            {
                             if (isdigit(c))
                                c -= '0';
                              else
                                c = (c - (isupper(c) ? 'A':'a')) + 10;
                              value = (value << 4) + c;
                            } 
                       }
                      else
                       {
                         for (;
                              ((c = (*p & 0x7f)) >= '0') && c <= '7';
                              p++)
                            value = (value << 3) + (c - '0');
                       }
                    }
                  else
                    {
                     while (isdigit(c = (*p & 0x7f)))
                      {
                        p++; 
                        value = (value * 10) + (c - '0');
                      }
                    }
                 if (!hex && ((c = *p) == '.' || c == 'e' || c == 'E'))
                     {
                       dval = value;
                       ttype = DLIT;                  
                       if (c == '.')
                         {
                          double units = 0.1;
                          p++; /* skip over the . */
                          while (isdigit(c = (*p & 0x7f)))
                           {
                             p++; 
                             dval += (c - '0') * units;
                             units /= 10.0;
                           }
                         }
                       if ((c = *p) == 'e' || c == 'E')
                         {
                          int exponent = 0;
                          int sign;
                          p++;
                          if (*p == '-')
                            {
                             p++;
                             sign = -1;
                            }
                           else
                            {
                             sign = 1;
                             if (*p == '+')
                               p++;
                            }
                          while (isdigit(c = (*p & 0x7f)))
                           {
                             p++; 
                             exponent = (exponent * 10) + (c - '0');
                           }
                          exponent *= sign;
                          dval *= pow(10.0,(double) exponent);
                         }

                        { /* check for integer suffix */
                         if ((c = *p) == 'F' || c == 'f')   
                           {
                            ttype = FLIT;
                            p++;
                           }
                          else
                            if ((c = *p) == 'L' || c == 'l')   
                              {
                               p++;
                               islong = TRUE;
                              }
                         }
                     }
                    else
                     { /* check for integer suffix */
                      if ((c = *p) == 'U' || c == 'u')   
                        {
                         haveu = TRUE;
                         p++;
                        }
                      if ((c = *p) == 'L' || c == 'l')   
                        {
                         p++;
                         islong = TRUE;
                        }
                      if (islong && !haveu && ((c = *p) == 'U' || c == 'u'))   
                        {
                         haveu = TRUE;
                         p++;
                        }
                     }
              }
              break;
     
         case MCCTOKPUNCT:   
                ttype = PUNCT;
                value = first;
                p++;
              break;
     
         default: 
#             if DEBUG & 0  
                MCCWRITE(MCCERRF,
"INTERNAL ERROR: Got a token starting with '%c'=[%02x] dont know what to do!\
      %s line %d,breakline=%d \n",first,first,mccsrc->pathname,mccsrc->lineno,breakline);
                if (mccoption[MCCOPTLISTING])
                    MCCWRITE(MCCLISTF,
"INTERNAL ERROR: Got a token starting with '%c'=[%02x] dont know what to do!\
      %s line %d,breakline=%d \n",first,first,mccsrc->pathname,mccsrc->lineno,breakline);
#             endif                            
              p++;
#if 0
              mccsrc->curchar = p;
              return NULL;
#else
              ttype = ILLTOK;
              value = 0;
#endif

            }
  }

 if (text == start)
   len = p - start;
  else
   len = strlen(text);
  
  /* if we get to here and have not returned because of syntax error  ... */
  /*    then precedence is set, start points to begin of token, p points to   */
  /*    first character after token.                                      */
  /* we now need to allocate a token structure and fill it in.            */
  tok = mcctokcopy(NULL);
      {   
        tok->sourceLoc.file = filenum;
        tok->sourceLoc.line = linenum; 
        tok->sourceLoc.column = column;                                
        if (haveu)
           tok->flags |= TFUnsigned;
        if (islong)
           tok->flags |= TFLong;

        tok->text = malloc(len+1);
        if (tok->text == NULL) {
           MCCABORT(MCCENOMEM);
        }
        else
          {
           memcpy(tok->text,text,len);
           tok->text[len] = 0;
           if (ttype == ID)
             {
              if (mcctrcoption['g'])
                 {
                  value = ustrId(mccgsym,tok->text);
                  islong = TRUE;
                  if (value && value < MCCMAXRESERVED)
                     ttype = KEY;
                 }
             }              
           else
             if (ttype == EOP)
               {
                unsigned int32 oplit;    
                char  *t;
                for (oplit = '    ', t = tok->text; *t ; t++)
                    oplit = ( oplit << 8 ) + *t;         
                islong = FALSE;                
  /*            printf("oplit = %08x, text = %s\n",oplit,tok->text); */
                switch(oplit)
                   {
                     case '   #': ttype = PPOP; value = PPLIT; break;
                     case '  ##': ttype = PPOP; value = PPGLUE; break;
                     case '   .': value = ELEM; break;
                     case '  ->': value = ELEMP; break;
                     case '  ++': value = INC; break;   /* PREINC or POSTINC */
                     case '  --': value = DEC; break;   /* PREDEC or POSTDEC */
                     case '   &': value = ADDRESS; break;  /* or BAND */
                     case '   *': value = PTR; break;   /* or MULT */
                     case '   +': value = PLUS; break;
                     case '   -': value = MINUS; break;  /* or ANEG */
                     case '   ~': value = BNEG; break;
                     case '   !': value = NOT; break;
                     case '   /': value = DIVIDE; break;
                     case '   %': value = MODULUS; break;
                     case '  <<': value = LSHIFT; break;
                     case '  >>': value = RSHIFT; break;
                     case '   <': value = LESS; break;
                     case '   >': value = GREATER; break;
                     case '  <=': value = LESSEQ; break;
                     case '  >=': value = GREATEREQ; break;
                     case '  ==': value = EQUAL; break;
                     case '  !=': value = NOTEQUAL; break;
                     case '   ^': value = XOR; break;
                     case '   |': value = BOR; break;
                     case '  ||': value = LOR; break;
                     case '  &&': value = LAND; break;
                     case '   ?': value = TERNOP; break;
                     case '   =': value = ASG; break;
                     case '  *=': value = ASGMULT; break;
                     case '  /=': value = ASGDIVIDE; break;
                     case '  %=': value = ASGMODULUS; break;
                     case '  +=': value = ASGPLUS; break;
                     case '  -=': value = ASGMINUS; break;
                     case ' <<=': value = ASGLSHIFT; break;
                     case ' >>=': value = ASGRSHIFT; break;
                     case '  &=': value = ASGAND; break;
                     case '  ^=': value = ASGXOR; break;
                     case '  |=': value = ASGOR; break;
                     case ' ...': value = ELIPSIS; break;
                     default: value = 0; break;
                   }
               }                               
             else if (ttype == PUNCT)
               switch (*text)
                 {
                   case ',': value = COMMA; break;
                   case '{': value = LCURLY; break;
                   case '}': value = RCURLY; break;
                   case ':': value = COLON; break;
                   case ';': value = SEMICOLON; break;
                   case '[': value = LSQUARE; break;
                   case ']': value = RSQUARE; break;
                   case '(': value = LPAREN; break;
                   case ')': value = RPAREN; break;
                   default: value = 0;
                 }
           if (ttype == DLIT)
              tok->value.dval = dval;
           else if (ttype == FLIT)
              tok->value.fval = dval;
           else if (islong)
              tok->value.lval = value;
            else
              tok->value.ival = value;   
           tok->ttype = ttype; 
           if (strlenwarning)
              mccwarning(MCCSTRLITOVFL,tok);
           if (strlitNewline)
              mccwarning(MCCSTRLITNL,tok);
#          if DEBUG && 1                                               
             if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
                mcctoktrace(tok,"mcctok");
#          endif   
          }
      }                                                                
  mccsrc->curchar = p;
  if (incomment && tok && tok->ttype == EOFILE)
     MCCERR(MCCEOFCOMMENT);
  mccntokens++;
  return tok;

}

/***************************************************** end mcctok   **/

#if DEBUG && 1                                               
/********************************************************** mcctoktrace */
/*-------------------------------------------------------------------*/
/* NAME: mcctoktrace                                                 */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: write info to trace file concerning a new token          */
/* USAGE:                                                            */
  void mcctoktrace( tok ,who )
/* PARAMETERS:                                                       */
tokenp tok; 
char  *who;
{
/* RETURNS:                                                          */
/*    nothing                                                        */
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
/********************************************************** mcctoktrace  **/

             if (mccoption[MCCOPTTRACE] && mcctrcoption['t'])
                MCCWRITE(MCCLISTF,
"%s: tok@%08x,id=%d, loc=%d/%d/%d, text='%s',\n\
         ttype=%d=%s,flags=%08x %c%c%c%c%c value=0x%08lx=%luu=%ld\n",18,
                     who,
                     tok, tok->id,
                     tok->sourceLoc.file,tok->sourceLoc.line,
                     tok->sourceLoc.column,
                     (tok->text) ? tok->text : "***EMPTY***",
                     tok->ttype,mccttypenames[tok->ttype],
                     tok->flags,
                     (tok->flags & TFUnsigned) ? 'U' : ' ',
                     (tok->flags & TFLong) ? 'L' : ' ',
                     (tok->flags & TFShort) ? 'S' : ' ',
                     (tok->flags & TFFloat) ? 'F' : ' ',
                     (tok->flags & TFDouble) ? 'D' : ' ',
                     tok->value.lval, 
                     tok->value.lval, 
                     tok->value.lval, 
                    0);
}
#endif   

/********************************************************** mcctokcopy */
/*-------------------------------------------------------------------*/
/* NAME: mcctokcopy                                                  */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: make a copy of a token                                   */
/* USAGE:                                                            */
  tokenp mcctokcopy( orig )
tokenp orig;  /* or NULL to create a new token from scratch          */
/* PARAMETERS:                                                       */
{
/* RETURNS:                                                          */
/*    pointer to new token structure allocated                       */
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
char   *text;
int    len;
tokenp tok;           /* pointer to token structure being built */
/********************************************************** mcctokcopy  **/

  /* we now need to allocate a token structure and fill it in.            */
  if (mccfreetoks)
    {
      tok = mccfreetoks;
      mccfreetoks = tok->right;
    }
   else
     tok = (tokenp) malloc(sizeof(token));                    
  if (tok == NULL)                                                               
      {
        MCCABORT(MCCENOMEM);
      }
    else
      {    
        if (orig)
           *tok = *orig;
         else
           {
             tok->sig = SIGTOK;
             tok->refcnt = 0;
             tok->text = NULL;
             tok->value.lval = 0;
             tok->sourceLoc.file = 0;
             tok->sourceLoc.line = 0; 
             tok->sourceLoc.column = 0;                                
             tok->ttype = NULLTOK;
             tok->flags = 0;
            }
        tok->flags |= TFmalloced;
        tok->right = tok->left = NULL;
        tok->id = ++mcctokenid;
        tok->refcnt = 0;                                                      
        if (orig && orig->text)
          {
            text = orig->text;
            len = strlen(text);
            tok->text = malloc(len+1);
            if (tok->text == NULL)
               MCCABORT(MCCENOMEM);
            memcpy(tok->text,text,len);
            tok->text[len] = 0;
         }
#          if DEBUG && 1                                               
             if (orig && mccoption[MCCOPTTRACE] && mcctrcoption['t'])
                mcctoktrace(tok,"mcctokcopy");
#          endif   
      }                                                                
  return tok;

}

/***************************************************** end mcctokcopy   **/



/********************************************************** mccpushtok   */
/*-------------------------------------------------------------------*/
/* NAME: mccpushtok                                                  */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89 6                                                    */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: push a token back on to token stack                      */
/* USAGE:                                                            */
  void mccpushtok(tok )
/* PARAMETERS:                                                       */
tokenp tok;               
{
/* RETURNS:                                                          */
/*    pointer to new token structure allocated                       */
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
sourcep src;
/********************************************************** mccpushtok  **/
 src = mccsrc;
 tok->right = mccsrc->tokq;
 tok->left = NULL;
 if (mccsrc->tokq)
    mccsrc->tokq->left = tok;
 mccsrc->tokq = tok;
}


