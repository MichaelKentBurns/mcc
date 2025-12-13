#define MCCVERSIONSTRING  "0.86"
#define MCCVERSIONLONG  " version 0.86"
#define DEBUG 1
/*-------------------------------------------------------------------*/
/* Copyright (c) 1986 by SAS Institute, Inc. Austin, Texas.          */
/* NAME: mcc.h                                                       */
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
/* PURPOSE: define data types for c compiler                         */
/*                                                                   */
/* NOTES:                                                            */
/*                                                                   */
/* END                                                               */
/*-------------------------------------------------------------------*/
/*34567890123456789012345678901234567890123456789012345678901234567890*/
/*       1         2         3         4         5         6         7*/

#include "types.h"
#include "proto.h"
#include "ansi.h"              
#include "mcccfg.h"
#ifndef BUFSIZE
#  include <stdio.h>
#endif
#include "mccmem.h"

/*-------------------------------------------------------------------*/  
typedef int16 signature;   /* many structures have unique signatures */

/* SVALUE: contains a scalar value */
typedef union SVALUE
            {
             char           *pval;
             char           cval; 
             short          sval;
             unsigned short usval;        
             int            ival;
             unsigned int   uival;
             long int       lval;
             unsigned long  ulval;
             float          fval;
             double         dval;
            } svalue;
                                                                    
/* SLOC: uniquely identifies the location in one of the src files */
typedef struct SLOC 
            {      
              int16        file;
              int16        line;
              int16        column;
            } sloc;

#define structhdr(ptype)                                     \
       signature sig;      /* structure signature */         \
       ptype    left;      /* doubly linked list or tree */  \
       ptype    right;                                       \
       int      id;        /* unique integer identifier */   \
       int16    refcnt;    /* number of references to this structure */

                                                                       
/* TOKEN : represents a unique lexical token from the source file */
typedef struct TOKEN *tokenp;
typedef struct TOKEN
      {structhdr(tokenp)
#      define SIGTOK 'TK'
       char      *text;      /* actual text as found in source */
       svalue    value;      /* interp value of token, if known */
       sloc      sourceLoc;  /* location in src */
       char      ttype;      /* lexical type of this token: */
#                  define EOFILE 0   /* end of file */
#                  define EOLN   1   /* end of line */
#                  define ID     2   /* identifier */
#                  define KEY    3   /* language keyword */
#                  define PPFLAG 4   /* preprocessor sentinal # */
#                  define PPID   5   /* preprocessor identifier */
#                  define PPOP   6   /* preprocessor operator */
#                  define EOP    7   /* expression operator */
#                  define PUNCT  8   /* punctuation */
#                  define ILIT   9   /* integer literal */
#                  define SLIT   10  /* string literal */
#                  define FLIT   11  /* float literal */
#                  define DLIT   12  /* double literal */
#                  define PPARG  13  /* preprocessor macro arg */
#                  define COMMENT 14 /* comment text */
#                  define MARKER 15  /* special internal marker token */
#                  define ILLTOK 16  /* illegal token */
#                  define NULLTOK 17  /* non-existent token */
/*  NOTE: if you add anything to this list add it also to the list below */
          unsigned int flags;
#define TFUnsigned (1 << 0)
#define TFLong     (1 << 1)
#define TFShort    (1 << 2)
#define TFFloat    (1 << 3)
#define TFDouble   (1 << 4)
#define TFmalloced (1 << 5)  /* this tok was malloc'd free it, else static */
      } token;

/* SOURCE : represents an open source file */
typedef struct SOURCE *sourcep;
typedef struct SOURCE  
       { structhdr(sourcep)
#      define SIGSRC 'SF'
         ptr  pathname; /* complete pathname */
         ptr  inpath;  /* pathname as appeared in #include */
         FILE *fd;            
         int  bufsize; 
         char *buffer;
         char *end;
         char *curchar;
         char *linestart;
         int  lineno;
         int  eof;
         tokenp tokq;
         int  incdepth;
         int  flags;
#define MCCUSERH  (1<<1)
#define MCCSYSH   (1<<2)
#define MCCNOLIST (1<<3)
       } source;

