/***************************************************************************
*
* Copyright 2013 by Sean Conner.
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

#include <stdlib.h>
#include <assert.h>
#include "tree.h"

/*************************************************************************/

static int      tree_delta      (tree__s *);
static tree__s  *tree_rotl      (tree__s *);
static tree__s  *tree_rotr      (tree__s *);
static tree__s  *tree_balance   (tree__s *);

/*************************************************************************/

static int tree_delta(tree__s *self)
{
  assert(self != NULL);
  return (self->left  ? self->left->height  : 0)
       - (self->right ? self->right->height : 0);
}

/*************************************************************************/

static tree__s *tree_rotl(tree__s *self)
{
  assert(self != NULL);
  tree__s *r  = self->right;
  self->right = r->left;
  r->left     = tree_balance(self);
  return tree_balance(r);
}

/*************************************************************************/

static tree__s *tree_rotr(tree__s *self)
{
  assert(self != NULL);
  tree__s *l = self->left;
  self->left = l->right;
  l->right   = tree_balance(self);
  return tree_balance(l);
}

/*************************************************************************/

static tree__s *tree_balance(tree__s *self)
{
  assert(self != NULL);
  int delta = tree_delta(self);
  
  if (delta < -1)
  {
    if (tree_delta(self->right) > 0)
      self->right = tree_rotr(self->right);
    return tree_rotl(self);
  }
  else if (delta > 1)
  {
    if (tree_delta(self->left) < 0)
      self->left = tree_rotl(self->left);
    return tree_rotr(self);
  }
  
  self->height = 0;
  
  if (self->left && (self->left->height > self->height))
    self->height = self->left->height;
    
  if (self->right && (self->right->height > self->height))
    self->height = self->right->height;
    
  self->height += 1;
  return self;
}

/*************************************************************************/

static tree__s *tree_move_right(tree__s *restrict self,tree__s *restrict rhs)
{
  if (self == NULL)
    return rhs;
    
  self->right = tree_move_right(self->right,rhs);
  return tree_balance(self);
}

/*************************************************************************/

tree__s *tree_insert(
        tree__s *restrict self,
        tree__s *restrict item,
        int (*compare)(void const *restrict,void const *restrict)
)
{
  int rc;
  
  if (self == NULL)
    return item;
    
  rc = (*compare)(item,self);
  
  if (rc < 0)
    self->left = tree_insert(self->left,item,compare);
  else
    self->right = tree_insert(self->right,item,compare);
    
  return tree_balance(self);
}

/*************************************************************************/

tree__s *tree_find(
        tree__s    *self,
        void const *item,
        int (*compare)(void const *restrict,void const *restrict)
)
{
  int rc;
  
  if (self == NULL)
    return NULL;
    
  rc = (*compare)(item,self);
  
  if (rc < 0)
    return tree_find(self->left,item,compare);
  else if (rc == 0)
    return self;
  else
    return tree_find(self->right,item,compare);
}

/*************************************************************************/

tree__s *tree_remove(
        tree__s     *self,
        void const  *item,
        int        (*compare)(void const *restrict,void const *restrict),
        tree__s    **remove
)
{
  int rc;
  
  if (self == NULL)
    return NULL;
    
  rc = (*compare)(item,self);
  
  if (rc < 0)
    self->left = tree_remove(self->left,item,compare,remove);
  else if (rc == 0)
  {
    if (remove != NULL)
      *remove = self;
      
    tree__s *tmp = tree_move_right(self->left,self->right);
    return tmp;
  }
  else
    self->right = tree_remove(self->right,item,compare,remove);
    
  return tree_balance(self);
}

/*************************************************************************/

