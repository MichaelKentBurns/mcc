#include "proto.h"
#include "typedecl.h"
/*-------------------------------------------------------------------*/
/* Copyright (c) 1989 by SAS Institute, Inc. Austin, Texas.          */
/* NAME:     typestring                                              */
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
/* PURPOSE: form a printable C representation of a data def chain    */
/*                                                                   */
/*  Internal functions in this module                                */
    int   typestring __ARGS((tNodeP node, char buf[]));   
/* INCLUDE .h                                                        */
#     include "types.h"
#     include "typedecl.h"
/* GLOBAL DECLARATIONS:                                              */
/* EXTERNAL INTERFACES:                                              */
/* HISTORY:                                                          */
/*                                                                   */
/* NOTES:                                                            */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/


/********************************************************** typestring */
/*-------------------------------------------------------------------*/
/* NAME: typestring                                                  */
/* DOCUMENT: none                                                    */
/* AUTHOR: Michael Burns                                             */
/* DATE: 10mar89                                                     */
/* SUPPORT: Michael Burns                                            */
/* LANGUAGE: C                                                       */
/* MACHINE: portable                                                 */
/* BRIDGED:  none                                                    */
/* PURPOSE: form a printable C representation of a data def chain    */
/* USAGE:                                                            */
 int typestring ( node, buf )
