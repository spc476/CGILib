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

#include <stdbool.h>
#include <errno.h>
#include <assert.h>

#include <syslog.h>

#include "dump.h"

int dump_memoryl(
        int         level,
        const void *data,
        size_t      size,
        size_t      amount,
        size_t      offset
)
{
  const char *mem = data;
  char        buffer[BUFSIZ];

  assert(level  >= 0);
  assert(level  <= LOG_DEBUG);  
  assert(data   != NULL);
  assert(size   >  0);
  assert(amount >  0);
  
  /*---------------------------------------------------------------------
  ; Each line contains:
  ;
  ;	sizeof(size_t)*2 characters	pointer
  ;	': '				separator
  ;	amount * 3			space + hex byte value
  ;	' '				separator
  ;	amount				ASCII dump
  ;	'\0'				NUL byte
  ;
  ; I want to ensure a large enough buffer, and this assert will catch a
  ; too-small buffer during development.
  ;---------------------------------------------------------------------*/
  
  assert(sizeof(buffer) >= sizeof(size_t)*2 + 2 + (amount * 3) + 1 + amount + 1);
  
  while(true)
  {
    int rc = dump_mems(buffer,sizeof(buffer),mem,size,amount,offset);
    if (rc < 0) return rc;
    syslog(level,"%s",buffer);
    if (amount >= size) break;
    
    mem    += amount;
    size   -= amount;
    offset += amount;
  }
  
  return 0;
}
