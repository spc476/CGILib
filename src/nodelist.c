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
#include "nodelist.h"
#include "ddt.h"

/*********************************************************************/

void (ListInit)(List *const pl)
{
  ddt(pl != NULL);

  pl->lh_Head	  = (Node *)&pl->lh_Tail;
  pl->lh_Tail	  = NULL;
  pl->lh_TailPred = (Node *)&pl->lh_Head;
}

/*********************************************************************/

void (ListAddHead)(List *const pl,Node *const pn)
{
  ddt(pl != NULL);
  ddt(pn != NULL);

  NodeInsert((Node *)&pl->lh_Head,pn);
}

/**********************************************************************/

void (ListAddTail)(List *const pl,Node *const pn)
{
  ddt(pl != NULL);
  ddt(pn != NULL);

  NodeInsert(pl->lh_TailPred,pn);
}

/*********************************************************************/

Node *(ListGetHead)(List *const pl)
{
  ddt(pl          != NULL);
  ddt(pl->lh_Head != NULL);

  return(pl->lh_Head);
}

/**********************************************************************/

Node *(ListGetTail)(List *const pl)
{
  ddt(pl              != NULL);
  ddt(pl->lh_TailPred != NULL);

  return(pl->lh_TailPred);
}

/********************************************************************/

Node *(ListRemHead)(List *const pl)
{
  Node *pn;

  ddt(pl != NULL);

  pn = ListGetHead(pl);
  ddt(pn != NULL);
  if (NodeValid(pn)) 
    NodeRemove(pn);
  return(pn);
}

/*********************************************************************/

Node *(ListRemTail)(List *const pl)
{
  Node *pn;

  ddt(pl != NULL);

  pn = ListGetTail(pl);
  ddt(pn != NULL);
  if (NodeValid(pn))
    NodeRemove(pn);
  return(pn);
}

/**********************************************************************/

int (ListEmpty)(List *const pl)
{
  ddt(pl != NULL);
  return(pl->lh_Head == (Node *)&pl->lh_Tail);
}

/***********************************************************************/

void (NodeInsert)(Node *const pn,Node *const pntoa)
{
  Node *pnn;

  ddt(pn    != NULL);
  ddt(pntoa != NULL);

  pnn            = pn->ln_Succ;
  pntoa->ln_Succ = pnn;
  pntoa->ln_Pred = pn;
  pn->ln_Succ	 = pntoa;
  pnn->ln_Pred   = pntoa;
}

/***********************************************************************/

void (NodeRemove)(Node *const pn)
{
  Node *pns;
  Node *pnp;

  ddt(pn          != NULL);
  ddt(pn->ln_Succ != NULL);
  ddt(pn->ln_Pred != NULL);

  pns = pn->ln_Succ;
  pnp = pn->ln_Pred;

  pns->ln_Pred = pnp;
  pnp->ln_Succ = pns;
}

/*************************************************************************/

Node *(NodeNext)(Node *pn)
{
  ddt(pn != NULL);

  if (NodeValid(pn))
    pn = pn->ln_Succ;
  return(pn);
}

/*********************************************************************/

Node *(NodePrev)(Node *pn)
{
  ddt(pn != NULL);

  if (NodeValid(pn))
    pn = pn->ln_Pred;
  return(pn);
}

/********************************************************************/

Node *(NodeNextW)(Node *pn)
{
  ddt(pn != NULL);

  if (NodeValid(pn))
  {
    pn = pn->ln_Succ;
    if (!NodeValid(pn))
    {
      pn = *((Node **)(pn) - 1);
    }
  }
  return(pn);
}

/********************************************************************/

Node *(NodePrevW)(Node *pn)
{
  ddt(pn != NULL);

  if (NodeValid(pn))
  {
    pn = pn->ln_Pred;
    if (!NodeValid(pn))
    {
      pn = *((Node **)(pn + 1));
    }
  }
  return(pn);
}

/*********************************************************************/

int (NodeValid)(Node *const pn)
{
  ddt(pn != NULL);

  if (pn->ln_Succ == NULL) return(0);
  if (pn->ln_Pred == NULL) return(0);
  return(1);
}

/**********************************************************************/

