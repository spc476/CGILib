
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
#include "imem.h"

/**************************************************************************/
  
static int		 mem_errhandler		(Error);

/**************************************************************************/

int (MemInit)(void)
{
  int rc;
  
  /*-----------------------------------------------------------
  ; the order of the following operations is critical.  In fact,
  ; a lot of the library assumes that the memory sub-system is
  ; up and fully operational, but at this point, it isn't.  So, with
  ; that in mind, we need to be aware of what is and isn't safe
  ; to call.
  ;
  ; ErrHandlerNew() is one such safe call IN THIS CASE, since we 
  ; are passing static memory to the call.  ErrHandlerPush() isn't
  ; safe until we can actually allocate memory through this subsystem,
  ; but's that okay if we can't register a handler because the default
  ; action of an empty handler chain is to return the error.
  ;
  ; So we create an empty error handler chain, then create our heap,
  ; then register a default handler (if we can).
  ;
  ; Theoretically, we should do the following:
  ;
  ;	rc = StandardCHeap(&lvheap);
  ;	if (rc != ERR_OKAY) return(rc);
  ;	HeapDefault(lvheap);
  ;
  ; But really, since this IS the implementation, and we KNOW what
  ; HeapDefault() does (and in fact, we can see it here) we can 
  ; just skip the HeapDefault() call.
  ; ------------------------------------------------------------*/
  
  ErrHandlerNew(&MemoryErr);
  if ((rc = StandardCHeap(&HeapMain)) != ERR_OKAY)
    return(rc);
  ErrHandlerPush(&MemoryErr,mem_errhandler);
  return(ERR_OKAY);
}

/************************************************************************/

Heap (HeapDefault)(const Heap heap)
{
  Heap retheap = HeapMain;
  
  ddt(heap != NULL);
  
  HeapMain     = heap;
  return(retheap);
}

/***********************************************************************/

int (HeapNew)(Heap *pheap,size_t hsize,const struct memvector *pvec,void *datum)
{
  Heap             heap;
  struct memstats *pstats;
  int              i;
  
  ddt(pheap != NULL);
  ddt(hsize >  0);
  ddt(pvec  != NULL);

  heap = (*pvec->hnew)(hsize,datum);
  if (heap == NULL)
    return(ErrorPush(CgiErr,HEAPNEW,ERR_NOMEMORY,""));
    
  heap->size   = hsize;
  heap->data   = datum;
  heap->vector = pvec;
  for (i = 0 , pstats = &heap->stats[0] ; i < STATMAX ; i++)
  {
    pstats[i].bytes = pstats[i].calls = pstats[i].max = 0;
    pstats[i].min   = ULONG_MAX;
  }
  
  *pheap = heap;
  return(ERR_OKAY);
}

/**************************************************************************/

void *(HeapMemAlloc)(const Heap heap,size_t size)
{
  void *p;
  
  ddt(heap != NULL);
  ddt(size >  0);
  
  heap->stats[STATALLOC].bytes += size;
  heap->stats[STATALLOC].calls ++;
  if (size < heap->stats[STATALLOC].min) heap->stats[STATALLOC].min = size;
  if (size > heap->stats[STATALLOC].max) heap->stats[STATALLOC].max = size;

  p = (*heap->vector->alloc)(heap,size + FUDGE);
  D(memset(p,PADDING,size + FUDGE);)
  return((void *)((unsigned char *)(p) + HFUDGE));
}

/*************************************************************************/

void *(HeapMemGrow)(const Heap heap,const void *p,size_t os,size_t ns)
{
  unsigned long delta;
  
  ddt(heap != NULL);
  ddt(p    != NULL);
  ddt(os   >  0);
  ddt(ns   >  0);
  ddt(ns   >  os);
  
  delta = ns - os;
  heap->stats[STATGROW].bytes += delta;
  heap->stats[STATGROW].calls ++;
  if (delta < heap->stats[STATGROW].min) heap->stats[STATGROW].min = delta;
  if (delta > heap->stats[STATGROW].max) heap->stats[STATGROW].max = delta;

# ifdef DDT
  {
    void *np;
    
    np = HeapMemAlloc(heap,ns);
    memcpy(np,p,os);
    HeapMemFree(heap,p,os);
    return(np);
  }
# else  
    return((*heap->vector->grow)(heap,p,os,ns));
# endif
}

/*************************************************************************/

void *(HeapMemShrink)(const Heap heap,const void *p,size_t os,size_t ns)
{
  unsigned long delta;
  
  ddt(heap != NULL);
  ddt(p    != NULL);
  ddt(os   >  0);
  ddt(ns   >  0);
  ddt(ns   <= os);
  
  delta = os - ns;
  heap->stats[STATSHRINK].bytes += delta;
  heap->stats[STATSHRINK].calls ++;
  if (delta < heap->stats[STATGROW].min) heap->stats[STATSHRINK].min = delta;
  if (delta > heap->stats[STATGROW].max) heap->stats[STATSHRINK].max = delta;

# ifdef DDT
  {
    void *np;
    
    np = HeapMemAlloc(heap,ns);
    memcpy(np,p,ns);
    HeapMemFree(heap,p,os);
    return(np);
  }
# else
    return((*heap->vector->grow)(heap,p,os,ns));
# endif
}

/**************************************************************************/

