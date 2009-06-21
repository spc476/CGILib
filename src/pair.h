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

struct pair	*(PairNew)		(char **,char,char);
struct pair	*(PairCreate)		(const char *,const char *);
void		 (PairFree)		(struct pair *);

struct pair	*(PairListGetPair)	(List *,const char *);
char		*(PairListGetValue)	(List *,const char *);
void		 (PairListFree)		(List *);

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

static inline struct pair *(PairListFirst)(List *plist)
{
  assert(plist != NULL);
  return (struct pair *)ListGetHead(plist);
}

/*----------------------------------------------------------------------*/

#endif

