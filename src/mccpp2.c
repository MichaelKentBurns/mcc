extern char *malloc();           
#define mlbfix
#include "mcc.h"
#include <time.h>
/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME:     mccpp    part II.                                       */
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
        tokenp mccendline __ARGS(( int, line ));
        tokenp mccppexp __ARGS(( tokenp ));
/* INCLUDE .h                                                        */
/* GLOBAL DECLARATIONS:                                              */
/* EXTERNAL INTERFACES:                                              */
/* HISTORY:                                                          */
/*                                                                   */
/* NOTES:                                                            */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/
/* NAME: mccendline                                                   */

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


/*-------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------*/
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

