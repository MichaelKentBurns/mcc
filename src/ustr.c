/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME: ustr.c                                                      */
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
/* PURPOSE: lookup tables for unique strings.                        */
/*                                                                   */
/*  Internal functions in this module                                */
/* INCLUDE .h                                                        */
#include "ustr.h"
#include "ustrpriv.h"                    
#include <stdio.h>
#include <memory.h>
#ifdef is_mcc_build
#   include "mcc.h"
#endif
/* GLOBAL DECLARATIONS:                                              */
/* EXTERNAL INTERFACES:                                              */
/* HISTORY:                                                          */
/*                                                                   */
/* NOTES:                                                            */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/

int ustrAddNode (ustrPoolP pool, ustrNodeP node);                

/******************************************************* ustrNewPool */
/*-------------------------------------------------------------------*/
/* NAME: ustrNewPool                                                 */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE:  create a new ustr pool                                  */
/* USAGE:                                                            */
 ptr ustrNewPool (
        int nquick,          /* (I) number of entries in quick table */ 
        long baseid,         /* (I) lowest id to use for this pool   */
        ptr (*quicktab)[]    /* (I) address of quick table           */    
 )
{
/* RETURNS:                                                          */
/*   pointer to new pool or NULL if problems                         */
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
ustrPoolP  pool;

/****************************************************** ustrNewPool **/
if ((pool = (ustrPoolP) malloc(sizeof(ustrPool))) != NULL)
  {          
    pool->nquick = nquick;
    pool->baseid = baseid;
    pool->root = NULL;
    pool->quicklist = quicktab;
    pool->topindex = NULL;
    pool->nsearches = pool->nsearchsteps = pool->maxsearchdepth =
         pool->curnnodes = pool->maxnnodes = 0;
  }
 return (ptr) pool;
}
/************************************************** end ustrNewPool **/

/****************************************************** ustrAddIndex */
/*-------------------------------------------------------------------*/
/* NAME: ustrAddIndex                                                */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE:  add a top level index to a pool                         */
/* USAGE:                                                            */
int ustrAddIndex (ptr poolptr)                 
{
/* RETURNS:                                                          */
/*  0 if ok, else error code: ???                                    */   
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
int i;
ustrPoolP pool = (ustrPoolP) poolptr;
ustrNodeP *pp;
/****************************************************** ustrAddIndex **/
if (pool != NULL)
  {          
    pool->topindex = pp = (ustrNodeP *) malloc(128*sizeof(ustrNodeP));
    if (pool->topindex)
      {
       for (i=0; i<128; pp++, i++)
         *pp = NULL;
       return(0);
      }
     else
       return(-1);    
  }
 else
   return (-1);
}
/************************************************* end ustrAddIndex **/
/***************************************************** ustrDumpPool  */
/*-------------------------------------------------------------------*/
/* NAME: ustrDumpPool                                                */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: dump a pool to stdout.                                   */
/* USAGE:                                                            */
 int ustrDumpPool (file, pool, node, eldump)                 
/* PARAMETERS:                                                       */
 ptr file;  /* NULL to dump to stdout */
 ptr pool;                      
 ptr node;  /* NULL to dump entire pool */ 
 void (*eldump)();  /* function to dump element or NULL */
{
/* RETURNS:                                                          */
/*   number of nodes                                                 */
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
ustrPoolP poolp;
ustrNodeP nodep;
char      *text;
int       nnodes; 
FILE      *fd;
int       i;
/********************************************************** ustrDumpPool **/
poolp = (ustrPoolP) pool;
nodep = (ustrNodeP) node;
fd = (FILE *) file;
if (fd == NULL)
   fd = stdout;
nnodes = 0;
if (nodep == NULL)
  {
   if (poolp->topindex)
     {
      for (i = 0; i < 128; i++)
        if ((nodep = poolp->topindex[i]))
           nnodes += ustrDumpPool(file,pool,(ptr) nodep,eldump); 
      return(nnodes);
     }
    else
      nodep = poolp->root; 
  }
if (nodep)                    
  {                                          
    if (nodep->left)
       nnodes += ustrDumpPool(file,pool,(ptr) nodep->left,eldump); 
    text = USTRNODETEXT(nodep);
    fprintf(fd,"  id=%08x text=%s\n",nodep->id,text);     
    if (eldump)
       (*eldump)(nodep->id,fd);
    nnodes++;
    if (nodep->right)
       nnodes += ustrDumpPool(file,pool,(ptr) nodep->right,eldump);
  }          
return nnodes;
}
/****************************************************** end ustrDumpPool **/
/***************************************************** ustrTraversePool  */
/*-------------------------------------------------------------------*/
/* NAME: ustrTraversePool                                                */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: traverse a pool and call eldump for each item            */
/* USAGE:                                                            */
 int ustrTraversePool (pool, node, eldump)                 
