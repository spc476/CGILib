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

#define _GNU_SOURCE

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../chunk.h"

Chunk ChunkNew(const char *cname,const struct chunk_callback *pcc,size_t scc)
{
  Chunk chunk;
  
  assert(cname != NULL);
  assert(pcc   != NULL);
  assert(scc   >  0);
  
  chunk         = malloc(sizeof(struct chunk));
  chunk->name   = strdup(cname);
  chunk->cb     = pcc;
  chunk->cbsize = scc;
  
  return chunk;
}
