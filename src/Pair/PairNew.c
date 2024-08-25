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

/*************************************************************************/

static char const *todelim(char const *str,size_t *plen,char delim,char eos)
{
  char const *p = str;
  
  assert(str  != NULL);
  assert(plen != NULL);
  
  for ( ; (*p != delim) && (*p != eos) && (*p != '\0') ; p++)
    ;
  *plen = (size_t)(p - str);
  return p;
}

/*************************************************************************/

struct pair *PairNew(char const **psrc,char delim,char eos)
{
  struct pair *psp;
  char const  *src;
  char const  *p;
  char        *name;
  char        *value;
  size_t       sname;
  size_t       svalue;
  
  assert(psrc  != NULL);
  assert(*psrc != NULL);
  
  src  = *psrc;
  p    = todelim(src,&sname,delim,eos);
  name = malloc(sname + 1);
  
  if (name == NULL)
    return NULL;
  
  memcpy(name,src,sname);
  name[sname] = '\0';
  
  if (*p == delim)
  {
    src = p + 1;
    p   = todelim(src,&svalue,delim,eos);
  }
  else
    svalue = 0;
  
  value = malloc(svalue + 1);
  
  if (value == NULL)
  {
    free(name);
    return NULL;
  }
  
  memcpy(value,src,svalue);
  value[svalue] = '\0';
  
  /*---------------------------------------------------------------------
  ; Don't call PairCreate() as that will just copy the copy we just made.
  ; Redundant madness there.
  ;----------------------------------------------------------------------*/
  
  psp = malloc(sizeof(struct pair));
  if (psp != NULL)
  {
    psp->node.ln_Succ = NULL;
    psp->node.ln_Pred = NULL;
    psp->name         = name;
    psp->value        = value;
    *psrc             = p;
  }
  else
  {
    free(value);
    free(name);
  }
  
  return psp;
}
