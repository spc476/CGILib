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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "../chunk.h"

/**********************************************************************/

static void chunk_readcallback(FILE *in,char *obuff,size_t size)
{
  char *buff;
  
  assert(in    != NULL);
  assert(obuff != NULL);
  assert(size  >  0);
  
  buff  = obuff;
  *buff = '\0';
  
  while(size)
  {
    int c = fgetc(in);
    if (c == EOF) return;
    
    if (c == '}')
    {
      c = fgetc(in);
      if (c == EOF) return;
      if (c == '%')  return;
      *buff++ = '}';
    }
    
    *buff++ = tolower(c);
    *buff   = '\0';
    size--;
  }
  
  assert((size_t)(buff - obuff) < size);
  return;
}

/**************************************************************************/

static int chunk_search_cmp(void const *needle,void const *haystack)
{
  char const                  *key  = needle;
  struct chunk_callback const *hole = haystack;
  
  assert(needle   != NULL);
  assert(haystack != NULL);
  
  return strcmp(key,hole->name);
}

/*********************************************************************/

static void chunk_handle(
                          FILE                        *restrict in,
                          FILE                        *restrict out,
                          struct chunk_callback const *pcc,
                          size_t                       scc,
                          void                        *data
                        )
{
  struct chunk_callback const *res;
  char                         cmdbuf[BUFSIZ];
  
  
  assert(in    != NULL);
  assert(out   != NULL);
  assert(pcc   != NULL);
  assert(scc   >  0);
  
  memset(cmdbuf,0,sizeof(cmdbuf));
  chunk_readcallback(in,cmdbuf,BUFSIZ);
  
  res = bsearch(cmdbuf,pcc,scc,sizeof(struct chunk_callback),chunk_search_cmp);
  
  if (res)
    (*res->callback)(out,data);
  else
    fprintf(stderr,"%%{processing error - can't find '%s'}%%",cmdbuf);
}

/**************************************************************************/

int ChunkProcessStream(Chunk const chunk,FILE *in,FILE *out,void *data)
{
  assert(chunk != NULL);
  assert(in    != NULL);
  assert(out   != NULL);
  
  while(!feof(in))
  {
    int c = fgetc(in);
    if (c == EOF) break;
    if (c == '%')
    {
      c = fgetc(in);
      if (c == '{')
      {
        chunk_handle(in,out,chunk->cb,chunk->cbsize,data);
        continue;
      }
      fputc('%',out);
    }
    fputc(c,out);
  }
  return(0);
}

/**************************************************************************/
