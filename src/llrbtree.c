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

static inline bool		 is_red		(const llrbnode__t *const);
static inline void 		 color_flip	(llrbnode__t *) __attribute__((nonnull,nothrow));
static inline llrbnode__t	*get_min	(llrbnode__t *) __attribute__((nonnull,nothrow));

static llrbnode__t	*rotate_left	(llrbnode__t *) __attribute__((nonnull,nothrow));
static llrbnode__t	*rotate_right	(llrbnode__t *) __attribute__((nonnull,nothrow));
static llrbnode__t	*move_red_left	(llrbnode__t *) __attribute__((nonnull,nothrow));
static llrbnode__t	*move_red_right	(llrbnode__t *) __attribute__((nonnull,nothrow));
static llrbnode__t	*fix_up		(llrbnode__t *) __attribute__((nonnull,nothrow));
static llrbnode__t	*insert		(llrbnode__t *,all__t,all__t,int (*)(all__t,all__t)) __attribute__((nonnull(4),nothrow));
static llrbnode__t	*delete_min	(llrbnode__t *) __attribute__((nonnull,nothrow));
static llrbnode__t	*delete		(llrbnode__t *,all__t,int (*)(all__t,all__t)) __attribute__((nonnull(3),nothrow));

/**************************************************************************/

static inline bool is_red(const llrbnode__t *const n)
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

void LLRBTreeInsert(llrbtree__t *const tree,all__t key,all__t value)
{
  tree->left      = insert(tree->left,key,value,tree->cmp);
  tree->left->red = false;
}

/**************************************************************************/

bool LLRBTreeFind(llrbtree__t *const tree,all__t key,all__t *pvalue)
{
  llrbnode__t *node;
  
  assert(tree   != NULL);
  assert(pvalue != NULL);
  
  node = tree->left;
  
  while(node != NULL)
  {
    int rc ;
    
    rc = (*tree->cmp)(key,node->key);
    
    if (rc == 0)
    {
      *pvalue = node->value;
      return true;
    }
    else if (rc < 0)
      node = node->left;
    else
      node = node->right;
  }
  
  return false;
}

/**************************************************************************/

void LLRBTreeDelete(llrbtree__t *const tree,all__t key)
{
  tree->left = delete(tree->left,key,tree->cmp);
  tree->left->red = false;
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
  
  x             = n->right;
  n->left       = x->right;
  x->right      = n;
  x->red        = x->right->red;
  x->right->red = true;
  return x;
}

/**************************************************************************/

static llrbnode__t *move_red_left(llrbnode__t *n)
{
  assert(n               != NULL);
  assert(n->right        != NULL);
  assert(n->right->right != NULL);
  
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
	llrbnode__t  *h,
	all__t        key,
	all__t        value,
	int         (*cmp)(all__t,all__t)
)
{
  assert(cmp);
  
  if (h == NULL)
  {
    h = malloc(sizeof(llrbnode__t));
    if (h == NULL)
      return NULL;
    
    h->left  = NULL;
    h->right = NULL;
    h->key   = key;
    h->value = value;
    h->red   = true;
    
    return h;
  }
  
  if (is_red(h->left) && is_red(h->right))
    color_flip(h);
  
  int rc = (*cmp)(key,h->key);
  if (rc < 0)
    h->left = insert(h->left,key,value,cmp);
  else if (rc == 0)
    h->value = value;
  else
    h->right = insert(h->right,key,value,cmp);
  
  if (is_red(h->right))
    h = rotate_left(h);
  
  if (is_red(h->left) && is_red(h->left->left))
    h = rotate_right(h);
  
  return h;
}

/**************************************************************************/

static llrbnode__t *delete_min(llrbnode__t *h)
{
  assert(h != NULL);
  
  if (h->left == NULL)
  {
    free(h);
    return NULL;
  }
  
  if (!is_red(h->left) && !is_red(h->left->left))
    h = move_red_left(h);
  
  h->left = delete_min(h->left);
  return fix_up(h);
}

/**************************************************************************/

static llrbnode__t *delete(
	llrbnode__t  *h,
	all__t        key,
	int         (*cmp)(all__t,all__t)
)
{
  if ((*cmp)(key,h->key) < 0)
  {
    if (!is_red(h->left) && !is_red(h->left->left))
      h = move_red_left(h);
    h->left = delete(h->left,key,cmp);
  }
  else
  {
    if (is_red(h->left))
      h = rotate_right(h);
    
    if (((*cmp)(key,h->key) == 0) && (h->right == NULL))
    {
      free(h);
      return NULL;
    }
    
    if (!is_red(h->right) && !is_red(h->right->left))
      h = move_red_right(h);
    
    if ((*cmp)(key,h->value) == 0)
    {
      llrbnode__t *m = get_min(h->right);
      h->value = m->value;
      h->key   = m->key;
      h->right = delete_min(h->right);
    }
    else
      h->right = delete(h->right,key,cmp);
  }
  
  return fix_up(h);
}

/**************************************************************************/

