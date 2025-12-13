#ifndef tARRAY

#ifndef int32
#  include "types.h"
#endif

/* derivation modifiers */
#define tARRAY    (1<<0)
#define tFUNCTION (1<<1)
#define tPOINTER  (1<<2) 
                          
/* sign and length modifiers */
#define tUNSIGNED (1<<3)
#define tSHORT    (1<<4)
#define tLONG     (1<<5)
#define tDOUBLE   (1<<6)
                               
/* basic types */
#define tVOID     (1<<7)
#define tCHAR     (1<<8)
#define tINT      (1<<9)
#define tFLOAT    (1<<10)
#define tSTRUCT   (1<<11)
#define tUNION    (1<<12)
#define tENUM     (1<<13)

/* type modifiers */                       
#define tCONST    (1<<14)
#define tNOALIAS  (1<<15)
#define tVOLATILE (1<<16)
                        
/* real or abstract */
#define tTYPEDEF  (1<<17)
#define tVARDEF   (1<<18)
                        
/* storage class specifiers */
#define tGLOBAL   (1<<19)
#define tEXTERN   (1<<20)
#define tSTATIC   (1<<21)
#define tAUTO     (1<<22)
#define tREGISTER (1<<23)

/* scope */
#define tBLOCK    (1<<24)
#define tROUTINE  (1<<25)
#define tUNIT     (1<<26)
#define tPROGRAM  (1<<27)                               

/* persistence  */
#define tTRANSIENT (1<<28)
#define tSTEP      (1<<29)
#define tSESSION   (1<<30)
#define tPERMANENT (1<<31)
                                    
#define tDERIVATION  (tARRAY|tFUNCTION|tPOINTER)

#define tLENGTH      (tSHORT|tLONG|tDOUBLE)

#define tBASETYPE    (tCHAR|tINT|tFLOAT|tSTRUCT|tUNION|tENUM|tVOID)

#define tAGGREGATE   (tARRAY|tSTRUCT|tUNION)
                        
#define tSCSPEC      (tTYPEDEF|tEXTERN|tSTATIC|tAUTO|tREGISTER)

#define tSCOPE       (tBLOCK|tROUTINE|tUNIT|tPROGRAM) 

#define tPERSISTENCE (tTRANSIENT|tSTEP|tSESSION|tPERMANENT)
            
/* node in a type network, describes one facet of a type */
typedef struct TNODE *tNodeP;
typedef struct TNODE 
 {                                                     
  tNodeP          basetype;      /* type this is derived from         */
  tNodeP          siblings;      /* other types derived from basetype */
  tNodeP          derivedtypes;  /* list of types derived from this   */
  unsigned int32  flags;         /* bit flags defined above           */
  char           *name;          /* name associated with this node    */
  char           *tag;           /* tag name for struct,union,enum    */
  unsigned long   dimension;     /* dimension if flags & tARRAY       */
  unsigned int    size;          /* size in bytes                     */
  ptr /* ? */     details;       /* ptr to detailed struct for aggr   */
 } tNode;



#endif 
