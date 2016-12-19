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

#include <stddef.h>
#include <assert.h>

#include "nodelist.h"

/*********************************************************************/

void ListInit(List *const pl)
{
  assert(pl != NULL);
  
  pl->lh_Head     = (Node *)&pl->lh_Tail;
  pl->lh_Tail     = NULL;
  pl->lh_TailPred = (Node *)&pl->lh_Head;
}

/*********************************************************************/

Node *ListRemHead(List *const pl)
{
  Node *pn;
  
  assert(pl != NULL);
  
  pn = ListGetHead(pl);
  assert(pn != NULL);
  if (NodeValid(pn))
    NodeRemove(pn);
  return(pn);
}

/*********************************************************************/

Node *ListRemTail(List *const pl)
{
  Node *pn;
  
  assert(pl != NULL);
  
  pn = ListGetTail(pl);
  assert(pn != NULL);
  if (NodeValid(pn))
    NodeRemove(pn);
  return(pn);
}

/**********************************************************************/

void NodeInsert(Node *const restrict pn,Node *const restrict pntoa)
{
  Node *pnn;
  
  assert(pn    != NULL);
  assert(pntoa != NULL);
  
  pnn            = pn->ln_Succ;
  pntoa->ln_Succ = pnn;
  pntoa->ln_Pred = pn;
  pn->ln_Succ    = pntoa;
  pnn->ln_Pred   = pntoa;
}

/***********************************************************************/

void NodeRemove(Node *const pn)
{
  Node *pns;
  Node *pnp;
  
  assert(pn          != NULL);
  assert(pn->ln_Succ != NULL);
  assert(pn->ln_Pred != NULL);
  
  pns = pn->ln_Succ;
  pnp = pn->ln_Pred;
  
  pns->ln_Pred = pnp;
  pnp->ln_Succ = pns;
}

/*************************************************************************/

Node *NodeNext(Node *pn)
{
  assert(pn != NULL);
  
  if (NodeValid(pn))
    pn = pn->ln_Succ;
  return(pn);
}

/*********************************************************************/

Node *NodePrev(Node *pn)
{
  assert(pn != NULL);
  
  if (NodeValid(pn))
    pn = pn->ln_Pred;
  return(pn);
}

/********************************************************************/

