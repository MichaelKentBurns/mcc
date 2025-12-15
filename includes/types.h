#ifndef NULL
#define NULL 0 
#endif      

typedef char *ptr;
typedef char boolean;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#include <ctype.h>
#ifdef apollo
#undef tolower
#endif
#endif 

#ifndef int32
                    
#if ANSI
#  define SIZEOFSHORT sizeof(short)
#  define SIZEOFINT sizeof(int)
#  define SIZEOFLONG sizeof(long)
#else
#  if  m68000
#  define SIZEOFSHORT 2
#  define SIZEOFINT   4
#  define SIZEOFLONG  4
#  else 
#  define SIZEOFSHORT 2
#  define SIZEOFINT   2
#  define SIZEOFLONG  4
#  endif
#endif

#define int8 char

#  if SIZEOFSHORT >= 2
#     define int16  short int
#  else
#    if SIZEOFINT >= 2
#       define int16  int
#    else
#      if SIZEOFLONG >= 2
#        define int16  long int
#      else
         error: cannot get a 16 bit integer
#      endif
#    endif
#  endif

#  if SIZEOFSHORT >= 4
#     define int32  short int
#  else
#    if SIZEOFINT >= 4
#       define int32  int
#    else
#      if SIZEOFLONG >= 4
#        define int32  long int
#      else
         error: cannot get a 32 bit integer
#      endif
#    endif      
#  endif

#endif