/* PARAMETERS:                                                       */
 ptr pool;                      
 ptr node;  /* NULL to dump entire pool */ 
 void (*eldump)();  /* function to dump element or NULL */
{
/* RETURNS:                                                          */
/*   number of nodes                                                 */
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
ustrPoolP poolp;
ustrNodeP nodep;
char      *text;
int       nnodes; 
int       i;
/********************************************************** ustrTraversePool **/
poolp = (ustrPoolP) pool;
nodep = (ustrNodeP) node;
nnodes = 0;
if (nodep == NULL)
  {
   if (poolp->topindex)
     {
      for (i = 0; i < 128; i++)
        if ((nodep = poolp->topindex[i]))
           nnodes += ustrTraversePool(pool,(ptr) nodep,eldump); 
      return(nnodes);
     }
    else
      nodep = poolp->root; 
  }
if (nodep)                    
  {                                          
    if (nodep->left)
       nnodes += ustrTraversePool(pool,(ptr) nodep->left,eldump); 
    text = USTRNODETEXT(nodep);
    if (eldump)
       (*eldump)(nodep->id);
    nnodes++;
    if (nodep->right)
       nnodes += ustrTraversePool(pool,(ptr) nodep->right,eldump);
  }          
return nnodes;
}
/***************************************************** ustrPoolStats */
/*-------------------------------------------------------------------*/
/* NAME: ustrPoolStats                                               */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: free a pool.                                             */
/* USAGE:                                                            */
 void ustrPoolStats (ptr pool, FILE* fd)
{
/* RETURNS:                                                          */
/*   number of nodes                                                 */
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
ustrPoolP poolp;
ustrNodeP nodep;
char      *text;
int       nnodes; 
boolean   isroot;
int       i;
int       slotsused;
/********************************************************** ustrPoolStats **/
poolp = (ustrPoolP) pool;
fprintf(fd,"Statistics for ustr pool at %08lx\n",poolp);
nnodes = 0;
   if (poolp->topindex)
     {
      for (i = 0, slotsused = 0; i < 128; i++)
        if ((nodep = poolp->topindex[i]))
           slotsused++;
      fprintf(fd,"     %d index slots occupied.\n",slotsused);
     }
   fprintf(fd,"     %d nodes currently, %d maximum.\n",   
        poolp->curnnodes,poolp->maxnnodes);
   fprintf(fd,"     %d searches by name, searchlength average = %d, max=%d\n",
         poolp->nsearches,
         (poolp->nsearches) ? poolp->nsearchsteps/poolp->nsearches : 0,
         poolp->maxsearchdepth);
return;
}
/************************************************ end ustrPoolStats **/

/***************************************************** ustrFreePool  */
/*-------------------------------------------------------------------*/
/* NAME: ustrFreePool                                                */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: free a pool.                                             */
/* USAGE:                                                            */
 int ustrFreePool (pool, node, nodefree)                 
/* PARAMETERS:                                                       */
 ptr pool;                      
 ptr node;  /* NULL to dump entire pool */
 void (*nodefree)();  /* routine to free user data for a node */
{
/* RETURNS:                                                          */
/*   number of nodes                                                 */
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
ustrPoolP poolp;
ustrNodeP nodep;
char      *text;
int       nnodes; 
FILE      *fd;
boolean   isroot;
int       i;
/********************************************************** ustrFreePool **/
poolp = (ustrPoolP) pool;
nodep = (ustrNodeP) node;
nnodes = 0;
if (isroot = (nodep == NULL))
  {
   if (poolp->topindex)
     {
      for (i = 0; i < 128; i++)
        if ((nodep = poolp->topindex[i]))
           nnodes += ustrFreePool(pool,(ptr) nodep,nodefree); 
      return(nnodes);
     }
    else
      nodep = poolp->root; 
  }
if (nodep)                    
  {                                          
    if (nodep->left)
       nnodes += ustrFreePool(pool,(ptr) nodep->left,nodefree); 
    if (nodep->right)
       nnodes += ustrFreePool(pool,(ptr) nodep->right,nodefree);
    if (nodefree)
      (*nodefree)(nodep->id);
    if (!nodep->t.longText.isConst)
       free(nodep->t.longText.text);
    free(nodep);
    nnodes++;
  }   
if (isroot)
 {
  if (poolp->topindex)
    free(poolp->topindex);
  free(poolp);       
 }
return nnodes;
}
/****************************************************** end ustrFreePool **/