/* DTYPE: represents a data type */
typedef struct DTYPE *dtypep;
typedef struct DTYPE 
      {structhdr(dtypep) 
#      define SIGDTYPE 'DT'
       char     modflg;  /* type modifier flags */
       char     depth; 
       dtypep   from;    /* ptr to type derived from */
       char    *name;    /* name of this type or NULL */
       int      size;       /* size in bytes of this type */
       int      dimension;  /* array dimension if array */
      } dtype;

/* VAR: represents an actual variable */
typedef struct VAR *varp;
typedef struct VAR 
      {structhdr(varp)
#      define SIGVAR 'VA'
       char     duration;  
#                 define DURSTATIC  0
                  /* otherwise automatic, nesting level >= 1 */
       char     *name;
       int      size;
       int      dimension;                                     
       dtypep   type; 
      } var;

  
/* LENT: represents a language entity */
typedef struct LENT *lentp;
typedef struct LENT 
      {structhdr(lentp) 
#      define SIGLENT 'LE'
       char     lentType;  /* language entity type: */
       union {
               struct {         
                       lentp   left;
                       tokenp  op;
                       lentp   right;
                      } binexp;         
               struct {
                       tokenp  op;
                       lentp   right;
                      } preexp;
               struct {
                       lentp   left;
                       tokenp  op;
                      } postexp;
               struct {         
                       lentp   left;
                       tokenp  op;
                       lentp   mid;
                       lentp   right;
                      } ternexp;                        
               struct {        
                       lentp   exp;
                       lentp   thenstmt;
                       lentp   elsestmt;
                      } ifstmt;     
               struct {              
                       char    nestlevel;
                       lentp   parent; 
                       varp    namespace;
                       lentp   stmts;
                      } blockstmt;       
               struct {              
                       lentp   exp;
                       lentp   cases;
                      } switchstmt;  
               struct {          
                       svalue  caseconst;
                       lentp   label;
                      } casestmt;
               struct {          
                       lentp   init;
                       lentp   top;
                       lentp   bottom;
                       lentp   stmt;
                      } loopstmt;    
               struct {            
                       lentp   exp;
                      } returnstmt;   
               struct {           
                       char    *name;
                       lentp   stmt;
                      } labelstmt;
               struct {          
                       lentp   label;
                      } gotostmt;
             } d;
      } lent;


/* keyword tokens */                                         
#define AUTO          21
#define BREAK         22
#define CASE          23
#define CHAR          24 
#define CONST         25
#define CONTINUE      26 
#define DEFAULT       27
#define DO            28
#define DOUBLE        29
#define ELSE          30
#define ENUM          31
#define EXTERN        32
#define FLOAT         33
#define FOR           34
#define GOTO          35
#define IF            36
#define INT           37
#define LONG          38 
#define NOALIAS       39
#define REGISTER      40
#define RETURN        41
#define SHORT         42
#define SIGNED        43
#define SIZEOF        44
#define STATIC        45
#define STRUCT        46
#define SWITCH        47
#define TYPEDEF       48
#define UNION         49
#define UNSIGNED      50
#define VOID          51
#define VOLATILE      52
#define WHILE         53                                   
/* operator tokens */   
#define LSQUARE       54  
#define RSQUARE       55
#define LPAREN        56
#define RPAREN        57
#define ELEM          58
#define ELEMP         59
#define INC           60
#define PREINC        61
#define POSTINC       62
#define DEC           63
#define PREDEC        64
#define POSTDEC       65
#define ADDRESS       66
#define PTR           67
#define PLUS          68
#define MINUS         69
#define ANEG          70
#define BNEG          71
#define NOT           72
/* #define               73 */
#define DIVIDE        74
#define MULT          75
#define MODULUS       76
#define LSHIFT        77
#define RSHIFT        78
#define LESS          79
#define GREATER       80
#define LESSEQ        81
#define GREATEREQ     82
#define EQUAL         83
#define NOTEQUAL      84
#define XOR           85
#define BOR           86
#define BAND          87
#define LOR           88
#define LAND          89
#define TERNOP        90
#define ASG           91
#define ASGMULT       92
#define ASGDIVIDE     93
#define ASGMODULUS    94
#define ASGPLUS       95
#define ASGMINUS      96
#define ASGLSHIFT     97
#define ASGRSHIFT     98
#define ASGAND        99
#define ASGXOR       100
#define ASGOR        101
/* punctuation tokens */
#define COMMA        102
#define PPSENTINAL   103
#define PPLIT        103 /* 104 */
#define PPGLUE       105
#define LCURLY       106
#define RCURLY       107
#define COLON        108
#define SEMICOLON    109
#define ELIPSIS      110
                        
