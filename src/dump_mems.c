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
  size_t bump;
  int    rc;
  
  assert(amount > 0);
  assert(size   > 0);
  assert(data   != NULL);
  assert(dest   != NULL);
  
  /*---------------------------------------------------------------------
  ; Each line contains:
  ;
  ;     sizeof(size_t)*2 characters     pointer
  ;     ': '                            separator
  ;     amount * 3                      space + hex byte value
  ;     ' '                             separator
  ;     amount                          ASCII dump
  ;     '\0'                            NUL byte
  ;
  ; I want to ensure a large enough buffer, and this assert will catch a
  ; too-small buffer during development.
  ;---------------------------------------------------------------------*/
  
  assert(dsize >= sizeof(size_t)*2 + 2 + (amount * 3) + 1 + amount + 1);
  
  hex(dest,dsize,offset,sizeof(size_t)*2);
  dest    += sizeof(size_t)*2;
  dsize   -= sizeof(size_t)*2;
  dest[0]  = ':';
  dest[1]  = ' ';
  dest    += 2;
  dsize   -= 2;
  
  rc = hexdump_mems(dest,dsize,data,size,amount);
  if (rc < 0) return rc;
  
  /*------------------------------------------------------------------
  ; Advance buffer pointer past the hex data.  There are two cases to watch
  ; out for---when size < amount of data we want, and size >= amount of data
  ; we want.  For the former, we only want to bump up the buffer pointer as
  ; much as we wrote out.  Yes, we could use strlen() to to get to the end
  ; of the buffer, but this is O(1) instead of O(n).
  ;------------------------------------------------------------------------*/
  
  bump   = size < amount ? size : amount;
  dest  += bump * 3;
  dsize -= bump * 3;
  
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
    
    /*--------------------------------------------------------------------
    ; memset() is not listed in POSIX as being an async-safe function.
    ; Really, POSIX?  Really?  I can't see it being non-async-safe as other
    ; than setting memory to 0, it has no other side effects outside the
    ; memory it's given.  Unless I'm horribly wrong.  I'm not going to write
    ; a loop to set a bunch of characters to ' ' just to be POSIXly anal
    ; here.  I'm going to call memset() and if there is a case where this
    ; truly breaks, then I'll change my tune.  But not until then.
    ;----------------------------------------------------------------------*/
    
    memset(dest,' ',d);
    dest  += d;
    dsize -= d;
  }
  
  return chardump_mems(dest,dsize,data,size,amount);
}
