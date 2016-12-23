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

#define _GNU_SOURCE 1

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "pair.h"

#define SIZET_MAX       ((size_t)-1)

/**********************************************************************/

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
  peos   = memchr(src,eos,SIZET_MAX);   /* doesn't work on DEC Alpha */
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

/***********************************************************************/

struct pair *PairCreate(const char *restrict name,const char *restrict value)
{
  struct pair *psp;
  
  assert(name  != NULL);
  assert(value != NULL);
  
  psp         = malloc(sizeof(struct pair));
  psp->name   = strdup(name);
  psp->value  = strdup(value);
  return(psp);
}

/***********************************************************************/

void PairFree(struct pair *psp)
{
  assert(psp != NULL);
  assert(NodeValid(&psp->node));
  
  free(psp->name);
  free(psp->value);
  free(psp);
}

/*************************************************************************/

void PairListCreate(
        List       *plist,
        const char *restrict name,
        const char *restrict value
)
{
  struct pair *pair;
  
  assert(plist != NULL);
  assert(name  != NULL);
  assert(value != NULL);
  
  pair = PairCreate(name,value);
  ListAddTail(plist,&pair->node);
}

/*************************************************************************/

struct pair *PairListGetPair(const List *plist,const char *name)
{
  struct pair *psp;
  
  assert(plist != NULL);
  assert(name  != NULL);
  
  psp = (struct pair *)ListGetHead(plist);
  while(NodeValid(&psp->node))
  {
    if (strcmp(psp->name,name) == 0) return(psp);
    psp = (struct pair *)NodeNext(&psp->node);
  }
  return(NULL);
}

/***********************************************************************/

char *PairListGetValue(const List *plist,const char *name)
{
  struct pair *psp;
  
  assert(plist != NULL);
  assert(name  != NULL);
  
  psp = PairListGetPair(plist,name);
  if (psp != NULL)
    return(psp->value);
  else
    return(NULL);
}

/************************************************************************/

void PairListFree(List *plist)
{
  struct pair *psp;
  
  assert(plist != NULL);
  
  while(1)
  {
    psp = (struct pair *)ListRemHead(plist);
    if (!NodeValid(&psp->node))
    {
      return;
    }
    PairFree(psp);
  }
}

/***********************************************************************/

