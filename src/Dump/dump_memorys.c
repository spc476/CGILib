/***************************************************************************
*
* Copyright 2016 by Sean Conner.
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, see <http://www.gnu.org/licenses/>.
*
* Comments, questions and criticisms can be sent to: sean@conman.org
*
*************************************************************************/

#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "../dump.h"

int dump_memorys(
        char       *dest,
        size_t      dsize,
        void const *data,
        size_t      size,
        size_t      amount,
        size_t      offset
)
{
  char const *mem = data;
  
  assert(amount >  0);
  assert(size   >  0);
  assert(data   != NULL);
  assert(dsize  >  0);
  assert(dest   != NULL);
  
  while(dsize > 0)
  {
    size_t len;
    int    rc;
    
    rc = dump_mems(dest,dsize,mem,size,amount,offset);
    if (rc < 0) return rc;
    
    len     = strlen(dest);
    dest   += len;
    dsize  -= len;
    
    if (dsize == 0) return ENOMEM;
    *dest++ = '\n';
    dsize--;
    
    if (amount >= size) break;
    
    mem    += amount;
    size   -= amount;
    offset += amount;
  }
  
  if (dsize == 0) return ENOMEM;
  *dest = '\0';
  
  return 0;
}
