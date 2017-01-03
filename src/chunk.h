/***************************************************************************
*
* Copyright 2001,2013 by Sean Conner.
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

#ifndef I_8EF18997_15E3_5BD7_BEBB_31E9B26BA661
#define I_8EF18997_15E3_5BD7_BEBB_31E9B26BA661

#include <stdio.h>

/*********************************************************************/

struct chunk_callback
{
  const char *const name;
  void (*callback)(FILE *,void *);
};

typedef struct chunk
{
  char                        *name;
  const struct chunk_callback *cb;
  size_t                       cbsize;
} *Chunk;

/*********************************************************************/

extern Chunk ChunkNew           (const char *,const struct chunk_callback *,size_t);
extern int   ChunkProcess       (const Chunk,const char *,FILE *,void *);
extern int   ChunkProcessStream (const Chunk,FILE *restrict,FILE *restrict,void *);
extern int   ChunkFree          (Chunk);

#endif