/* non-atomic elements */
/* begin at 128 nice round number */

#define MCCMAXRESERVED 255    /* highest reserved symbol */ 

/* maximum nesting level of macro expansion */
#define MCCMACNESTINV 31 /* cant find ansi std for this number */

                                                                              
/*-------------------------------------------------------------------*/
/* mcc macros */
/*-------------------------------------------------------------------*/
                        
#define MCCERRF mccerrf 
#define MCCLISTF  mcclistf 
#define MCCSRCF   stdin
#define MCCWRITE  mccwrite              
#define MCCERR(code) { MCCWRITE(MCCERRF,mccerrfmt,mccerrmsg[code],"",""); mccerrcount++; }
#define MCCERR1(code,one) { MCCWRITE(MCCERRF,mccerrfmt,mccerrmsg[code],one,""); mccerrcount++; }
#define MCCERR2(code,one,two) { MCCWRITE(MCCERRF,mccerrfmt,mccerrmsg[code],one,two); mccerrcount++; }
#define MCCWARN(code)  { MCCWRITE(MCCERRF,mccwarnfmt,mccerrmsg[code],"","");  mccwarningcount++; }
#define MCCWARN1(code,one)  { MCCWRITE(MCCERRF,mccwarnfmt,mccerrmsg[code],one,"");  mccwarningcount++; }
#define MCCWARN2(code,one,two)  { MCCWRITE(MCCERRF,mccwarnfmt,mccerrmsg[code],one,two);  mccwarningcount++; }
#define MCCABORT(code) { MCCWRITE(MCCERRF,mccabortfmt,mccerrmsg[code],__LINE__,__FILE__); abort(code); }
#define MCCRETURN(code,val) { MCCWRITE(MCCERRF,mccabortfmt,mccerrmsg[code],__LINE__,__FILE__); return(val); }
#if DEBUG
#   define MCCTRC(str)  { MCCWRITE(MCCLISTF,mcctrcfmt,str,__LINE__,__FILE__); }  
#   define MCCTRCd(str,n)  { MCCWRITE(MCCLISTF,mcctrcdfmt,str,n,__LINE__,__FILE__); }  
#   define MCCTRCx(str,n)  { MCCWRITE(MCCLISTF,mcctrcxfmt,str,n,__LINE__,__FILE__); }  
#   define MCCTRCs(str,s)  { MCCWRITE(MCCLISTF,mcctrcsfmt,str,s,__LINE__,__FILE__); }  
#else
#   define MCCTRC(str)
#   define MCCTRCd(str,n)
#   define MCCTRCx(str,n)
#   define MCCTRCs(str,s)
#endif                                                                 
#include "mcchost.h"

/*-------------------------------------------------------------------*/
/* mcc global data defined here */
/*-------------------------------------------------------------------*/

#define USEFILL 0
#ifdef MCCDATA
#   define MCCGLOBAL
#   define MCCINIT(i) = i
#if USEFILL
#   define MCCFILL(name) char name[] = "MCC FILLER on line __LINE__, named name";
#else
#   define MCCFILL(name)
#endif 
#else
#   define MCCGLOBAL extern
#   define MCCINIT(i) 
#   define MCCFILL(name)
#endif                   
                                   
