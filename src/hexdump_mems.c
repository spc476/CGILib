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
#include <assert.h>

int hexdump_mems(
        char       *dest,
        size_t      dsize,
        const void *data,
        size_t      size,
        size_t      amount
)
{
  const unsigned char *block = data;
  int                  bytes;
  
  assert(amount > 0);
  assert(size   > 0);
  assert(data   != NULL);
  assert(dsize  >= amount * 3);
  assert(dest   != NULL);
  
  for ( ; (dsize > 0) && (amount > 0) ; amount--,block++)
  {
    bytes = snprintf(dest,dsize," %02X",*block);
    if (bytes < 0) return bytes;
    if ((unsigned)bytes > dsize) return ENOMEM;
    dest  += bytes;
    dsize -= bytes;
  }
  
  return 0;
}
