
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

/*************************************************************************/

static void		*heap_new	(size_t,void *);
static size_t		 heap_tspace	(const struct heap *);
static size_t		 heap_fspace	(const struct heap *);
static size_t		 heap_uspace	(const struct heap *);
static void		 heap_free	(const struct heap *);

static void		*mem_alloc	(const struct heap *,size_t);
static void		*mem_grow	(const struct heap *,const void *,size_t,size_t);
static void		*mem_shrink	(const struct heap *,const void *,size_t,size_t);
static void		 mem_free	(const struct heap *,const void *,size_t);

static void		*mem_resize	(const struct heap *,const void *,size_t,size_t);

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
  free((const Heap)pheap);
}

/***************************************************************************/

static void *mem_alloc(const struct heap *pheap,size_t size)
{
  void *p;
  int   rc;
  
  ddt(pheap != NULL);
  ddt(size  > 0);
  ddt(size  < (130L*1024L));

  do
  {
    rc = ERR_OKAY;
    p  = malloc(size);
    if (p == NULL)
    {
      ErrorPush(CgiErr,MEMALLOC,ERR_NOMEMORY,"");
      rc = ErrThrow(&MemoryErr);
    }
  } while(rc == RETRY);

  return(p);
}

/*******************************************************************/

static void *mem_grow(const struct heap *pheap,const void *p,size_t os,size_t ns)
{
  ddt(pheap != NULL);
  ddt(p     != NULL);
  ddt(os    >  0);
  ddt(ns    >  0);
  ddt(os    <  ns);

  return(mem_resize(pheap,p,os,ns));
}

/**********************************************************************/

void *mem_shrink(const struct heap *pheap,const void *p,size_t os,size_t ns)
{
  ddt(pheap != NULL);
  ddt(p     != NULL);
  ddt(os    >  0);
  ddt(ns    >  0);
  ddt(os    > ns);

  return(mem_resize(pheap,p,os,ns));
}

/***********************************************************************/

void *mem_resize(const struct heap *pheap,const void *p,size_t os,size_t ns)
{
  void *np;
  int   rc;
  
  ddt(pheap != NULL);
  
  do
  {
    rc = ERR_OKAY;
    np = realloc((const Heap)p,ns);
    if (np == NULL)
    {
      ErrorPush(CgiErr,MEMSHRINK,ERR_NOMEMORY,"");
      rc = ErrThrow(&MemoryErr);
    }
  } while(rc == RETRY);
  
  return(np);
}

/************************************************************************/

void mem_free(const struct heap *pheap,const void *p,size_t size)
{
  ddt(pheap != NULL);
  ddt(p     != NULL);
  ddt(size  >  0);
  
  free((const Heap)p);
}

/**************************************************************************/

