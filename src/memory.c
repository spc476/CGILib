
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

#include "types.h"
#include "nodelist.h"
#include "errors.h"
#include "memory.h"
#include "ddt.h"

#ifdef DDT
#  define FUDGE		64
#  define HFUDGE	(FUDGE/2)
#else
#  define FUDGE		0
#  define HFUDGE	0
#endif

#ifdef SGI
#  define PADDING	0xB1
#elif defined LINUX
#  define PADDING	0xCC
#  define PADDALL	0xCC
#  define PADDFREE	0xDD
#elif defined AIX
#  define PADDING	0xB1
#else
#  error Please define system
#endif

/*************************************************************************/

typedef union align
{
  double   d;
  void    *p;
  int    (*f)();
} ALIGN;

struct heapnode
{
  struct Node    node;
  size_t         size;
  size_t         offs;
  unsigned char *mem;
};

/**************************************************************************/
  
ErrorHandler            MemoryErr;

static int              init     = TRUE;
static size_t           heapsize = HEAPDEFAULT;
static struct heapnode *cheap;
static struct List      heap;
static struct memstats  salloc   = { 0 , 0 , ULONG_MAX , 0 } ;
static struct memstats  sfree    = { 0 , 0 , ULONG_MAX , 0 } ;
static struct memstats  sgrow    = { 0 , 0 , ULONG_MAX , 0 } ;
static struct memstats  sshrink  = { 0 , 0 , ULONG_MAX , 0 } ;
static struct memstats  sresize  = { 0 , 0 , ULONG_MAX , 0 } ;
static struct memstats  sheap    = { 0 , 0 , 0         , 0 } ;
static struct memstats *stats[] = 
{ 
  &salloc, 
  &sfree, 
  &sgrow, 
  &sshrink,
  &sresize,
  &sheap
};

/**************************************************************************/

static void		*mem_malloc		(size_t);
static struct heapnode	*mem_heapadd		(void);
static int		 mem_errhandler		(Error);
#ifdef DDT
  static void		 mem_logdumpall		(void);
  static void		 mem_logdump		(char *,struct memstats *);
#endif

/*************************************************************************/

void MemInit(void)
{
  int i;
  
  if (init)
  {
    MemHeapInit(HEAPDEFAULT);
    for (i = 0 ; i < sizeof(stats) / sizeof(struct memstats *) ; i++)
    {
      stats[i]->bytes = 0;
      stats[i]->calls = 0;
      stats[i]->min   = ULONG_MAX;
      stats[i]->max   = 0;
    }
    init = FALSE;
    ErrHandlerNew(&MemoryErr);
    ErrHandlerPush(&MemoryErr,mem_errhandler);
  }
}

/***************************************************************************/

void MemDeInit(void)
{
  MemHeapFree();
  init = TRUE;
  D(mem_logdumpall();)
}

/*************************************************************************/

static int mem_errhandler(Error error)
{
  ddt(error != NULL);
  
  ddtlog(ddtstream,"no more memory");
  return(ABORT);
}

/*************************************************************************/

static void *mem_malloc(size_t size)
{
  void *p;
  int   rc;

  while(1)
  {
    p = malloc(size);
    if (p == NULL)
    {
      if (init == FALSE)
      {
        ErrorPush(CgiErr,ERR_MEMORY,ERR_NOMEMORY,"");
        rc = ErrThrow(&MemoryErr);
        if (rc == RETRY) continue;	/* anything else, blow up */
      }					/* this may be on purpose */
      else
      {
        ddtlog(ddtstream,"really hosed the system, eh?");
	exit(1);
      }
    }					/* this comment used be up there */
    break;
  }
  return(p);
}

/**********************************************************************/

static struct heapnode *mem_heapadd(void)
{  
  struct heapnode *phn;
  
  phn        = mem_malloc(sizeof(struct heapnode));
  phn->mem   = mem_malloc(heapsize);
  phn->size  = heapsize;
  phn->offs  = 0;
  cheap      = phn;
  D(memset(phn->mem,PADDING,heapsize);)
  ListAddTail(&heap,&phn->node);
  sheap.bytes = heapsize;
  sheap.calls ++;
  return(phn);
}

