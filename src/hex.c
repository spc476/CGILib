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

#include <assert.h>
#include "dump.h"

void hex(char *dest,size_t dsize,uintptr_t val,size_t digits)
{
  int c;
  
  assert(dest   != NULL);
  assert(dsize  >= sizeof(uintptr_t) * 2);
  assert(digits <= sizeof(uintptr_t) * 2);
  
  dest[digits] = '\0';
  while(digits--)
  {
    c = (int)(val & (uintptr_t)15);
    assert(c >= 0);
    assert(c <  16);
    c += '0';
    if (c > '9') c += 7;
    dest[digits] = c;
    val >>= 4;
  }
}
