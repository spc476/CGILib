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

#ifndef NODELIST_H
#define NODELIST_H

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

void		(ListInit)		(List *const);
void		(ListAddHead)		(List *const,Node *const);
void		(ListAddTail)		(List *const,Node *const);
Node           *(ListGetHead)		(List *const);
Node           *(ListGetTail)		(List *const);
Node           *(ListRemHead)		(List *const);
Node           *(ListRemTail)		(List *const);
int		(ListEmpty)		(List *const);

void		(NodeInsert)		(Node *const,Node *const);
void		(NodeRemove)		(Node *const);
Node           *(NodeNext)		(Node *);
Node           *(NodePrev)		(Node *);
Node           *(NodeNextW)		(Node *);
Node           *(NodePrevW)		(Node *);
int		(NodeValid)		(Node *const);

/************************************************************************/

#ifdef SCREAM

#  define NodeValid(n)	(n)->ln_Succ ? (n)->ln_Pred ? 1 : 0 : 0 
#  define NodeNext(n)	((n)->ln_Succ ? (n)->ln_Pred ? (n)->ln_Succ : (n):(n))
#  define NodePrev(n)	((n)->ln_Succ ? (n)->ln_Pred ? (n)->ln_Pred : (n):(n))

#  define ListAddHead(l,n)	NodeInsert((Node *)&(l)->lh_Head,(n))
#  define ListAddTail(l,n)	NodeInsert((l)->lh_TailPred,(n))
#  define ListGetHead(l)	(l)->lh_Head
#  define ListGetTail(l)	(l)->lh_TailPred
#  define ListEmpty(l)		((l)->lh_Head == (Node *)&(l)->lh_Tail)

#endif
	
#endif