/*******  dynamic data ****************/
MCCGLOBAL int mccmemtot MCCINIT(0);  /* current total of all malloc'd memory */
MCCGLOBAL int mccmempeak MCCINIT(0);  /* peak total of all malloc'd memory */
MCCGLOBAL int mccnmallocs MCCINIT(0);  /* count of calls to mccmalloc */
MCCGLOBAL int mccnrealmallocs MCCINIT(0);  /* count of calls to malloc */
MCCGLOBAL int mccnfrees MCCINIT(0);  /*  count of calls to mccfree */
MCCGLOBAL int mccnrealfrees MCCINIT(0);  /*  count of calls to free */
MCCGLOBAL int mccbigchunks MCCINIT(0);  /*  count of big allocations */
MCCGLOBAL ptr mccgsym;                    /* mcc global symbol table */
MCCGLOBAL ptr mccppsym;                   /* mcc preprocessor symbol table */
#define IFTHEN 1      /* yes, process source, we are in an active #if state */
#define IFELSE 0      /* no, dont process source, we are in an #else state */
#define IFNEVER -1    /* no, dont process source, we are nested below a #else state */
#define MCCELSEMARKER '~'
MCCFILL(fill1)
MCCGLOBAL int mccppiflevel MCCINIT(0); /* mcc preprocessor #if stack level */
MCCFILL(fill2)
MCCGLOBAL short mccppifnest[ANSINESTPPIF]; /* mcc preprocessor #if stack */
MCCFILL(fill3)
MCCGLOBAL ptr mccreserved[MCCMAXRESERVED+1];       /* quick pointers */

MCCFILL(fill4)
MCCGLOBAL int mccerrcount MCCINIT(0);
MCCFILL(fill5)
MCCGLOBAL int mccwarningcount MCCINIT(0);
MCCFILL(fill6)

MCCGLOBAL ptr mccerrpath  MCCINIT(NULL);                               
MCCFILL(fill7)
MCCGLOBAL ptr mccoutpath  MCCINIT(NULL);                               
MCCFILL(fill8)
MCCGLOBAL long int mccerrbegin;
MCCFILL(fill9)
                
MCCGLOBAL sourcep mccsrc   MCCINIT(NULL);
MCCGLOBAL sourcep mccdefsrc MCCINIT(NULL);
MCCFILL(fill10)
MCCGLOBAL sourcep mccoldsources  MCCINIT(NULL);
MCCFILL(fill11)

#define MCCBUFCHECK(p) {mccsrc->curchar = p; \
    if ( (mccsrc->end - mccsrc->curchar) < MCCALMOSTEMPTY \
    && !mccsrc->eof && mccsrc->fd) { mccreadsrc(mccsrc,__FILE__,__LINE__); p = mccsrc->curchar;} } 

MCCGLOBAL int  mccfileid   MCCINIT(0);  /* current file sequence number */
MCCGLOBAL int  mcctokenid  MCCINIT(0);  /* current token number */
MCCGLOBAL int  mccntokens MCCINIT(0);  /* total number of tokens parsed */
MCCGLOBAL tokenp mcctokstack MCCINIT(NULL);  /* stack of untok'd tokens */
MCCGLOBAL tokenp mccfreetoks MCCINIT(NULL);  /* queue of free tokens */
#define NMEMSIZES 20
MCCGLOBAL int **mccfbs[NMEMSIZES]; /* array of ptrs to free blocks*/
MCCGLOBAL int mccfbusage[NMEMSIZES]; /* number of times each used */
MCCGLOBAL int mccfbcur[NMEMSIZES]; /* number of blocks currently */
MCCGLOBAL int mcccleanup MCCINIT(FALSE);  /* TRUE when cleaning up */

