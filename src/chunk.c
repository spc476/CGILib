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

#define _GNU_SOURCE 1

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "chunk.h"

/**********************************************************************/

static void chunk_readcallback(FILE *in,char *obuff,size_t size)
{
  char *buff;
  int   c;
  
  assert(in    != NULL);
  assert(obuff != NULL);
  assert(size  >  0);
  
  buff  = obuff;
  *buff = '\0';
  
  while(size)
  {
    c = fgetc(in);
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

static int chunk_search_cmp(const void *needle,const void *haystack)
{
  const char                  *key  = needle;
  const struct chunk_callback *hole = haystack;
  
  assert(needle   != NULL);
  assert(haystack != NULL);
  
  return strcmp(key,hole->name);
}

/*********************************************************************/

static void chunk_handle(
                          FILE                  *restrict in,
                          FILE                  *restrict out,
                          const struct chunk_callback *pcc,
                          size_t                 scc,
                          void                  *data
                        )
{
  const struct chunk_callback *res;
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

/***********************************************************************/

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

/*********************************************************************/

int ChunkProcessStream(const Chunk chunk,FILE *in,FILE *out,void *data)
{
  int c;
  
  assert(chunk != NULL);
  assert(in    != NULL);
  assert(out   != NULL);
  
  while(!feof(in))
  {
    c = fgetc(in);
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

/***********************************************************************/

int ChunkFree(Chunk chunk)
{
  assert(chunk != NULL);
  
  free(chunk->name);
  free(chunk);
  return(0);
}

/**********************************************************************/