/* PARAMETERS:                                                       */
  tNodeP node;    /* (I) pointer to first node in chain              */
  char   buf[];   /* (O) buffer to receive C declaration             */
{
/* RETURNS:  0 if all ok, else line number where error detected      */
#ifdef __LINE__
#   define ERROR __LINE__
#else
#   define ERROR 1
#endif
/* FATAL ERRORS:                                                     */
#ifdef DEBUG
/*                                                                   */
#endif
/* EXTERNAL INTERFACES:                                              */
/* HISTORY:                                                          */
/* NOTES:                                                            */
/*                                                                   */
/* ALGORITHM:                                                        */
/*    1) translates from most derived node in chain to absolute base */
/*       type by starting pointers left and right at the middle of   */
/*       a large temp buffer.  Each node is translated by adding     */
/*       more text to the right or left.  When all nodes done then   */
/*       we simply copy the resultant text (between left and right)  */
/*       from temp buffer to callers buffer.                         */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/
#define  TEMPSIZE 200    /* big enough to hold largest ANSI declr    */
char     temp[TEMPSIZE]; /* temporary buffer to expand text into     */                            
char    *left;           /* points to next position to fill to left  */
char    *right;          /* points to next position to fill to right */
char    *p;              /* points to source of token to copy        */                   
char    c;               /* temp char being copied                   */
int     len;             /* temp length of string being copied       */
int32   flags;           /* copy of flags from this node             */
int32   lastflags;       /* looks back to node just left             */
int32   cumflags;        /* accumulative flags through whole path    */
int     done;            /* flag set to stop evaluation              */
/********************************************************** typestring */
                                                    
*buf = 0;
left = temp + (TEMPSIZE/2);
right = left + 1;                                                      
cumflags = 0;

for (lastflags = 0, done = FALSE;
     node != NULL && !done;
     lastflags = flags, node = node->basetype)
  {
     flags = node->flags;
     cumflags |= flags;
     switch (flags & tDERIVATION)
       {
         case 0:   /* not derived, must be either var or base type */
             if (flags & tBASETYPE)
                {
                  switch (flags & tBASETYPE)
                    { 
                      case tVOID:    p = "void ";     break;
                      case tCHAR:    p = "char ";     break;
                      case tINT:     p = "int ";      break;
                      case tFLOAT:   p = (flags & tDOUBLE) ? "double " : "float ";    break;
                      case tSTRUCT:  
                      case tUNION:                          
                         /* might put out details here in future */
                      case tENUM:
                         if (node->details)
                           {
                             *left-- = ' ';
                             *left-- = '}';
                             *left-- = '{';
                           }
                         if (p = node->tag)
                           {
                             *left-- = ' ';
                             for (len = 0; 
                                  *p;
                                  len++,p++)
                                 ;  /* get to end of string */
                             while (len--)  
                                  *left-- = *(--p);
                           }              
                         if (flags & tSTRUCT)
                             p = "struct ";
                         else if (flags & tUNION)
                             p = "union ";
                         else p = "enum ";
                        /* these are treed as derivatives of void, but dont want to say void */
                        done = TRUE;
                        break;
                      default:
                         /* error: cant have multiple base types */
                         return ERROR;
                    }
                  for (len = 0; 
                       *p;
                       len++,p++)
                      ;  /* get to end of string */
                  while (len--)  
                       *left-- = *(--p);

                  /* now handle modifiers */
                  if (flags & tLONG)
                    {
                      for (len = 0, p="long ";
                           *p;
                           len++,p++)
                           ;  /* get to end of string */
                       while (len--)  
                                  *left-- = *(--p);
                    }
                  if (flags & tSHORT)
                    {
                      for (len = 0, p="short ";
                           *p;
                           len++,p++)
                           ;  /* get to end of string */
                       while (len--)  
                                  *left-- = *(--p);
                    }
                  if (flags & tUNSIGNED)
                    {
                      for (len = 0, p="unsigned ";
                           *p;
                           len++,p++)
                           ;  /* get to end of string */
                       while (len--)  
                                  *left-- = *(--p);
                    }
                  if (cumflags & tVOLATILE)
                    {
                      for (len = 0, p="volatile ";
                           *p;
                           len++,p++)
                           ;  /* get to end of string */
                       while (len--)  
                                  *left-- = *(--p);
                    }                          
                  if (cumflags & tNOALIAS)
                    {
                      for (len = 0, p="noalias ";
                           *p;
                           len++,p++)
                           ;  /* get to end of string */
                       while (len--)  
                                  *left-- = *(--p);
                    }                         
                  if (cumflags & tCONST)
                    {
                      for (len = 0, p="const ";
                           *p;
                           len++,p++)
                           ;  /* get to end of string */
                       while (len--)  
                                  *left-- = *(--p);
                    }

                  /* handle storage classes */
                  if (cumflags & tEXTERN)
                    {
                      for (len = 0, p="extern ";
                           *p;
                           len++,p++)
                           ;  /* get to end of string */
                       while (len--)  
                                  *left-- = *(--p);
                    }
                  if (cumflags & tSTATIC)
                    {
                      for (len = 0, p="static ";
                           *p;
                           len++,p++)
                           ;  /* get to end of string */
                       while (len--)  
                                  *left-- = *(--p);
                    }
                  if (cumflags & tAUTO)
                    {
                      for (len = 0, p="auto ";
                           *p;
                           len++,p++)
                           ;  /* get to end of string */
                       while (len--)  
                                  *left-- = *(--p);
                    }
                  if (cumflags & tREGISTER)
                    {
                      for (len = 0, p="register ";
                           *p;
                           len++,p++)
                           ;  /* get to end of string */
                       while (len--)  
                                  *left-- = *(--p);
                    }

                  /* we had better be done now! */
                  if (node->basetype != NULL && !done)
                     return ERROR;
                }
             else if (flags & (tTYPEDEF | tVARDEF))
                {                           
                  /* copy name of typedef or variable right to left */
                  if (p = node->name)
                    {
                      for (len = 0; 
                           *p;
                           len++,p++)
                           ;  /* get to end of string */
                      while (len--)  
                           *left-- = *(--p);
                    }
                }
             else 
                { /* unrecognized node, punt */
                  return ERROR;
                }
             break;
         case tARRAY:
         case tFUNCTION:
             if (lastflags & tPOINTER)
                {
                  *left-- = '(';
                  *right++ = ')';
                }                
             if (flags & tARRAY)
                {
                  *right++ = '[';                     
                  if (node->dimension)
                    {
                     sprintf(right,"%d",node->dimension);
                     while (*right) 
                       right++;
                    }
                  *right++ = ']';
                }
              else /* must be tFUNCTION */
                {                
                  *right++ = '(';
                  /* might possibly do prototype here in future */
                  *right++ = ')';
                }
             break;
         case tPOINTER:
               *left-- = '*';
             break;
         default:
           /* internal error: multiple derivations at one node */
           return ERROR;  
       } 
  }                               

/* now copy from left to right, into users buffer */
for (p = ++left, *right = 0;
     *p;
    )
  *buf++ = *p++;
while (buf[-1] == ' ')
  buf--;
*buf = 0;

 return FALSE;  /* normal completion */
}
/****************************************************** end typestring */

