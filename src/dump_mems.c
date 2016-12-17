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

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "dump.h"

int dump_mems(
        char       *dest,
        size_t      dsize,
        const void *data,
        size_t      size,
        size_t      amount,
        size_t      offset
)
{
  int bytes;
  int rc;
  
  assert(amount > 0);
  assert(size   > 0);
  assert(data   != NULL);
  assert(dsize  >= 10 + (amount * 4) + 2);
  assert(dest   != NULL);
  
  bytes = snprintf(dest,dsize,"%08zX:",offset);
  if (bytes < 0) return bytes;
  if ((unsigned)bytes > dsize) return ENOMEM;
  
  dest  += bytes;
  dsize -= bytes;
  
  rc = hexdump_mems(dest,dsize,data,size,amount);
  if (rc < 0) return rc;
  
  dest  += amount * 3;
  dsize -= amount * 3;
  
  if (dsize == 0)  return ENOMEM;
  
  *dest++ = ' ';
  dsize --;
  
  /*--------------------------------------------------------------------
  ; pad out the hexdump area with spaces if we haven't filled the entire
  ; space with hex data.
  ;---------------------------------------------------------------------*/
  
  if (size < amount)
  {
    size_t d = (amount - size) * 3;
    if (d > dsize) return ENOMEM;
    memset(dest,' ',d);
    dest  += d;
    dsize -= d;
  }
  
  return chardump_mems(dest,dsize,data,size,amount);
}