/***************************************************** ustrAddConst  */
/*-------------------------------------------------------------------*/
/* NAME: ustrAddConst                                                */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: add a node to the pool using a pointer to constant text. */
/* USAGE:                                                            */
 long ustrAddConst (ptr poolp, ptr text, long id) 
{
/* RETURNS:                                                          */
/*   id of newly added member (same as input id) or 0 if errors      */
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
ustrNodeP  newnode;
ustrPoolP  pool;
/********************************************************** ustrAddConst **/
pool = (ustrPoolP) poolp;
newnode = (ustrNodeP) malloc(sizeof(*newnode));
if (newnode)                    
  {     
   newnode->t.longText.isNotValid = FALSE;
   newnode->t.longText.isConst = TRUE;
   newnode->t.longText.text = text;    
   if (id == 0)
     id = pool->baseid++;
   newnode->id = id;
   if (!ustrAddNode(pool,newnode)) /* got added ok ? */ 
     {
      if (id <= pool->nquick)
        *(pool->quicklist)[id] = text; 
      return id;  /* normal completion */
     }
    else  /* must have been a duplicate */
      free(newnode);
  }          
 else
  return 0;  /* abnormal completion */

}
/****************************************************** end ustrAddConst **/
/***************************************************** ustrAdd  */
/*-------------------------------------------------------------------*/
/* NAME: ustrAdd                                                */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: add a node to the pool using a pointer to constant text. */
/* USAGE:                                                            */
 long ustrAdd (poolp,text,id)                 
/* PARAMETERS:                                                       */
 ptr poolp;
 ptr text;
 long id;
{
/* RETURNS:                                                          */
/*   id of newly added member (same as input id) or 0 if errors      */
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
ustrNodeP  newnode;
ustrPoolP pool; 
char *textp;
int len;
/********************************************************** ustrAdd **/
pool = (ustrPoolP) poolp;
newnode = (ustrNodeP) malloc(sizeof(*newnode));
if (newnode)                    
  {     
   len = strlen(text) + 1;                              
   if (len > USTRSHORTLEN)
      {
       newnode->t.longText.isNotValid = FALSE;
       newnode->t.longText.isConst = FALSE;
       textp = newnode->t.longText.text = malloc(len);    
       if (!textp)
         {
          free(newnode);
          return 0;   /* abnormal completion */
         }                                     
       strcpy(textp,text);
      }
     else
      {                                   
       textp = NULL;
       newnode->t.longText.isNotValid = TRUE;
       strcpy(newnode->t.shortText,text);
      }
   if (id == 0)
     id = pool->baseid++;
   newnode->id = id;
   if (!ustrAddNode(pool,newnode)) /* got added ok ? */ 
     {
      if (id <= pool->nquick)
        *(pool->quicklist)[id] = text; 
      return id;  /* normal completion */
     }
    else  /* must have been a duplicate */
     {
      free(newnode);
      if (textp)
        free(textp);
     }
  }          
 else
  return 0;  /* abnormal completion */

}
/****************************************************** end ustrAdd **/


/***************************************************** ustrAddQuickConst  */
/*-------------------------------------------------------------------*/
/* NAME: ustrAddQuickConst                                           */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: add text to quicktab without adding a node to the pool.  */
/* USAGE:                                                            */
 long ustrAddQuickConst (poolp,text,id)                 
