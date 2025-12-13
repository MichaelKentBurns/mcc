/* host dependant information */

#if !defined(DOS) && !defined(unix)
#  define unix 1
#endif

#if defined(LATTICE)
#define ANSI 1
#endif

#if defined(unix) || defined(stdc) || defined(DOS)

#     define MCCMAXPATHLEN 255 
#     define MCCEXTSEP  '.'   /* seperator between filename and extension */ 
#     define MCCFILSEP  '/'   /* seperator between directory and filename */ 
#     define MCCEXTSRC  ".c"
#     define MCCEXTLST  ".lst"
#     define MCCEXTERR  ".err"
#     define MCCEXTQUAD ".q"
#     define MCCEXTOBJ  ".o"
#     define MCCEXTPP   ".i"
#     define MCCSYSIDIR "/usr/include"

#else
      
     error NO APPROPRIATE HOST INFORMATION FOUND

#endif

#if defined(ANSI) 

#    define MCCDELETEFILE(path)  remove(path)

#else
#  if defined(unix)

#    define MCCDELETEFILE(path)  unlink(path) 
          
#  else

      error CANNOT DEFINE MCCDELETE no suitable host info

#  endif

#endif
