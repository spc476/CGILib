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

#ifndef MEMORH_H
#define MEMORY_H

#include <stdlib.h>
#include "nodelist.h"
#include "errors.h"

/*********************************************************************/

typedef struct heap
{
  Node                    node;
  size_t                  size;
  const struct memvector *vector;
  void                   *data;
} *Heap;

struct memvector
{
  void   *(*hnew)   (size_t,void *);	/* interesting hack here */
  void    (*hfree)  (const struct heap *);
  void   *(*alloc)  (const struct heap *,size_t);
  void   *(*resize) (const struct heap *,void *,size_t);
  void    (*free)   (const struct heap *,void *);
};

/**********************************************************************/

extern Heap         HeapMain;

int		 (MemInit)		(void);
int		 (StandardCHeap)	(Heap *);
int		 (ApacheHeap)		(Heap *,void *);
int		 (SpcHeap)		(Heap *);

Heap		 (HeapDefault)		(const Heap);
int		 (HeapNew)		(Heap *,size_t,const struct memvector *,void *);
void		*(HeapMemAlloc)		(const Heap,size_t);
void		*(HeapMemResize)	(const Heap,void *,size_t);
void		 (HeapMemFree)		(const Heap,void *);
void		 (HeapFree)		(Heap *);

void		*(MemAlloc)		(size_t);
void		*(MemResize)		(void *,size_t);
void		 (MemFree)		(void *);

#ifdef SCREAM
#  define HeapMemAlloc(h,s)	(*(h)->vector->alloc)((h),(s))
#  define HeapMemResize(h,p,s)	(*(h)->vector->resize)((h),(p),(s))
#  define HeapMemFree(h,p)	(*(h)->vector->free)((h),(p))
#  define MemAlloc(s)		HeapMemAlloc(HeapMain,(s))
#  define MemResize(p,s)	HeapMemResize(HeapMain,(p),(s))
#  define MemFree(p)		HeapMemFree(HeapMain,(p))
#endif

/********************************************************************/

#endif

