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

#ifndef PAIR_H
#define PAIR_H

#include "nodelist.h"

struct pair
{
  Node    node;
  char   *name;
  char   *value;
};

/**********************************************************************/

extern struct pair	*PairNew		(char **,char,char);
extern struct pair	*PairCreate		(const char *,const char *);
extern void		 PairFree		(struct pair *);

extern struct pair	*PairListGetPair	(List *,const char *);
extern char		*PairListGetValue	(List *,const char *);
extern void		 PairListFree		(List *);

/***********************************************************************/

static inline struct pair *PairClone(struct pair *pair)
{
  assert(pair != NULL);
  return PairCreate(pair->name,pair->value);
}

/*-------------------------------------------------------------------*/

static inline void PairListAdd(List *plist,char **psrc,char delim,char eos)
{
  struct pair *psp;
  
  assert(plist != NULL);
  assert(psrc  != NULL);
  assert(*psrc != NULL);
  assert(delim != eos);
  
  psp = PairNew(psrc,delim,eos);
  ListAddTail(plist,&psp->node);
}

/*---------------------------------------------------------------------*/

static inline struct pair *PairListFirst(List *plist)
{
  assert(plist != NULL);
  return (struct pair *)ListGetHead(plist);
}

/*----------------------------------------------------------------------*/

#endif

