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

#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <assert.h>

#include "../chunk.h"

int ChunkProcess(const Chunk chunk,const char *name,FILE *out,void *data)
{
  char   fname[FILENAME_MAX];
  FILE  *in;
  
  assert(chunk != NULL);
  assert(name  != NULL);
  assert(out   != NULL);
  
  sprintf(fname,"%s/%s",chunk->name,name);
  
  in = fopen(fname,"r");
  if (in == NULL)
    return(errno);
    
  ChunkProcessStream(chunk,in,out,data);
  
  fclose(in);
  return(0);
}
