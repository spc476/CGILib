
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

#include <string.h>
#include <ctype.h>

#include "nodelist.h"
#include "memory.h"
#include "stream.h"
#include "util.h"
#include "rfc822.h"
#include "pair.h"
#include "ddt.h"

#define STRING_DELTA	1024

/***************************************************************************/

char *(RFC822LineRead)(const Stream in)
{
  size_t  tbuffsize = 0;
  size_t  size      = 0;
  char   *tbuff     = NULL;
  char   *ret;
  int     c;
  
  ddt(in != NULL);
  
  while(!StreamEOF(in))
  {
    if (size == tbuffsize)
    {
      tbuffsize += STRING_DELTA;
      tbuff      = MemResize(tbuff,size,tbuffsize);
    }
    
    c = Line_ReadChar(in);
    if (c == '\n')
    {
      c = StreamRead(in);
      if ((c == '\n') || !isspace(c))
      {
        StreamUnRead(in,c);
        break;
      }
    }    
    tbuff[size++] = c;
  }
  
  tbuff[size] = '\0';
  ret         = (!empty_string(tbuff)) ? dup_string(tbuff) : NULL ;
    
  MemFree(tbuff,tbuffsize);
  return(ret);
}

/***************************************************************************/

void (RFC822HeadersRead)(const Stream in,const List *list)
{
  char        *line;
  char        *t;
  struct pair *ppair;
  
  ddt(in   != NULL);
  ddt(list != NULL);
  
  while((line = RFC822LineRead(in)) != NULL)
  {
    t            = line;
    ppair        = PairNew(&t,':','\0');
    ppair->name  = trim_space(ppair->name);
    ppair->value = trim_space(ppair->value);
    up_string(ppair->name);
    ListAddTail((List *)list,&ppair->node);
    MemFree(line,strlen(line) + 1);
  }  
}

/*************************************************************************/

size_t (RFC822HeadersWrite)(const Stream out,const List *list)
{
  struct pair  *ppair;
  int         (*conv)(int);
  size_t        size;
  char         *name;
  
  ddt(out  != NULL);
  ddt(list != NULL);
  
  for
  (
    size = 0 , ppair = (struct pair *)ListGetHead((List *)list);
    NodeValid(&ppair->node);
    ppair = (struct pair *)NodeNext(&ppair->node)
  )
  {
    name = dup_string(ppair->name);
    for (conv = (toupper) ; *name ; name++)
    {
      *name = (*conv)(*name);
      conv  = isalpha(*name) ? (tolower) : (toupper) ;
    }

    size += LineSFormat(out,"$ $","%a: %b\n",name,ppair->value);
    MemFree(name,strlen(name) + 1);
  }
  return(size);
}

/*************************************************************************/

