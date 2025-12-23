#include <ctype.h>
#include <stdio.h>         
#include <memory.h> 

#define GLOBAL    
#define AUTO    
typedef void (*vfp)();
       
typedef struct { char c2[2];  } c2;
typedef struct { char c3[3];  } c3;
typedef struct { char c4[4];  } c4;
typedef struct { char c8[8];  } c8;
typedef struct { char c16[16];} c16;
typedef struct { char c80[80];} c80;
#define M2(f,t)  ( ( *((c2 *)  (t))) = ( *((c2 *) (f)))  )
#define M3(f,t)  ( ( *((c3 *)  (t))) = ( *((c3 *) (f)))  )
#define M4(f,t)  ( ( *((c4 *)  (t))) = ( *((c4 *) (f)))  )
#define M8(f,t)  ( ( *((c8 *)  (t))) = ( *((c8 *) (f)))  )
#define M16(f,t) ( ( *((c16 *) (t))) = ( *((c16 *) (f))) )
#define M80(f,t) ( ( *((c80 *) (t))) = ( *((c80 *) (f))) )

GLOBAL char toprintc;
#define TOPRINT(c) ( toprintc = (c) & 0x7f, (isprint(toprintc)) ? toprintc : '_' )
              
GLOBAL char hex[] = "0123456789abcdef";
GLOBAL char blanks[] =  
"                                                                                        \
                                                                                          ";
void memdump(fd,description,name,a,p,l)
 FILE *fd;
 char *description;
 char *name;
 char *a;                             
 long p;
 int  l;
{
  struct { 
           char address[20];
           char colon;
           char blank1;
           char hex[44];
           char blank2;
           char ascii[16];   
           char nl;
           char term;  
           char fill[20];
         } line;
  long cur;
  char *hp;
  char *ap;
  int  skipover;
  int  n;         
  char last16[16];   
  int  nsame;

  if (fd == NULL)
      fd = stdout;
  if (description == NULL)
      description = "";
            
  fprintf(fd,"DUMP of '%s'%s%s (pointer=%016lx length=%d) :\n",description,
              (name != NULL) ? " NAMED ": "", (name != NULL) ? name : "",p,l,1,2);
  cur = (long) a;
  nsame = 0;
  last16[0] = *a + 1;  /* guarantee that last16 does not match beginning text */
  while(l > 0)
    {       
      skipover = cur & 0xf;
      cur -= skipover; 

      M80(blanks,&line);
      line.term = 0; 
      line.nl = '\n';
      sprintf(line.address,"%016lx",cur);
      line.colon = ':';                 
      
      hp = line.hex; 
      ap = line.ascii;
      n = 0; 
     
      cur += skipover;
      hp += (skipover*2) + (skipover / 2) + (skipover / 4) + (skipover / 8);
      ap += skipover;
      n += skipover; 

      if ( l <= 16 || memcmp(a,last16,16) != 0)
         {
           M16(a,last16);
           if (nsame > 0)
              fprintf(fd,"   ... %d lines the same.\n",nsame);
           nsame = 0;
           while (l > 0 && n < 16)
              {             
               *hp++ = hex[(*a >> 4) & 0xf];
               *hp++ = hex[*a & 0xf];
               cur++;
               if ((cur & 0x1) == 0)
                 hp++;          
               if ((cur & 0x3) == 0)
                 hp++;
               if ((cur & 0x7) == 0)
                 hp++;                        
               *ap++ = TOPRINT(*a++);
               n++;
               l--;
              }
           fputs((char *) &line,fd);
         }
        else
         {
           nsame++;
           cur += 16;
           a += 16;
           l -= 16;
         }
         
    }

}                      