void *(HeapMemResize)(const Heap heap,const void *p,size_t os,size_t ns)
{
  ddt(heap != NULL);
  
  if (p != NULL)
  {
    if (ns == 0)
    {
      ddt(os > 0);
      HeapMemFree(heap,p,os);
      return(NULL);
    }
    else if (ns < os)
      return(HeapMemShrink(heap,p,os,ns));
    else if (ns > os)
      return(HeapMemGrow(heap,p,os,ns));
    else
      ddt(0);
  }
  else
    return(HeapMemAlloc(heap,ns));
  
  ddt(0);
  return((void *)p);
}

/**************************************************************************/

void (HeapMemFree)(const Heap heap,const void *p,size_t size)
{
  ddt(heap != NULL);
  ddt(size >  0);
  
  heap->stats[STATFREE].bytes += size;
  heap->stats[STATFREE].calls ++;
  if (size < heap->stats[STATFREE].min) heap->stats[STATFREE].min = size;
  if (size > heap->stats[STATFREE].max) heap->stats[STATFREE].max = size;

# ifdef DDT
  {
    unsigned char *pu;
    int            i;

    for ( 
          pu = (unsigned char *)(p) - HFUDGE , i = 0 ;
          i < HFUDGE ;
          pu++ , i++
        )
    {
      if (*pu != PADDING)
      {
        ddtlog(
                ddtstream,
                "p i L i",
                "%a front mem guard corrupt at %b size %c char %d",
                (void *)p,
                i,
                size,
                *pu
              );
        dump_memory(ddtstream->buffer,p,(size < 128) ? size : 128 , (size_t)p);
	ddtlog(ddtstream,"","-----");
        dump_memory(ddtstream->buffer,(unsigned char *)(p) - HFUDGE , HFUDGE , (size_t)(p) - HFUDGE);
        ddt(*pu == PADDING);
      }
    }

    for (
          pu = (unsigned char *)(p) + size, i = 0 ;
          i < HFUDGE ;
          pu++ , i++
        )
    {
      if (*pu != PADDING)
      {
        ddtlog(
                ddtstream,
                "p i L i",
                "%a back mem guard corrupt at %b size %c char %d",
                (void *)p,
                i,
                size,
                *pu
              );
        dump_memory(ddtstream->buffer,p,(size < 128) ? size : 128 , (size_t)p);
        dump_memory(ddtstream->buffer,(unsigned char *)(p) + size, HFUDGE , (size_t)(p) + size);
        ddt(*pu == PADDING);
      }
    }

    pu = (unsigned char *)(p) - HFUDGE;	/* fill memory with garbage */
    memset(pu,PADDING,size+FUDGE);
    p     = pu;
    size += FUDGE;
    /*(*heap->vector->free)(heap,pu,size+FUDGE);*/
  }
# endif
  (*heap->vector->free)(heap,p,size);
}

/**************************************************************************/

size_t (HeapSpaceTotal)(const Heap heap)
{
  ddt(heap != NULL);
  return((*heap->vector->htspace)(heap));
}

/************************************************************************/

size_t (HeapSpaceFree)(const Heap heap)
{
  ddt(heap != NULL);
  return((*heap->vector->hfspace)(heap));
}

/************************************************************************/

size_t (HeapSpaceUsed)(const Heap heap)
{
  ddt(heap != NULL);
  return((*heap->vector->huspace)(heap));
}

/************************************************************************/

int (HeapMemStats)(const Heap heap,struct memstats *pstats,size_t n,int calls)
{
  int i;
  /*int mask;*/
  
  ddt(pstats          != NULL);
  ddt(n               >  0);
  ddt(n               <= 5);
  ddt((calls & C_MAX) == 0);

#if 1
  for (i = 0  ; i < (int)n ; i++)
  {
    pstats[i].bytes = heap->stats[i].bytes;
    pstats[i].calls = heap->stats[i].calls;
    pstats[i].min   = heap->stats[i].min;
    pstats[i].max   = heap->stats[i].max;
  }
#else
  for (mask = 0 , i = 0 ; (mask < 5) && (i < n) ; mask++)
  {
    if ((calls & (1 << mask)) == (1 << mask)) 
    {
      pstats[i].bytes = heap->stats[mask].bytes;
      pstats[i].calls = heap->stats[mask].calls;
      pstats[i].min   = heap->stats[mask].min;
      pstats[i].max   = heap->stats[mask].max;
      i++;
    }
  }
#endif
  return(i);
}

/***************************************************************************/

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

void *(MemGrow)(const void *p,size_t os,size_t ns)
{
  ddt(p  != NULL);
  ddt(os >  0);
  ddt(ns >  0);
  ddt(os <  ns);
        
  return(HeapMemGrow(HeapMain,p,os,ns));
}

/*************************************************************************/

void *(MemShrink)(const void *p,size_t os,size_t ns)
{
  ddt(p  != NULL);
  ddt(os >  0);
  ddt(ns >  0);
  ddt(os >= ns);
  
  return(HeapMemShrink(HeapMain,p,os,ns));
}

/***************************************************************************/

void *(MemResize)(const void *p,size_t os,size_t ns)
{
  return(HeapMemResize(HeapMain,p,os,ns));
}

/*************************************************************************/

void (MemFree)(const void *p,size_t size)
{
  ddt(p    != NULL);
  ddt(size >  0);
  
  HeapMemFree(HeapMain,p,size);
}

/*************************************************************************/

int (MemStats)(struct memstats *pstat,size_t num,int flags)
{
  ddt(pstat != NULL);
  ddt(num   >  0);
  
  return(HeapMemStats(HeapMain,pstat,num,flags));
}

/**********************************************************************/

static int mem_errhandler(Error error)
{
  ddtlog(ddtstream,"","out of memory");
  return(ABORT);
}

