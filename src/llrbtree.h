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

typedef union all
{
  unsigned char      uc;
  signed char        c;
  unsigned short int us;
  short int          s;
  unsigned int       ui;
  int                i;
  unsigned long      ul;
  long               l;
  unsigned long long ull;
  long long          ll;
  void              *p;
} all__t;

typedef struct llrbnode
{
  struct llrbnode *left;
  struct llrbnode *right;
  bool             red;
  all__t           key;
  all__t           value;
} llrbnode__t;

typedef struct llrbtree
{
  struct llrbnode  *left;
  int             (*cmp)(all__t,all__t);
} llrbtree__t;

/**************************************************************************/

void	LLRBTreeInsert	(llrbtree__t *const,all__t,all__t);
bool	LLRBTreeFind	(llrbtree__t *const,all__t,all__t *);
void	LLRBTreeDelete	(llrbtree__t *const,all__t);

/**************************************************************************/

#endif