MCCGLOBAL FILE *mcclistf  MCCINIT(stdout);                             
MCCGLOBAL int  mccliston MCCINIT(TRUE);
MCCGLOBAL ptr mcclistpath MCCINIT(NULL);
MCCGLOBAL int  mcclistInPmode MCCINIT(FALSE);

MCCGLOBAL ptr mccppopath MCCINIT(NULL);                      
MCCGLOBAL FILE *mccppof MCCINIT(NULL);                      
MCCGLOBAL FILE *mccerrf MCCINIT(stderr);                      
                         
#define         MCCIDIRMAX  50       
MCCGLOBAL int   mccnidirs MCCINIT(0);
MCCGLOBAL char *mccidirs [MCCIDIRMAX];   
#define         MCCNOSEARCH   1   /* dont search for file */
#define         MCCHOMESEARCH 2   /* search in home dir first, ie #include "foo.h" */
#define         MCCSYSSEARCH  3   /* search in system dir, ie #include <foo.h> */       
MCCGLOBAL char  mcchomedir [MCCMAXPATHLEN] MCCINIT({ 0 });
MCCGLOBAL char  mccsysidir [MCCMAXPATHLEN] MCCINIT( MCCSYSIDIR );      
MCCGLOBAL long  mcctotfiles MCCINIT(0);
MCCGLOBAL long  mcctotlines MCCINIT(0);
MCCGLOBAL long  mcctotbytes MCCINIT(0);
/**** constants ******************/
MCCGLOBAL ptr mccabortfmt MCCINIT("FATAL ERROR: %s (reported on line %d of module %s)\n") ;               
MCCGLOBAL ptr mccerrfmt MCCINIT("ERROR: %s %s %s\n") ;               
MCCGLOBAL ptr mccwarnfmt  MCCINIT("WARNING: %s %s %s\n") ; 
#if DEBUG
   MCCGLOBAL ptr mcctrcfmt MCCINIT("MCCTRACE: %s, line %d, file %s\n");
   MCCGLOBAL ptr mcctrcdfmt MCCINIT("MCCTRACE: %s %d, line %d, file %s\n");
   MCCGLOBAL ptr mcctrcxfmt MCCINIT("MCCTRACE: %s %08x, line %d, file %s\n");
   MCCGLOBAL ptr mcctrcsfmt MCCINIT("MCCTRACE: %s %s, line %d, file %s\n");
#endif
#      define MCCENOMEM   0
#      define MCCWBADOPTION 1
#      define MCCEOERF     2
#      define MCCECERF     3
#      define MCCSUMMARY   4
#      define MCCVERSION   5                     
#      define MCCINITERRS  6  
#      define MCCENOFILES  7
#      define MCCBADFTYPE  8               
#      define MCCEOSRCF    9
#      define MCCEOLSTF    10
#      define MCCECLSTF    11  
#      define MCCBADSUBOPT 12 
#      define MCCBADPPKEY  13                                
#      define MCCNOINCF    14
#      define MCCOINCF     15                    
#      define MCCINCFSYNTAX 16
#      define MCCEOPPF     17
#      define MCCECPPF     18       
#      define MCCBADPPFA   19                                      
#      define MCCPPARGNO   20
#      define MCCPPIFUFLOW 21
#      define MCCPPIFOFLOW 22        
#      define MCCPPARGEOF  23
#      define MCCMANYIDIRS 24  
#      define MCCPPREDEFN  25  
#      define MCCPPNOUNDEF 26
#      define MCCPPNOREDEF 27 
#      define MCCEOFNOENDIF 28
#      define MCCSRCLOOP   29
#      define MCCSRCREPEAT 30
#      define MCCFILELIST  31      
#      define MCCNESTCMT   32
#      define MCCNOIFEXP   33
#      define MCCERRANY  34
#      define MCCSYNTAX    35
#      define MCCNOIMP     36
#      define MCCOPTIGNORED 37
#      define MCCEOFCOMMENT 38
#      define MCCNULLPPEXP  39
#      define MCCIREDPPEXP  40
#      define MCCNULLSUBEXP 41
#      define MCCBADOPERTYPE 42
#      define MCCEILLTOK    43
#      define MCCSTRLITOVFL 44
#      define MCCSTRLITNL  45
#      define MCCPPIFWRFILE 46
#      define MCCPPMISSARGS 47

