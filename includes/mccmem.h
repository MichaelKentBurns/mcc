#include <memory.h>
#define malloc(size) mccmalloc(size)  
#define free(addr) mccfree(addr)
