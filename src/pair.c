
/************************************************************************
*
* Copyright 2001 by Sean Conner.  All Rights Reserved.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
* Comments, questions and criticisms can be sent to: sean@conman.org
*
*************************************************************************/

#include <stddef.h>
#include <string.h>
#include "types.h"
#include "nodelist.h"
#include "memory.h"
#include "pair.h"
#include "stream.h"
#include "util.h"
#include "ddt.h"

/**********************************************************************/

struct pair *(PairNew)(char **psrc,char delim,char eos)
{
  struct pair *psp;
  char        *src;
  char        *peos;
  char        *pdelim;
  size_t       sname;
  size_t       svalue;
  
  ddt(psrc  != NULL);
  ddt(*psrc != NULL);
  ddt(delim != eos);
  
  src    = *psrc;
  peos   = memchr(src,eos,SIZET_MAX);	/* doesn't work on DEC Alpha */
  ddt(peos   != NULL);
  pdelim = memchr(src,delim,peos-src);
  ddt(pdelim != NULL);
  
  sname      = pdelim - src;
  svalue     = peos   - pdelim - 1;
  psp        = MemAlloc(sizeof(struct pair));
  psp->name  = MemAlloc(sname + 1);
  psp->value = MemAlloc(svalue + 1);
  memcpy(psp->name,src,sname);
  memcpy(psp->value,pdelim+1,svalue);
  psp->name[sname]   = '\0';
  psp->value[svalue] = '\0';
  psp->sname         = sname  + 1;
  psp->svalue        = svalue + 1;
  psp->oname         = psp->name;
  psp->ovalue        = psp->value;
  *psrc              = peos   + 1;
  return(psp);
}

/***********************************************************************/

struct pair *(PairCreate)(const char *name,const char *value)
{
  struct pair *psp;
  
  ddt(name  != NULL);
  ddt(value != NULL);
  
  psp         = MemAlloc(sizeof(struct pair));
  psp->name   = psp->oname  = dup_string(name);
  psp->value  = psp->ovalue = dup_string(value);
  psp->sname  = strlen(name)  + 1;
  psp->svalue = strlen(value) + 1;
  return(psp);
}

/***********************************************************************/

struct pair *(PairClone)(struct pair *pair)
{
  ddt(pair != NULL);
  
  return(PairCreate(pair->name,pair->value));
}

/************************************************************************/

void (PairFree)(struct pair *psp)
{
  ddt(psp != NULL);
  ddt(NodeValid(&psp->node));
  
  MemFree(psp->oname,psp->sname);
  MemFree(psp->ovalue,psp->svalue);
  MemFree(psp,sizeof(struct pair));
}

/*************************************************************************/

void (PairListAdd)(List *plist,char **psrc,char delim,char eos)
{
  struct pair *psp;
  
  ddt(plist != NULL);
  ddt(psrc  != NULL);
  ddt(*psrc != NULL);
  ddt(delim != eos);
  
  psp = PairNew(psrc,delim,eos);
  ListAddTail(plist,&psp->node);
}

/**********************************************************************/

struct pair *(PairListFirst)(List *plist)
{
  ddt(plist != NULL);
  return((struct pair *)ListGetHead(plist));
}

/**********************************************************************/

struct pair *(PairListGetPair)(List *plist,const char *name)
{
  struct pair *psp;
  
  ddt(plist != NULL);  
  ddt(name  != NULL);
  
  psp = (struct pair *)ListGetHead(plist);
  while(NodeValid(&psp->node))
  {
    if (strcmp(psp->name,name) == 0) return(psp);
    psp = (struct pair *)NodeNext(&psp->node);
  }
  return(NULL);
}

/***********************************************************************/

char *(PairListGetValue)(List *plist,const char *name)
{
  struct pair *psp;

  ddt(plist != NULL);  
  ddt(name  != NULL);
  
  psp = PairListGetPair(plist,name);
  if (psp != NULL)
    return(psp->value);
  else
    return(NULL);
}

/************************************************************************/

void (PairListFree)(List *plist)
{
  struct pair *psp;
  
  ddt(plist != NULL);
  
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

