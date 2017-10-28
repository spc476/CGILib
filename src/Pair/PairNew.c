/***************************************************************************
*
* Copyright 2001,2013 by Sean Conner.
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

#ifdef __GNUC__
#  define _GNU_SOURCE
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../pair.h"

struct pair *PairNew(char **psrc,char delim,char eos)
{
  struct pair *psp;
  char        *src;
  char        *peos;
  char        *pdelim;
  size_t       sname;
  size_t       svalue;
  
  assert(psrc  != NULL);
  assert(*psrc != NULL);
  assert(delim != eos);
  
  src    = *psrc;
  peos   = memchr(src,eos,SIZE_MAX);   /* doesn't work on DEC Alpha */
  assert(peos   != NULL);
  pdelim = memchr(src,delim,(size_t)(peos-src));
  
  /*-------------------------------------------------------------------------
  ; Sigh.  Dealing with garbage input.  We *know* that the end of the string
  ; given will end with a '&' (since we added one, just to make sure).  But
  ; what if we're given:
  ;
  ;     "name&"
  ; or
  ;     "&"
  ;
  ; We need to handle these bizare corner cases.
  ;-------------------------------------------------------------------------*/
  
  if (pdelim == NULL)
  {
    pdelim = peos;
    sname  = (size_t)(pdelim - src);
    svalue = 0;
  }
  else
  {
    sname  = (size_t)(pdelim - src);
    svalue = (size_t)(peos   - pdelim) - 1;
  }
  
  psp        = malloc(sizeof(struct pair));
  psp->name  = malloc(sname + 1);
  psp->value = malloc(svalue + 1);
  memcpy(psp->name,src,sname);
  memcpy(psp->value,pdelim+1,svalue);
  psp->name[sname]   = '\0';
  psp->value[svalue] = '\0';
  *psrc              = peos   + 1;
  return(psp);
}
