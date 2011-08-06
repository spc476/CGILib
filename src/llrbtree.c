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

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#include "llrbtree.h"

/**************************************************************************/

static inline bool		 is_red		(llrbnode__t *);
static inline void 		 color_flip	(llrbnode__t *);
static inline llrbnode__t	*get_min	(llrbnode__t *);

static llrbnode__t	*rotate_left	(llrbnode__t *);
static llrbnode__t	*rotate_right	(llrbnode__t *);
static llrbnode__t	*move_red_left	(llrbnode__t *);
static llrbnode__t	*move_red_right	(llrbnode__t *);
static llrbnode__t	*fix_up		(llrbnode__t *);
static llrbnode__t	*insert		(llrbtree__t *,llrbnode__t *,void *,llrbnode__t *);
static llrbnode__t	*delete_min	(llrbnode__t *,void (*)(void *));
static llrbnode__t	*delete		(llrbtree__t *,llrbnode__t *,void *);

/**************************************************************************/

static inline bool is_red(llrbnode__t *n)
{
  if (n == NULL) return false;
  return n->red;
}

/**************************************************************************/

static inline void color_flip(llrbnode__t *n)
{
  assert(n        != NULL);
  assert(n->left  != NULL);
  assert(n->right != NULL);
  
  n->red        = !n->red;
  n->left->red  = !n->left->red;
  n->right->red = !n->right->red;
}

/**************************************************************************/

static inline llrbnode__t *get_min(llrbnode__t *n)
{
  assert(n != NULL);
  
  while(n->left)
    n = n->left;
  
  return n;
}

/************************************************************************/

void LLRBTreeInsert(
	llrbtree__t *tree,
	void        *key,
	llrbnode__t *node
)
{
  assert(tree != NULL);
  assert(key  != NULL);
  assert(node != NULL);
  
  tree->left      = insert(tree,tree->left,key,node);
  tree->left->red = false;
}

/**************************************************************************/

void *LLRBTreeFind(
	llrbtree__t *tree,
	void        *key
)
{
  llrbnode__t *node;
  
  assert(tree != NULL);
  assert(key  != NULL);

  node = tree->left;
  
  while(node != NULL)
  {
    int rc = (*tree->cmp)(key,node);
    
    if (rc < 0)
      node = node->left;
    else if (rc == 0)
      return node;
    else
      node = node->right;
  }
  
  return NULL;
}

/**************************************************************************/

void LLRBTreeDelete(
	llrbtree__t *tree,
	void        *key
)
{
  assert(tree != NULL);
  assert(key  != NULL);
  
  tree->left = delete(tree,tree->left,key);
  if (tree->left) tree->left->red = false;
}

/**************************************************************************/

static llrbnode__t *rotate_left(llrbnode__t *n)
{
  llrbnode__t *x;
  
  assert(n != NULL);

  x            = n->right;
  n->right     = x->left;
  x->left      = n;
  x->red       = x->left->red;
  x->left->red = true;
  return x;
}

/**************************************************************************/

static llrbnode__t *rotate_right(llrbnode__t *n)
{
  llrbnode__t *x;
  
  assert(n != NULL);
  
  x             = n->left;
  n->left       = x->right;
  x->right      = n;
  x->red        = x->right->red;
  x->right->red = true;
  return x;
}

/**************************************************************************/

static llrbnode__t *move_red_left(llrbnode__t *n)
{
  assert(n        != NULL);
  assert(n->right != NULL);
  
  color_flip(n);
  if (is_red(n->right->left))
  {
    n->right = rotate_right(n->right);
    n = rotate_left(n);
    color_flip(n);
  }
  
  return n;
}

/**************************************************************************/

static llrbnode__t *move_red_right(llrbnode__t *n)
{
  assert(n             != NULL);
  assert(n->left       != NULL);
  assert(n->left->left != NULL);
  
  color_flip(n);
  if (is_red(n->left->left))
  {
    n = rotate_right(n);
    color_flip(n);
  }
  return n;
}

/**************************************************************************/

static llrbnode__t *fix_up(llrbnode__t *n)
{
  assert(n != NULL);
  
  if (is_red(n->right))
    n = rotate_left(n);
  
  if (is_red(n->left) && is_red(n->left->left))
    n = rotate_right(n);
  
  if (is_red(n->left) && is_red(n->right))
    color_flip(n);
  
  return n;
}

/**************************************************************************/

static llrbnode__t *insert(
	llrbtree__t *tree,
	llrbnode__t *h,
	void        *key,
	llrbnode__t *node
)
{
  assert(tree != NULL);
  assert(key  != NULL);
  assert(node != NULL);
  
  if (h == NULL)
  {
    node->left  = NULL;
    node->right = NULL;
    node->red   = true;
    return node;
  }
  
  if (is_red(h->left) && is_red(h->right))
    color_flip(h);
  
  int rc = (*tree->cmp)(key,h);
  if (rc < 0)
    h->left = insert(tree,h->left,key,node);
  else if (rc == 0)
    (*tree->upd)(h,key);
  else
    h->right = insert(tree,h->right,key,node);
  
  if (is_red(h->right))
    h = rotate_left(h);
  
  if (is_red(h->left) && is_red(h->left->left))
    h = rotate_right(h);
  
  return h;
}

/**************************************************************************/

static llrbnode__t *delete_min(llrbnode__t *h,void (*del)(void *))
{
  assert(h != NULL);
  assert(del);
  
  if (h->left == NULL)
  {
    assert(h->right == NULL);
    (*del)(h);
    return NULL;
  }
  
  if (!is_red(h->left) && !is_red(h->left->left))
    h = move_red_left(h);
  
  h->left = delete_min(h->left,del);
  return fix_up(h);
}

/**************************************************************************/

static llrbnode__t *delete(llrbtree__t *tree,llrbnode__t *h,void *key)
{
  assert(tree != NULL);
  assert(h    != NULL);
  assert(key  != NULL);
  
  if ((*tree->cmp)(key,h) < 0)
  {
    if (!is_red(h->left) && !is_red(h->left->left))
      h = move_red_left(h);
    h->left = delete(tree,h->left,key);
  }
  else
  {
    if (is_red(h->left))
      h = rotate_right(h);
    
    if (((*tree->cmp)(key,h) == 0) && (h->right == NULL))
    {
      (*tree->del)(h);
      return NULL;
    }
    
    if (!is_red(h->right) && !is_red(h->right->left))
      h = move_red_right(h);
    
    if ((*tree->cmp)(key,h) == 0)
    {
      llrbnode__t *m = get_min(h->right);
      (*tree->upd)(h,m);
      h->right       = delete_min(h->right,tree->del);
    }
    else
      h->right = delete(tree,h->right,key);
  }
  
  return fix_up(h);
}

/**************************************************************************/