/* PARAMETERS:                                                       */
 ptr poolp;
 ptr text;
 long id;
{
/* RETURNS:                                                          */
/*   id of newly added member (same as input id) or 0 if errors      */
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
ustrPoolP pool;
/********************************************************** ustrAddQuickConst **/
   pool = (ustrPoolP) poolp;
   *(pool->quicklist)[id] = text;
   return id;

}
/************************************************* end ustrAddQuickConst **/

/***************************************************** ustrAddNode  */
/*-------------------------------------------------------------------*/
/* NAME: ustrAddNode                                                 */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: add a node to the pool using a pointer to node           */
/* USAGE:                                                            */
int ustrAddNode (pool,node)                 
/* PARAMETERS:                                                       */
 ustrPoolP pool;
 ustrNodeP node;
{
/* RETURNS:                                                          */
/*   0 if ok, else 1 if duplicate node                               */
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
ustrNodeP  testnode;               
ustrNodeP  parent;
ptr        newtext;
ptr        testtext;
int        difference;
ustrNodeP  *rootp;
/********************************************************** ustrAddNode **/
  newtext = USTRNODETEXT(node);
  parent = NULL;
  if (pool->topindex)  
     rootp = &(pool->topindex[*newtext]);
   else
     rootp = &(pool->root);
  if (*rootp == NULL)
     difference = 1;
    else
     for (testnode = *rootp, difference = 1;
          testnode != NULL && difference;
          testnode = (difference > 0) ? testnode->right : testnode->left)
       {
         testtext = USTRNODETEXT(testnode);
#if 1
         difference = *newtext - *testtext;
         if (!difference)
#endif
            difference = strcmp(newtext,testtext);
         parent = testnode;
       }
  if (difference) /* then, no identical match */
    {
      if (parent == NULL)
         *rootp = node;
       else
         if (difference > 0)
             parent->right = node;
           else
             parent->left = node;
      node->right = node->left = NULL;
      pool->curnnodes++;
      if (pool->curnnodes > pool->maxnnodes)
          pool->maxnnodes = pool->curnnodes;
      return 0; /* normal completion */
    }          
   else
      return 1; /* abnormal completion */

}
/****************************************************** end ustrAddNode **/

/***************************************************** ustrId  */
/*-------------------------------------------------------------------*/
/* NAME: ustrId                                                 */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: find an entry and return its id value                    */
/* USAGE:                                                            */
  long    ustrId(poolp, text) 
/* PARAMETERS:                                                       */
 ptr    poolp;
 ptr    text;
{
/* RETURNS:                                                          */
/*   id of node, else 0 if not found                                 */
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
 ustrPoolP pool;
ustrNodeP  testnode;               
ptr        testtext;
int        difference;
ustrNodeP  *rootp;
int        depth;
/********************************************************** ustrId **/
  depth = 0;
  pool = (ustrPoolP) poolp;
  if (pool->topindex)  
     rootp = &(pool->topindex[*text]);
   else
     rootp = &(pool->root);
  for (testnode = *rootp, difference = 1;
       testnode != NULL;
       testnode = (difference > 0) ? testnode->right : testnode->left)
    {
      depth++; 
      testtext = USTRNODETEXT(testnode);
#if 1
      difference = *text - *testtext;
      if (!difference)
#endif
         difference = strcmp(text,testtext);
#if DEBUG && 0
 printf("comparing '%s' with '%s' difference = %d\n",text,testtext,difference); 
#endif
      if (!difference) 
         break;
    }         
#if DEBUG && 0
 printf("difference = %d, testnode=%08x, testnode->id = %d testnode->text=%s\n",
        difference,testnode,(testnode) ? testnode->id : -1,
                            (testnode) ? testnode->text : "");
#endif
   pool->nsearches++;
   pool->nsearchsteps += depth;
   if (depth > pool->maxsearchdepth)
        pool->maxsearchdepth = depth;
  if (!difference && testnode) /* then, no identical match */
      return testnode->id;     /* normal completion, found it */
   else
      return 0; /* not found */

}
/****************************************************** end ustrId **/

