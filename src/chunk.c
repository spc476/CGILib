/*********************************************
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
**********************************************/

#define _GNU_SOURCE 1

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "chunk.h"
#include "errors.h"

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

static void chunk_docallback(
                              FILE                  *out,
                              char                  *cmd,
                              const struct chunk_callback *pcc,
                              size_t                 scc,
                              void                  *data
                            )
{
  int i;

  assert(out   != NULL);
  assert(cmd   != NULL);
  assert(pcc   != NULL);
  assert(scc   >  0);
  
  for (i = 0 ; i < scc ; i++)
  {
    if (strcmp(cmd,pcc[i].name) == 0)
    {
      (*pcc[i].callback)(out,data);
      return;
    }
  }

  fprintf(stderr,"%%{processing error - can't find '%s'}%%",cmd);
}

/************************************************************************/

static void chunk_handle(
			  FILE                  *in,
			  FILE                  *out,
                          const struct chunk_callback *pcc,
                          size_t                 scc,
                          void                  *data
                        )
{
  char  cmdbuf[BUFSIZ];
  char *cmd;
  char *p;
  
  assert(in    != NULL);
  assert(out   != NULL);
  assert(pcc   != NULL);
  assert(scc   >  0);
  
  chunk_readcallback(in,cmdbuf,BUFSIZ);
  
  for (p = cmdbuf ; (cmd = strtok(p," \t\v\r\n")) != NULL ; p = NULL )
  {
    chunk_docallback(out,cmd,pcc,scc,data);
  }
}

/**************************************************************************/

Chunk (ChunkNew)(const char *cname,const struct chunk_callback *pcc,size_t scc)
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

int (ChunkProcess)(Chunk chunk,const char *name,FILE *out,void *data)
{
  char   fname[FILENAME_MAX];
  FILE  *in;

  assert(chunk != NULL);
  assert(name  != NULL);
  assert(out   != NULL);
    
  sprintf(fname,"%s/%s",chunk->name,name);
  
  in = fopen(fname,"r");
  if (in == NULL)
    return(ERR_ERR);

  ChunkProcessStream(chunk,in,out,data);
  
  fclose(in);
  return(ERR_OKAY);
}

/*********************************************************************/

int (ChunkProcessStream)(Chunk chunk,FILE *in,FILE *out,void *data)
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
  return(ERR_OKAY);
}

/***********************************************************************/

int (ChunkFree)(Chunk chunk)
{
  assert(chunk != NULL);
  
  free(chunk->name);
  free(chunk);
  return(ERR_OKAY);
}

/**********************************************************************/

