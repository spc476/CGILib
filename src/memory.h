
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

#ifndef MEMORY_H_INCLUDE
#define MEMORY_H_INCLUDE
#  include <stdlib.h>
#  include "errors.h"
#endif

/*********************************************************************/

#ifndef MEMORY_H_DEFINE
#define MEMORY_H_DEFINE

#  define STATALLOC	0
#  define STATFREE	1
#  define STATGROW	2
#  define STATSHRINK	3
#  define STATRESIZE	4
#  define STATMAX	5

#  define C_MEMALLOC	(1 << 0)
#  define C_MEMFREE	(1 << 1)
#  define C_MEMGROW	(1 << 2)
#  define C_MEMSHRINK	(1 << 3)
#  define C_MEMRESIZE	(1 << 4)
#  define C_MAX		~(C_MEMALLOC | C_MEMFREE | C_MEMGROW | C_MEMSHRINK | C_MEMRESIZE)

#  define MEMINIT		(ERR_MEM + 0)
#  define STANDARDCHEAP		(ERR_MEM + 1)
#  define APACHEHEAP		(ERR_MEM + 2)
#  define SPCHEAP		(ERR_MEM + 3)
#  define HEAPDEFAULT		(ERR_MEM + 10)
#  define HEAPNEW		(ERR_MEM + 11)
#  define HEAPSPACETOTAL	(ERR_MEM + 12)
#  define HEAPSPACEFREE		(ERR_MEM + 13)
#  define HEAPSPACEUSED		(ERR_MEM + 14)
#  define HEAPMEMALLOC		(ERR_MEM + 15)
#  define HEAPMEMGROW		(ERR_MEM + 16)
#  define HEAPMEMSHRINK		(ERR_MEM + 17)
#  define HEAPMEMRESIZE		(ERR_MEM + 18)
#  define HEAPMEMFREE		(ERR_MEM + 19)
#  define HEAPMEMSTATS		(ERR_MEM + 20)
#  define HEAPFREE		(ERR_MEM + 21)
#  define MEMALLOC		(ERR_MEM + 30)
#  define MEMGROW		(ERR_MEM + 31)
#  define MEMSHRINK		(ERR_MEM + 32)
#  define MEMRESIZE		(ERR_MEM + 33)
#  define MEMFREE		(ERR_MEM + 34)
#  define MEMSTATS		(ERR_MEM + 35)

#endif

/*******************************************************************/

#ifndef MEMORY_H_TYPES
#define MEMORY_H_TYPES

  struct memstats
  {
    unsigned long bytes;
    unsigned long calls;
    unsigned long min;
    unsigned long max;
  };

  typedef struct heap
  {
    Node                    node;
    size_t                  size;
    const struct memvector *vector;
    void                   *data;
    struct memstats         stats[5];
  } *Heap;

  struct memvector
  {
    void   *(*hnew)   (size_t,void *);	/* interesting hack here */
    size_t  (*htspace)(const struct heap *);
    size_t  (*hfspace)(const struct heap *);
    size_t  (*huspace)(const struct heap *);
    void    (*hfree)  (const struct heap *);
    void   *(*alloc)  (const struct heap *,size_t);
    void   *(*grow)   (const struct heap *,const void *,size_t,size_t);
    void   *(*shrink) (const struct heap *,const void *,size_t,size_t);
    void    (*free)   (const struct heap *,const void *,size_t);
  };

#endif

/**********************************************************************/

#ifndef MEMORY_H_API
#define MEMORY_H_API

  extern Heap         HeapMain;
  extern ErrorHandler MemoryErr;

  int		 (MemInit)		(void);
  int		 (StandardCHeap)	(Heap *);
  int		 (ApacheHeap)		(Heap *,void *);
  int		 (SpcHeap)		(Heap *);

  Heap		 (HeapDefault)		(const Heap);
  int		 (HeapNew)		(Heap *,size_t,const struct memvector *,void *);
  size_t	 (HeapSpaceTotal)	(const Heap);
  size_t	 (HeapSpaceFree)	(const Heap);
  size_t	 (HeapSpaceUsed)	(const Heap);
  void		*(HeapMemAlloc)		(const Heap,size_t);
  void		*(HeapMemGrow)		(const Heap,const void *,size_t,size_t);
  void		*(HeapMemShrink)	(const Heap,const void *,size_t,size_t);
  void		*(HeapMemResize)	(const Heap,const void *,size_t,size_t);
  void		 (HeapMemFree)		(const Heap,const void *,size_t);
  int		 (HeapMemStats)		(const Heap,struct memstats *,size_t,int);
  void		 (HeapFree)		(Heap *);

  void		*(MemAlloc)		(size_t);
  void		*(MemGrow)		(const void *,size_t,size_t);
  void		*(MemShrink)		(const void *,size_t,size_t);
  void		*(MemResize)		(const void *,size_t,size_t);
  void		 (MemFree)		(const void *,size_t);
  int		 (MemStats)		(struct memstats *,size_t,int);

#  ifdef SCREAM
#    ifdef NOSTATS
#      define HeapMemAlloc(h,s)		(*(h)->vector->alloc)((h),(s))
#      define HeapMemGrow(h,p,os,ns)	(*(h)->vector->grow)((h),(p),(os),(ns))
#      define HeapMemShrink(h,p,os,ns)	(*(h)->vector->shrink)((h),(p),(os),(ns))
#if 0
#      define HeapMemResize(h,p,os,ns)				\
		( (p) && (os) && (ns) )				\
		  ? (os) < (ns)					\
		    ? (*(h)->vector->grow)((h),(p),(os),(ns))	\
		    : (*(h)->vector->shrink)((h),(p),(os),(ns))	\
		  : ( ((p) == NULL) && (ns > 0))		\
		    ? (*(h)->vector->alloc)((h),(ns))		\
		    : ( (p) && (ns == 0))			\
		      ? (*(h)->vector->free)((h),(os))		\
		      : (p)
#endif
#      define HeapMemFree(h,p,s)	(*(h)->vector->free)((h),(p),(s))
#    endif
#if 0
#    define HeapSpaceTotal(h)	(*(h)->htspace)((h))
#    define HeapSpaceFree(h)	(*(h)->hfspace)((h))
#    define HeapSpaceUsed(h)	(*(h)->huspace)((h))
#endif
/*#    define HeapFree(ph)	(* (*(ph))->hfree)(*(ph))*/
#    define MemAlloc(s)		HeapMemAlloc(HeapMain,(s))
#    define MemGrow(p,os,ns)	HeapMemGrow(HeapMain,(p),(os),(ns))
#    define MemShrink(p,os,ns)	HeapMemShrink(HeapMain,(p),(os),(ns))
#    define MemResize(p,os,ns)	HeapMemResize(HeapMain,(p),(os),(ns))
#    define MemFree(p,s)	HeapMemFree(HeapMain,(p),(s))
#  endif

#endif

/********************************************************************/

