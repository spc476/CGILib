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

#ifndef __GNUC__
#  define __attribute__(x)
#endif

#include <assert.h>
#include "../dump.h"
#include "../util.h"

void hex(char *dest,size_t dsize __attribute__((unused)),uintptr_t val,size_t digits)
{
  assert(dest   != NULL);
  assert(digits <= sizeof(uintptr_t) * 2);
  assert(dsize  >  digits);
  
  dest[digits] = '\0';
  while(digits--)
  {
    dest[digits] = hextoc((int)(val & (uintptr_t)15));
    val >>= 4;
  }
}