MCCGLOBAL ptr mccerrmsg[] 
#               ifdef MCCDATA
             = {                                                   
 /*  0   */        "Cannot allocate enough memory",
 /*  1   */        "WARNING: Unrecognized command line option '%s'\n",
 /*  2   */        "Cannot open error file",
 /*  3   */        "Cannot close error file",
 /*  4   */        "%s complete.  %d errors, %d warnings.  Mike Burns' C compiler version %s\n",
 /*  5   */        MCCVERSIONSTRING,
 /*  6   */        "Problems initializing compiler",
 /*  7   */        "No files to process",
 /*  8   */        "ERROR: Cannot process file '%s', wrong file type.\n",
 /*  9   */        "ERROR: Cannot open source file '%s'\n",
 /*  10  */        "ERROR: Cannot open list file '%s'\n",
 /*  11  */        "ERROR: Cannot close list file '%s'\n",
 /*  12  */        "WARNING: '%c' is not a recognized sub option for the '%c' option, ignored.\n",
 /*  13  */        "Unrecognized preprocessor directive '%s'\n",
 /*  14  */        "Missing pathname of file to be included.\n",
 /*  15  */        "ERROR: Unable to open include file '%s'\n",
 /*  16  */        "Bad #include pathname syntax\n",
 /*  17  */        "ERROR: Cannot open preprocessor output file '%s'\n",
 /*  18  */        "ERROR: Cannot close preprocessor output file '%s'\n",
 /*  19  */        "Illegal token (%s) in macro formal argument list.\n",
 /*  20  */        "Wrong number of arguments passed to macro '%s'.\n",
 /*  21  */        "#%s without matching #if, #ifdef, or #ifndef.\n",      
 /*  22  */        "Preprocessor conditional (#%s) nested too deeply.\n",
 /*  23  */        "Found end of file while expecting arguments to macro '%s'.\n",
 /*  24  */        "ERROR: Too many -i switches on command line, '-i %s' ignored.\n",
 /*  25  */        "Redefinition of preprocessor macro '%s'.\n", 
 /*  26  */        "Cannot #undef predefined macro '%s'.\n",
 /*  27  */        "Cannot #define predefined macro '%s'.\n",
 /*  28  */        "Found end of file while still expecting an #endif.\n", 
 /*  29  */        "WARNING: Cannot #include files recursively!\n       Pathname='%s' #included in file '%s' on line %d.\n",
 /*  30  */        "NOTE: #include of file '%s' skipped because it has already been #included.\n",  
 /*  31  */        "NOTE: File #%d: %.*s %s      had %d lines.\n",   
 /*  32  */        "WARNING: %d:%d:%d unterminated or nested comment.\n",
 /*  33  */        "Expressions in #if is not yet implemented, Sorry.\n",
 /*  34  */        "%s\n",
 /*  35  */        "Syntax error.\n",
 /*  36  */        "Sorry, this syntax is not yet implemented.\n",
 /*  37  */        "WARNING: the '%c' option is currently ignored.\n",
 /*  38  */        "End of file encountered with unterminated comment.\n",
 /*  39  */        "#if directive requires an expression, and none was found.\n",
 /*  40  */        "Sorry, could not fully reduce preprocessor expression.\n",
 /*  41  */        "Illegal empty sub expression.\n",
 /*  42  */        "Operand(s) of wrong type for operator.\n",
 /*  43  */        "Illegal token.\n",
 /*  44  */        "String literal too long, truncated.\n",
 /*  45  */        "Un-escaped newline in string literal. String truncated.\n",
 /*  46  */        "#endif matches with #if in a different file.\n",
 /*  47  */        "Invocation of macro '%s' missing arguments.\n",
                    NULL
               }
#               endif
            ;             

