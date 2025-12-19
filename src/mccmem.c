#include <stdio.h>
#include "mcc.h"
#undef malloc
#undef free
#include <memory.h>
#include <stdlib.h>

#undef free
#define BLOCKSIG 'BLCK'
extern int mccmemtot;
extern int mccmempeak;
extern int mccnfrees;
extern int mccnmallocs;
extern char mcctrcoption[];

#define BIGCHUNK 40960
static char *bigblock = NULL;
static int bigblockleft = 0;


void* mccmalloc(int size, char *file, int line )
{
 int *iaddr;
 int slotno;
 int checkit = TRUE; /* (mcctrcoption['a']); */

 if (mcctrcoption['c'])
  {
    /* round up to 8 byte boundary */
    size = (size + 7) & 0xfffffff8;
    if (checkit)
       /* if accounting for memory, add two bytes for signature and size */
       size = size + (2*sizeof(int));
     else
       size = size + (1*sizeof(int));
    slotno = size >> 3;
    if (slotno >= NMEMSIZES)
       {
        iaddr = (int *) malloc(size);
        mccnrealmallocs++;
       }
    else if (mccfbs[slotno] == NULL )
       {
           if (bigblockleft < size && size < BIGCHUNK)
             {
              bigblock = (char *) malloc(BIGCHUNK);
              mccnrealmallocs++;
              bigblockleft = BIGCHUNK - 80;
              mccbigchunks++;
             }
           if (bigblockleft >= size)
             {
              bigblockleft -= size;
              iaddr = (int *) (bigblock + bigblockleft);
             }
            else
             {
              iaddr = (int *) malloc(size);
              mccnrealmallocs++;
             }
       }
      else
       {
        iaddr = (int *) mccfbs[slotno];
        mccfbs[slotno] = *((int ***) iaddr);
        mccfbusage[slotno]++;
        mccfbcur[slotno]--;
       }
   if (iaddr)
     {
       iaddr[0] = size; 
       iaddr += 1;
       if (checkit)
        {
          iaddr[0] = BLOCKSIG; 
          mccmemtot += size;
          if (mccmemtot > mccmempeak)
            mccmempeak = mccmemtot;
          iaddr += 1;
          mccnmallocs++;
        }
     }
  }
 else
    iaddr = (int *) malloc(size);
 return((char *) iaddr);
}

void mccfree(char *addr, char *file, int line )
{
  int *iaddr;
  int size;
  int slotno;
  int checkit = TRUE; /* (mcctrcoption['a']); */

 if (mcctrcoption['c'])
  {
    iaddr = (int *) addr;
    if (checkit)
       iaddr -= 2;
     else
       iaddr -= 1;
     
    if (!checkit || iaddr[1] == BLOCKSIG)
      {
       size = iaddr[0];
       if (checkit)
         {
          mccnfrees++;
          mccmemtot -= size;
         }
       slotno = size >> 3;
       if (slotno >= NMEMSIZES)
          {
           free((char *) iaddr);
           mccnrealfrees++;
          }
         else
           {
             /* first word of block is used as forward link to next block */
             iaddr[0] = (int) mccfbs[slotno];
             mccfbs[slotno] = (int **) iaddr;
             mccfbcur[slotno]++;
           }
      } 
    else if (mcctrcoption['a'])
      {
      fprintf(stderr,
         "mccfree: attempt to free bad or corrupt block @0x%08lx.\n",
         addr);
      }
   }
  else 
    free(addr);

return;
}