/*************************************************************************/
  
void MemHeapInit(size_t hintsize)
{
#if 1
  ddt(hintsize > 0);
  heapsize = hintsize;
  ListInit(&heap);
  mem_heapadd();
#endif
}

/*************************************************************************/

void MemHeapFree(void)
{
#if 1
  struct heapnode *phn;

  cheap = NULL;
    
  while(1)
  {
    phn = (struct heapnode *)ListRemHead(&heap);
    if (!NodeValid(&phn->node)) return;
    free(phn->mem);
    free(phn);
  }
#endif
}
 
/**************************************************************************/
 
void *MemAlloc(size_t size)
{
  void            *p;
  size_t           nsize;
  struct heapnode *phn;
    
  ddt(size > 0);
  if (size > (130L*1024L))
    ddt(0);
  ddt(size < (130L*1024L));

  nsize = size + FUDGE;
#if 1
  phn   = cheap;
  D(ddtlog(ddtstream,"cheap: %08lX",(unsigned long)phn);)
  
  if ((phn->offs + nsize + ((nsize / sizeof(ALIGN)) + 1) * sizeof(ALIGN)) > phn->size)
  /*if ((nsize + phn->offs) > phn->size)*/
    phn = mem_heapadd();
  
  ddt(nsize < phn->size);
  
  p          = &phn->mem[phn->offs];
  phn->offs += ((nsize / sizeof(ALIGN)) + 1) * sizeof(ALIGN);
  ddt(phn->offs < phn->size);
#else
  p = mem_malloc(nsize);
#endif

  salloc.bytes += size;
  salloc.calls ++;
  if (size < salloc.min) salloc.min = size;
  if (size > salloc.max) salloc.max = size;
  D(memset(p,PADDING,size+FUDGE);)
  return((void *)((unsigned char *)(p) + HFUDGE));
}

/*************************************************************************/

void *MemDDTAlloc(size_t size,char *file,int line)
{
  void *p;
  
  p = MemAlloc(size);
  ddtlog(
          ddtstream,
          "MEM: %p %lu %s(%d)",
          (void *)((unsigned char *)(p)),
          (unsigned long)size,
          file,
          line
        );  
  return(p);
}

/***********************************************************************/

void *MemGrow(void *p,size_t osize,size_t nsize)
{
  void *np;
  
  ddt(p     != NULL);
  ddt(osize >  0);
  ddt(nsize >  0);
  ddt(nsize >  osize);
  
  np = MemAlloc(nsize);
  DDTPTR(void *,np);
  DDTPTR(void *,p);
  memcpy(np,p,osize);
  MemFree(p,osize);
  return(np);
}

/************************************************************************/

void *MemShrink(void *p,size_t osize,size_t nsize)
{
  void *np;
  
  ddt(p     != NULL);
  ddt(osize >  0);
  ddt(nsize >  0);
  ddt(nsize <  osize);
  
  np = MemAlloc(nsize);
  DDTPTR(void *,np);
  DDTPTR(void *,p);
  memcpy(np,p,nsize);
  MemFree(p,osize);
  return(np);
}

/************************************************************************/

void *MemResize(void *p,size_t osize,size_t nsize)
{
  if (p == NULL)	/* allocate the memory */
  {
    return(MemAlloc(nsize));
  }
  else if (nsize == 0)	/* free the memory */
  {
    MemFree(p,osize);
    return(NULL);
  }
  else if (nsize > osize)	/* grow the memory */
  {
    return(MemGrow(p,osize,nsize));
  }
  else if (nsize < osize)	/* shrink the memory */
  {
    return(MemShrink(p,osize,nsize));
  }
  else				/* no-op */
  {
    D(ddt(0);)
    return(p);
  }
}

/*********************************************************************/

