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
#include <assert.h>

#include "../dump.h"

int hexdump_mems(
        char       *dest,
        size_t      dsize,
        void const *data,
        size_t      size,
        size_t      amount
)
{
  unsigned char const *block = data;
  
  assert(amount > 0);
  assert(size   > 0);
  assert(data   != NULL);
  assert(dsize  >= amount * 3);
  assert(dest   != NULL);
  
  for (
        ;
        (dsize > 0) && (amount > 0) && (size > 0) ;
        amount--,block++,size--
      )
  {
    if (dsize < 4) return ENOMEM;
    dest[0] = ' ';
    hex(&dest[1],dsize,*block,2);
    dest  += 3;
    dsize -= 3;
  }
  
  *dest = '\0';
  return 0;
}
