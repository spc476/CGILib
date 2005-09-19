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
#include "../util.h"

#ifdef MIPS
#  define PADDING	0xB1
#  define NULLP		0xB1B1B1B1
#elif defined __i386__
#  define PADDING	0xCC
#  define NULLP		0xCCCCCCCC
#elif defined MC68K
#  define PADDING	0xA1
#  define NULLP		0xA1A1A1A1
#elif defined RS6000
#  define PADDING	0xB1
#  define NULLP		0xB1B1B1B1
#else
#  error Please define system
#endif

/*#ifdef DDT*/
#if 0
#  define FUDGE		64
#  define HFUDGE	(FUDGE/2)
#  define NULLPTR(p)	((void *)(p)) == ((void *)NULLP)
#else
#  define FUDGE		0
#  define HFUDGE	0
#  define NULLPTR(p)	(p) == NULL
#endif

/**************************************************************************/
  
int (MemInit)(void)
{
  int rc;
  
  if ((rc = StandardCHeap(&HeapMain)) != ERR_OKAY)
    return(rc);
  return(ERR_OKAY);
}

/************************************************************************/

Heap (HeapDefault)(const Heap heap)
{
  Heap retheap = HeapMain;
  
  ddt(heap != NULL);
  
  HeapMain = heap;
  return(retheap);
}

/***********************************************************************/

int (HeapNew)(Heap *pheap,size_t hsize,const struct memvector *pvec,void *datum)
{
  Heap heap;
  
  ddt(pheap != NULL);
  ddt(hsize >  0);
  ddt(pvec  != NULL);

  heap = (*pvec->hnew)(hsize,datum);
  if (heap == NULL)
    return(ERR_ERR);
    
  heap->size   = hsize;
  heap->data   = datum;
  heap->vector = pvec;
  *pheap       = heap;
  return(ERR_OKAY);
}

/**************************************************************************/

void *(HeapMemAlloc)(const Heap heap,size_t size)
{
  void *p;
  
  ddt(heap != NULL);
  ddt(size >  0);

  p = (*heap->vector->alloc)(heap,size + FUDGE);
  D(memset(p,PADDING,size + FUDGE);)
  return((void *)((unsigned char *)(p) + HFUDGE));
}

/*************************************************************************/

void *(HeapMemResize)(const Heap heap,void *p,size_t ns)
{
  ddt(heap != NULL);
  ddt(ns   >  0);

  return((*heap->vector->resize)(heap,(unsigned char *)(p) - HFUDGE,ns));
}

/*************************************************************************/

void (HeapMemFree)(const Heap heap,void *p)
{
  ddt(heap != NULL);

  (*heap->vector->free)(heap,(unsigned char *)(p) - HFUDGE);
}

/**************************************************************************/

void (HeapFree)(Heap *pheap)
{
  struct heap *heap;
  
  ddt(pheap  != NULL);
  ddt(*pheap != NULL);
  
  heap = *pheap;
  (*heap->vector->hfree)(*pheap);
  *pheap = NULL;
}

/*************************************************************************/

void *(MemAlloc)(size_t size)
{
  ddt(size > 0);
  
  return(HeapMemAlloc(HeapMain,size));
}

/************************************************************************/

void *(MemResize)(void *p,size_t ns)
{
  return(HeapMemResize(HeapMain,p,ns));
}

/*************************************************************************/

void (MemFree)(void *p)
{
  ddt(p != NULL);
  
  HeapMemFree(HeapMain,p);
}

/*************************************************************************/

