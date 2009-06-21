/************************************************************************
*
* Copyright 2009 by Sean Conner.  All Rights Reserved.
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

#define _GNU_SOURCE 1

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "nodelist.h"
#include "util.h"
#include "rfc822.h"
#include "pair.h"

#define STRING_DELTA	1024

/***************************************************************************/

char *(RFC822LineRead)(FILE *in)
{
  char   *line = NULL;
  size_t  max  = 0;
  size_t  idx  = 0;
  int     c;
  
  assert(in != NULL);
  
  while((c = fgetc(in)) != EOF)
  {
    if (c == '\n')
    {
      c = fgetc(in);
      if ((c == EOF) || (c == '\n') || !isspace(c))
        ungetc(c,in);
      else
        c = ' ';
    }
    
    if (idx == max)
    {
      max += 80;
      line = realloc(line,max);
    }
    line[idx++] = c;
    
    if (c == '\n') break;
  }
  
  if ((line != NULL) && (*line == '\n'))
  {
    free(line);
    line = NULL;
  }
  
  return line;
}

/***************************************************************************/

void (RFC822HeadersRead)(FILE *in,const List *list)
{
  char        *line;
  char        *t;
  struct pair *ppair;
  
  assert(in   != NULL);
  assert(list != NULL);
  
  while((line = RFC822LineRead(in)) != NULL)
  {
    t            = line;
    ppair        = PairNew(&t,':','\0');
    ppair->name  = trim_space(ppair->name);
    ppair->value = trim_space(ppair->value);
    up_string(ppair->name);
    ListAddTail((List *)list,&ppair->node);
    free(line);
  }  
}

/*************************************************************************/

size_t (RFC822HeadersWrite)(FILE *out,const List *list)
{
  struct pair *ppair;
  size_t       size;

  assert(out  != NULL);
  assert(list != NULL);
  
  for
  (
    size = 0 , ppair = (struct pair *)ListGetHead((List *)list);
    NodeValid(&ppair->node);
    ppair = (struct pair *)NodeNext(&ppair->node)
  )
  {
    size += RFC822HeaderWrite(out,ppair->name,ppair->value);
  }
  return(size);
}

/*************************************************************************/

size_t (RFC822HeaderWrite)(FILE *out,const char *name,const char *value)
{
  int        (*conv)(int);
  size_t       size;
  char         n[strlen(name) + 1];
  const char  *s;
  char        *d;
  
  assert(out   != NULL);
  assert(name  != NULL);
  assert(value != NULL);
  
  for (conv = (toupper) , s = name , d = n ; ; s++ , d++)
  {
    *d   = (*conv)(*s);
    conv = isalpha(*d) ? (tolower) : (toupper);
    if (*d == '\0') break;
  }
  
  size = fprintf(out,"%s: %s\n",n,value);
  return(size);
}

/***********************************************************************/