MCCGLOBAL int mccoptansi MCCINIT(FALSE);
MCCGLOBAL char mccoption[]
#               ifdef MCCDATA                                                        
/*            1         2         3         4         5         6   */   
/*  01234567890123456789012345678901 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~  */
 = "                                           !                         !        .!-  .    . !      . ! ! . !  !  !   !! ! !       "
#               endif
            ;                                      
#define MCCOPTTYPEIGNORE '.'
#define MCCOPTTYPEMULTI '='
#define MCCOPTTYPEPATHNAME '?'
MCCGLOBAL char mccoptiontype[]
#               ifdef MCCDATA                                                        
/*            1         2         3         4         5         6   */   
/*  01234567890123456789012345678901 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~  */
 = "                                                                         =    ..!  =    . ?      . ! ? . ?  :  ? . == ! !       "
#               endif
            ;                                      
MCCGLOBAL char mccoptimoption[]
#               ifdef MCCDATA                                                        
/*            1         2         3         4         5         6   */   
/*  01234567890123456789012345678901 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~  */
 = "                                                                                                                                "
#               endif
            ;                                      
MCCGLOBAL char mcctrcoption[]
#               ifdef MCCDATA                                                        
/*            1         2         3         4         5         6   */   
/*  01234567890123456789012345678901 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~  */
 = "                                                                                                 !!- !!! ! ! !  ! !!!-          "
#               endif
            ;                                      
MCCGLOBAL char mccparseoption[]
#               ifdef MCCDATA                                                        
/*            1         2         3         4         5         6   */   
/*  01234567890123456789012345678901 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~  */
 = "                                    !                              !                               !          !                 "
#               endif
            ;                                      
MCCGLOBAL char mcclistoption[]
#               ifdef MCCDATA                                                        
/*            1         2         3         4         5         6   */   
/*  01234567890123456789012345678901 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~  */
 = "                                   !                                                                 !          !  ! -  !       "
#               endif
            ;                                      
MCCGLOBAL char mccppoption[]
#               ifdef MCCDATA                                                        
/*            1         2         3         4         5         6   */   
/*  01234567890123456789012345678901 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~  */
 = "                                                 -                                                -!  -     -  ! !              "
#               endif
            ;                                      