void MemFree(void *p,size_t s)
{
# ifdef DDT
    unsigned char *pu;
    int            i;
# endif

  ddt(p != NULL);
  ddt(s >  0);

  sfree.bytes += s;
  sfree.calls ++;
  if (s < sfree.min) sfree.min = s;
  if (s > sfree.max) sfree.max = s;

# ifdef DDT

    for ( 
          pu = (unsigned char *)(p) - HFUDGE , i = 0 ;
          i < HFUDGE ;
          pu++ , i++
        )
    {
      if (*pu != PADDING)
      {
        ddtlog(ddtstream,"%p front mem guard corrupt at %d size %lu char %d",(void *)p,i,s,*pu);
        mem_logdumpall();
        ddt(*pu == PADDING);
      }
    }

    for (
          pu = (unsigned char *)(p) + s + HFUDGE - 1 , i = 0 ;
          i < HFUDGE ;
          pu-- , i++
        )
    {
      if (*pu != PADDING)
      {
        ddtlog(ddtstream,"%p back mem guard corrupt at %d size %lu char %d",(void *)p,i,s,*pu);
	mem_logdumpall();
        ddt(*pu == PADDING);
      }
    }

    pu = (unsigned char *)(p) - HFUDGE;	/* fill memory with garbage */
    memset(pu,PADDING,s+FUDGE);
# endif
}

/***********************************************************************/

void MemDDTFree(void *p,size_t size,char *file,int line)
{
  ddtlog(
          ddtstream,
          "FREE: %p %lu %s(%d)",
          (void *)((unsigned char *)(p)),
          (unsigned long)size,
          file,
          line
        );
  MemFree(p,size);
}

/***********************************************************************/

void MemStatus(unsigned long *data)
{
  ddt(data != NULL);

  data[0] = salloc.bytes;
  data[1] = salloc.calls;
  data[2] = salloc.min;
  data[3] = salloc.max;
  data[4] = sfree.bytes;
  data[5] = sfree.calls;
  data[6] = sfree.min;
  data[7] = sfree.max;
}

/************************************************************************/

int MemStatistics(struct memstats *pstats,size_t num,int calls)
{
  int i;
  int mask;
  
  ddt(pstats          != NULL);
  ddt(num             >  0);
  ddt(num             <= 5);
  ddt((calls & C_MAX) == 0);
    
  for (mask = 0 , i = 0 ; (mask < 5) && (i < num) ; mask++)
  {
    if ((calls & (1 << mask)) == (1 << mask)) pstats[i++] = *stats[mask];
  }
  return(i);
}

/*************************************************************************/

#ifdef DDT
  
  static void mem_logdumpall(void)
  {
    mem_logdump("alloc", &salloc);
    mem_logdump("free",  &sfree);
    mem_logdump("grow",  &sgrow);
    mem_logdump("shrink",&sshrink);
    mem_logdump("resize",&sresize);
    mem_logdump("heap",  &sheap);
  }
  
  /***********************************************************************/
  
  static void mem_logdump(char *msg,struct memstats *ps)
  {
    ddt(msg != NULL);
    ddt(ps  != NULL);
    
    ddtlog(ddtstream,"%s\t: %lu bytes across %lu calls",msg,ps->bytes,ps->calls);
    ddtlog(ddtstream,"\t: %lu min %lu max",ps->min,ps->max);
  }
  
#endif

/**************************************************************************/

#if 0

  void *memcpy(void *dest,const void *src,size_t n)
  {
    char       *d;
    const char *s;
    
    if (n == 0)
    {
      ddtlog(ddtstream,"MEM: moving 0 bytes.  Ah ... ");
      return(dest);
    }

    if ((dest == NULL) || (dest == (void *)0xCCCCCCCC))
    {
      ddt(0);
      exit(1);
    }

    if ((src == NULL) || (src == (void *)0xCCCCCCCC))
    {
      memset(dest,PADDING,n);
      return(dest);
    }
    
    d = dest;
    s = src;
    
    for ( ; n ; *d++ = *s++ , n-- )
      ;
    return(dest);
  }
  
#endif

