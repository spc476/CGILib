/************************************************************************
*
* Copyright 2011 by Sean Conner.  All Rights Reserved.
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

#ifndef LLRB_TREE_H
#define LLRB_TREE_H

typedef struct llrbnode
{
  struct llrbnode *left;
  struct llrbnode *right;
  bool             red;
} llrbnode__t;

typedef struct llrbtree
{
  llrbnode__t  *left;
  int         (*cmp)(void *,llrbnode__t *);
  void        (*upd)(llrbnode__t *,llrbnode__t *);
  void        (*del)(void *);
} llrbtree__t;

/**************************************************************************/

void	 LLRBTreeInsert	(llrbtree__t *,void *,llrbnode__t *);
void	*LLRBTreeFind	(llrbtree__t *,void *);
void	 LLRBTreeDelete	(llrbtree__t *,void *);

/**************************************************************************/

#endif