#define MCCOPTERRFILE 'e'         
#define MCCOPTLISTING 'l'       
#define MCCOPTTRACE   't'      
#define MCCOPTXREF    'x'      
#define MCCOPTOPTIM   'O'   
#define MCCOPTOUTPUT   'o'   
#define MCCOPTPARSE   's'      
#define MCCOPTPPOUTPUT 'i'
#define MCCOPTPPONLY  'P'      
#define MCCOPTPPSTDOUT 'E'      
#define MCCOPTIDIR    'I'                        
#define MCCOPTCPLUSPLUS "-c++"
#define MCCOPTCPLPL   '+'
#define MCCOPTZSYSIDIR 'Z'   
#define MCCOPTSYSIDIR "-SYSI"   
#define MCCOPTANSI "-ansi"      
#define MCCOPTDEF 'D'
#define MCCOPTUNDEF 'U'
#define MCCOPTDEBUG 'g'
#define MCCOPTNOLD 'c'
#define MCCOPTVERBOSE 'v'
MCCGLOBAL char *mccusage[] 
#ifdef MCCDATA
= {          
          "",
          "Mike Burns' C compiler",
          MCCVERSIONLONG,
          "",
          "USAGE: ",
          "",
          "mcc <file.c> ...",
          "    -ansi         : flag ansi violations",
          "    -e            : write errors to *.err",
          "    -e=<file>     : write errors to <file>",
          "    -Z <dir>      : #include <> files from <dir>/usr/include",
          "    -SYSI <dir>   : #include <> files from <dir>",
          "    -I <dir>      : #include \"\" files from <dir>",
          "    -i<pathname>  : write pp output to pathname not *.i",
          "    -I1           : #include files only once",
          "    -l<usxp#e>    : list options: ",
          "                       u=list user .h source",
          "                       s=list system .h source",
          "                       x=xref",
          "                       e=list conditionally excluded source ",
          "                       #=match #if-#endif's ",
          "                       p=show .i output in listing",
          "    -O<>          : optimize options: ",
          "    -o <pathname> : write object or executable output to <pathname> ",
          "    -P            : preprocess only ",
          "    -P<qlc1bf>    : preprocess options: ",
          "                    q=squash blanks out of .i output",
          "                    l=put #line lines in .i output ",
          "                    o=use old form of #line (# <n> <path>) ",
          "                    c=put comments in .i output",
          "                    1=include only once",
          "                    b=allow blanks between macro name and paren",
          "                    f=flush a bunch of blanks to clear pipe!",
          "    -s<c$nC>       : syntax options: ",
          "                      c=allow nested comments,",
          "                      C=flag /* in comments as warning ,",
          "                      $=allow '$' in names ",
          "                      n=allow un-escaped newlines in stings. ",
          "    -t<rtspe>     : trace options: r=source read  t=tokens,",
          "                         p=pp macros, s=dump global syms,",
          "                        e=expressions, i=include pathnames",
          "                        a=memory allocation/free",
          "    -x            : produce cross reference listing <not yet>",  
          "    -c++          : allow c++ syntax (or use the command c++)",
          "    -+            : allow c++ syntax (or use the command c++)",
          "    -D<name>=<value> : predefine a preprocessor symbol",
          "    -U<name>      : undefine a predefined preprocessor symbol",
          "    -g            : compile code for debug and build debug symbol table",
          "    -c            : compile only, do not proceed to link phase ",
          "    -v            : display a summary of effective options settings ",
          " turn options on with '-' and off with '~' or '!' ",
          NULL }
#endif          
    ;

#define MCCTOKID     'i'
#define MCCTOKSTRLIT 's'
#define MCCTOKCHRLIT 'c'      
#define MCCTOKNUMLIT 'n'
#define MCCTOKOP     'o'               
#define MCCTOKPUNCT  'p'
#define MCCTOKWHITE  'w'
MCCGLOBAL char mcctokfirst[]
#               ifdef MCCDATA                                                        
/*            1         2         3         4         5         6   */   
/*  01234567890123456789012345678901 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~ */
 = "         ww                     wosoioocppoopooonnnnnnnnnnppoooo iiiiiiiiiiiiiiiiiiiiiiiiiip poi iiiiiiiiiiiiiiiiiiiiiiiiiipopo "
#               endif
            ;                                      

#if DEBUG
MCCGLOBAL char *mccttypenames[] 
#               ifdef MCCDATA
           = {
                   "EOFILE ",   /* end of file */
                   "EOLN   ",   /* end of line */
                   "ID     ",   /* identifier */
                   "KEY    ",   /* language keyword */
                   "PPFLAG ",   /* preprocessor sentinal # */
                   "PPID   ",   /* preprocessor identifier */
                   "PPOP   ",   /* preprocessor operator */
                   "EOP    ",   /* expression operator */
                   "PUNCT  ",   /* punctuation */
                   "ILIT   ",   /* integer literal */
                   "SLIT   ",   /* string literal */
                   "FLIT   ",   /* float literal */
                   "DLIT   ",   /* double literal */
                   "PPARG  ",   /* preprocessor macro argument */
                   "COMMENT",   /* comment */
                   "MARKER" ,   /* end of list marker */
                   "ILLTOK" ,   /* illegal token */
                   "NULLTOK"    /* non-existent token */
             }
#               endif
            ;  
#endif

/*-------------------------------------------------------------------*/
/* internal function prototypes */
/*-------------------------------------------------------------------*/
                               
extern sourcep mccopensrc __ARGS((char *path; int bufsize));


/*-------------------------------------------------------------------*/
/* other required includes */
/*-------------------------------------------------------------------*/

#include "ustr.h"        /* unique string handler */

/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/* HISTORY:                                                          */
