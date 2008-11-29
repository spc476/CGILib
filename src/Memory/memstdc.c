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

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../types.h"
#include "../nodelist.h"
#include "../memory.h"
#include "../ddt.h"

/*************************************************************************/

static void		*heap_new	(size_t,void *);
static void		 heap_free	(const struct heap *);
static void		*mem_alloc	(const struct heap *,size_t);
static void		*mem_resize	(const struct heap *,void *,size_t);
static void		 mem_free	(const struct heap *,void *);

/**************************************************************************/

static const struct memvector vector = 
{
  heap_new,
  heap_free,
  mem_alloc,
  mem_resize,
  mem_free
};

/*************************************************************************/

int (StandardCHeap)(Heap *pheap)
{
  ddt(pheap != NULL);
  
  return(HeapNew(pheap,sizeof(struct heap),&vector,NULL));
}

/*************************************************************************/

static void *heap_new(size_t hsize,void *data)
{
  ddt(hsize == sizeof(struct heap));
  ddt(data  == NULL);
  
  return(malloc(hsize));
}

/**********************************************************************/

static void heap_free(const struct heap *pheap)
{
  free((const Heap)pheap);
}

/***************************************************************************/

static void *mem_alloc(const struct heap *pheap,size_t size)
{
  void *p;
  
  ddt(pheap != NULL);
  ddt(size  > 0);
  ddt(size  < (130L*1024L));

  p = malloc(size);
  if (p == NULL)
    ddt(0);
  
  return(p);
}

/*******************************************************************/

void *mem_resize(const struct heap *pheap,void *p,size_t ns)
{
  void *np;
  
  ddt(pheap != NULL);
  ddt(ns    >  0); 
  
  np = realloc(p,ns);
  if (np == NULL)
    ddt(0);
  
  return(np);
}

/************************************************************************/

void mem_free(const struct heap *pheap,void *p)
{
  ddt(pheap != NULL);

  free(p);
}

/**************************************************************************/

