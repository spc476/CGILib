
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
#include "../errors.h"
#include "../memory.h"
#include "../ddt.h"
#include "imem.h"

#define DEFHEAPSIZE		(128L * 1024L)

/*************************************************************************/

typedef union align
{
  double   d;
  void    *p;
  int    (*f)();
} ALIGN;

struct heapnode
{
  Node           node;
  size_t         size;
  size_t         offs;
  unsigned char *mem;
};

struct myheap 
{
  struct heap      base;
  struct heapnode *cheap;
  List             heaps;
  Heap             parentheap;
};

/********************************************************************/

static void		*heap_new	(size_t,void *);
static size_t		 heap_tspace	(const struct heap *);
static size_t		 heap_fspace	(const struct heap *);
static size_t		 heap_uspace	(const struct heap *);
static void		 heap_free	(const struct heap *);

static void		*mem_alloc	(const struct heap *,size_t);
static void		*mem_grow	(const struct heap *,const void *,size_t,size_t);
static void		*mem_shrink	(const struct heap *,const void *,size_t,size_t);
static void		 mem_free	(const struct heap *,const void *,size_t);

static void		*spcheap_expand	(struct myheap *const);

/**************************************************************************/

static const struct memvector vector = 
{
  heap_new,
  heap_tspace,
  heap_fspace,
  heap_uspace,
  heap_free,
  mem_alloc,
  mem_grow,
  mem_shrink,
  mem_free
};

/*************************************************************************/

int (SpcHeap)(Heap *pheap)
{
  ddt(pheap != NULL);
  
  return(HeapNew(pheap,sizeof(struct myheap),&vector,NULL));
}

/*************************************************************************/

static void *heap_new(size_t hsize,void *data)
{
  struct myheap *heap;
  Heap           parentheap;
  int            rc;
   
  ddt(hsize == sizeof(struct myheap));
  ddt(data  == NULL);
  
  rc = StandardCHeap(&parentheap);
  if (rc != ERR_OKAY) return(NULL);
  heap = HeapMemAlloc(parentheap,hsize);
  if (heap == NULL) 
  {
    HeapFree(&parentheap);
    return(heap);
  }
  heap->parentheap = parentheap;
  ListInit(&heap->heaps);
  spcheap_expand(heap);
  return(heap);
}

/**********************************************************************/

static size_t heap_tspace(const struct heap *pheap)
{
  ddt(pheap != NULL);
  return(ULONG_MAX);
}

/***********************************************************************/

static size_t heap_fspace(const struct heap *pheap)
{
  ddt(pheap != NULL);
  return(
          ULONG_MAX 
          - pheap->stats[STATALLOC].bytes 
          - pheap->stats[STATGROW].bytes
          + pheap->stats[STATSHRINK].bytes
          + pheap->stats[STATFREE].bytes
        );
}

/************************************************************************/

static size_t heap_uspace(const struct heap *pheap)
{
  ddt(pheap != NULL);
  return(
          pheap->stats[STATALLOC].bytes
          + pheap->stats[STATGROW].bytes
          - pheap->stats[STATSHRINK].bytes
          - pheap->stats[STATFREE].bytes
        );
}

/**************************************************************************/

static void heap_free(const struct heap *pheap)
{
  Heap                 parentheap;
  const struct myheap *pmh = (const struct myheap *)pheap;
  struct heapnode     *phn;
  
  ddt(pheap != NULL);
  
  parentheap = pmh->parentheap;
  for (
        phn = (struct heapnode *)ListRemHead((List *const)&pmh->heaps);
        NodeValid(&phn->node);
        phn = (struct heapnode *)ListRemHead((List *const)&pmh->heaps)
      )
  {
    HeapMemFree(parentheap,phn->mem,phn->size);
    HeapMemFree(parentheap,phn,sizeof(struct heapnode));
  }
  HeapMemFree(parentheap,pheap,sizeof(struct myheap));
  HeapFree(&parentheap);
}

/***************************************************************************/

static void *mem_alloc(const struct heap *pheap,size_t size)
{
  struct myheap   *ph = (struct myheap *)pheap;
  void            *p;
  struct heapnode *phn;

  ddt(pheap != NULL);    
  ddt(size  > 0);

  phn = ph->cheap;
  
  if ((phn->offs + size + ((size / sizeof(ALIGN)) + 1) * sizeof(ALIGN)) > phn->size)
    phn = spcheap_expand(ph);
  
  ddt(size < phn->size);
  
  p          = &phn->mem[phn->offs];
  phn->offs += ((size / sizeof(ALIGN)) + 1) * sizeof(ALIGN);
  ddt(phn->offs < phn->size);
  return(p);
}

/*******************************************************************/

static void *mem_grow(const struct heap *const pheap,const void *p,size_t os,size_t ns)
{
  void *np;
  
  ddt(pheap != NULL);
  ddt(p     != NULL);
  ddt(os    >  0);
  ddt(ns    >  0);
  ddt(os    <  ns);

  np = HeapMemAlloc((const Heap)pheap,ns);
  memcpy(np,p,os);
  HeapMemFree((const Heap)pheap,p,os);
  return(np);
}

/**********************************************************************/

void *mem_shrink(const struct heap *const pheap,const void *p,size_t os,size_t ns)
{
  void *np;
  
  ddt(pheap != NULL);
  ddt(p     != NULL);
  ddt(os    >  0);
  ddt(ns    >  0);
  ddt(os    > ns);

  np = HeapMemAlloc((const Heap)pheap,ns);
  memcpy(np,p,ns);
  HeapMemFree((const Heap)pheap,p,os);
  return(np);
}

/***********************************************************************/

void mem_free(const struct heap *pheap,const void *p,size_t size)
{
  ddt(pheap != NULL);
  ddt(p     != NULL);
  ddt(size  >  0);
  
  D(memset(p,PADDING,size);)
}

/**************************************************************************/

static void *spcheap_expand(struct myheap *const pheap)
{
  struct heapnode *phn;

  ddt(pheap != NULL);
  
  phn          = HeapMemAlloc(pheap->parentheap,sizeof(struct heapnode));
  phn->mem     = HeapMemAlloc(pheap->parentheap,DEFHEAPSIZE);
  phn->size    = DEFHEAPSIZE;
  phn->offs    = 0;
  pheap->cheap = phn;
  ListAddTail(&pheap->heaps,&phn->node);
  D(memset(phn->mem,PADDING,DEFHEAPSIZE);)
  return(phn);  
}

/***********************************************************************/