/***************************************************** ustrText  */
/*-------------------------------------------------------------------*/
/* NAME: ustrText                                                 */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: find an entry and return its text value                  */
/* USAGE:                                                            */
  char*    ustrText(ptr poolp, unsigned long id) 
{
/* RETURNS:                                                          */
/*   text of node, else NULL if not found                            */
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
 ustrPoolP pool;
ustrNodeP  testnode;               
int        difference;
ustrNodeP  *rootp;
ustrNodeP  *pp;
int        i;
/********************************************************** ustrText **/
  pool = (ustrPoolP) poolp;                             
  pp = pool->topindex;
  rootp = NULL;
  i = 0;
  while (1)
    {
     if (pp)  
       {
         while (i < 128 && *pp)
            {
             pp++;
             i++;
            }
         if (i < 128)
            rootp = &(pool->topindex[i]);
          else
            break;
       }
      else
       {
        if (rootp)
          break;
        else
          rootp = &(pool->root);
       }
     if (id < pool->nquick)
        return (*pool->quicklist)[id];
      else
        {
         for (testnode = *rootp, difference = 1;
               testnode != NULL;
               testnode = (difference > 0) ? testnode->right : testnode->left)
            {
              difference = id - testnode->id;;
              if (!difference) 
                 break;
            }
          if (!difference && testnode) /* then, no identical match */
              return USTRNODETEXT(testnode);
        }
   } /* end while */
  return NULL;  /* not found */
 }
/****************************************************** end ustrText **/


/***************************************************** ustrMoveTree  */
/*-------------------------------------------------------------------*/
/* NAME: ustrMoveTree                                                 */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: remove node from the pool using a pointer to name        */
/* USAGE:                                                            */
static unsigned long ustrMoveTree (pool, node)                 
/* PARAMETERS:                                                       */
 ustrPoolP pool;               
 ustrNodeP node;
{
/* RETURNS:                                                          */
/*   void                                                            */
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
/********************************************************** ustrMoveTree **/
  if (node)
    {
     if (node->left)
        ustrMoveTree(pool,node->left);
     if (node->right)
        ustrMoveTree(pool,node->right);
     node->right = node->left = NULL;
     ustrAddNode(pool,node);
    }
}
/****************************************************** end ustrMoveTree **/

/***************************************************** ustrRemText  */
/*-------------------------------------------------------------------*/
/* NAME: ustrRemText                                                 */
/* DOCUMENT: internal                                                */
/* AUTHOR: Michael Burns                                             */
/* DATE: 2feb89                                                      */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: apollo                                                   */
/* BRIDGED:  none                                                    */
/* PURPOSE: remove node from the pool using a pointer to name        */
/* USAGE:                                                            */
unsigned long ustrRemText (ptr poolptr, char *text)                 
{
/* RETURNS:                                                          */
/*   id of removed node if found, else NULL if not found             */
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
ustrPoolP  pool = (ustrPoolP) poolptr;
ustrNodeP  node;
ustrNodeP  testnode;               
ustrNodeP  parent;
ustrNodeP  heir,orphan;
ptr        testtext;
int        difference;    
unsigned long id;
ustrNodeP  *rootp;

/********************************************************** ustrRemText **/
  node = NULL;
  parent = NULL;
  if (pool->topindex)  
     rootp = &(pool->topindex[*text]);
   else
     rootp = &(pool->root);
  if (*rootp == NULL)
     difference = 1;
    else
     for (testnode = *rootp, difference = 1;
          testnode != NULL && difference;
          testnode = (difference > 0) ? testnode->right : testnode->left)
       {
         testtext = USTRNODETEXT(testnode);
#if 1
         difference = *text - *testtext;
         if (!difference)
#endif
            difference = strcmp(text,testtext);
         if (!difference)
            node = testnode;
           else
            parent = testnode;
       }
  if (node)
    {  /* found it, now remove it */
               
      if (node->left) 
        {
         heir = node->right;
         orphan = node->left;
        }
       else                  
        {
         heir = node->left;
         orphan = node->right;
        }                     
      if (parent)
        {
         if (parent->left == node)
             parent->left = heir;
           else
             parent->right = heir;
        }
       else
        {
          *rootp = heir;
        }                   
      if (orphan)
        {
         ustrMoveTree(pool,orphan);
        }      
      id = node->id; 
      if (!node->t.longText.isNotValid)      
        if (!node->t.longText.isConst)
           free(node->t.longText.text);
      free(node);
      pool->curnnodes--;
      return id; /* normal completion */
    }          
  else
    return NULL;  /* no match */

}
/****************************************************** end ustrRemText **/

