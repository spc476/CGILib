
#include <string.h>

#include "../types.h"
#include "../memory.h"
#include "../ddt.h"
#include "../sio.h"

/***********************************************************************/

size_t SIOCopy(SOutput dest,SInput src)
{
  size_t amount = 0;
  
  ddt(dest != NULL);
  ddt(src  != NULL);
  
  while(!SIEof(src))
  {
    struct blockdata data;
    
    data    = SIBlock(src);
    amount += SOBlock(dest,data);
    SIUpdate(src,data);
  }
  return amount;
}

/****************************************************************/

size_t SIOCopyN(SOutput dest,SInput src,size_t size)
{
  size_t amount = 0;
  
  ddt(dest != NULL);
  ddt(src  != NULL);
  
  while(!SIEof(src) && (size > 0))
  {
    struct blockdata data;
    
    data       = SIBlock(src);
    data.size  = min(data.size,size);
    amount    += SOBlock(dest,data);
    SIUpdate(src,data);
    size -= data.size;
  }
  return amount;
}

/****************************************************************/
