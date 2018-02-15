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

#ifndef I_B1911DD8_9CF1_554A_869D_531256015997
#define I_B1911DD8_9CF1_554A_869D_531256015997

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

/*********************************************************************/

typedef struct node
{
  struct node *ln_Succ;
  struct node *ln_Pred;
} Node;

typedef struct list
{
  struct node *lh_Head;
  struct node *lh_Tail;
  struct node *lh_TailPred;
} List;

/***********************************************************************/

extern void  ListInit    (List *const);
extern Node *ListRemHead (List *const);
extern Node *ListRemTail (List *const);

extern void  NodeInsert  (Node *const restrict,Node *const restrict);
extern void  NodeRemove  (Node *const);

/************************************************************************/

static inline bool NodeValid(Node const *const pn)
{
  assert(pn != NULL);
  return ((pn->ln_Succ != NULL) && (pn->ln_Pred != NULL));
}

/*----------------------------------------------------------------------*/

static inline Node *NodeNext(Node const *const n)
{
  assert(n != NULL);
  assert(NodeValid(n));
  return n->ln_Succ;
}

/*-----------------------------------------------------------------------*/

static inline Node *NodePrev(Node const *const n)
{
  assert(n != NULL);
  assert(NodeValid(n));
  return n->ln_Pred;
}

/*-----------------------------------------------------------------------*/

static inline void ListAddHead(List *const pl,Node *const pn)
{
  assert(pl != NULL);
  assert(pn != NULL);
  
  NodeInsert((Node *)&pl->lh_Head,pn);
}

/*-----------------------------------------------------------------------*/

static inline void ListAddTail(List *const pl,Node *const pn)
{
  assert(pl != NULL);
  assert(pn != NULL);
  
  NodeInsert(pl->lh_TailPred,pn);
}

/*-----------------------------------------------------------------------*/

static inline Node *ListGetHead(List const *const pl)
{
  assert(pl          != NULL);
  assert(pl->lh_Head != NULL);
  
  return(pl->lh_Head);
}

/*------------------------------------------------------------------------*/

static inline Node *ListGetTail(List const *const pl)
{
  assert(pl              != NULL);
  assert(pl->lh_TailPred != NULL);
  
  return(pl->lh_TailPred);
}

/*------------------------------------------------------------------------*/

static inline bool ListEmpty(List const *const pl)
{
  assert(pl != NULL);
  return(pl->lh_Head == (Node *)&pl->lh_Tail);
}

/*-----------------------------------------------------------------------*/

#endif
