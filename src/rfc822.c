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
      tbuff      = MemResize(tbuff,tbuffsize);
    }
    
    c = Line_ReadChar(in);
    if (c == '\n')
    {
      c = Line_ReadChar(in);
      
      /*---------------------------------------
      ; if it's another line feed or a non-space
      ; character, we're done reading this line
      ;----------------------------------------*/
      
      if ((c == '\n') || !isspace(c))
      {
        StreamUnRead(in,c);
        break;
      }

      /* ----------------------------------
      ; discard white space
      ;-----------------------------------*/

      /*---------------------------------------------
      ; BUG is here---basically, if we hit a section of
      ; a header formatted like:
      ;
      ;		Random-J-Header: blah blah blah
      ;		_
      ;		body of text ...
      ;
      ; (that is, the blank line after the headers contains just
      ; white space, which, being white, is hard to see) then the lines
      ; in the following body are sucked up as part of the Random-J-Header
      ; line, which can cause problems later down the line.
      ;
      ; Need to fix this.
      ;------------------------------------------------/
      
      while(isspace(c))
        c = Line_ReadChar(in);

      /*-----------------------------------
      ; save the first-nonspace character,
      ; but continue with at least one space
      ; character---in effect, turning the '\n'
      ; into a space character.
      ;-------------------------------------*/
      
      StreamUnRead(in,c);
      c = ' ';
    }    
    tbuff[size++] = c;
  }

  if (tbuff == NULL)
    return(NULL);
  
  tbuff[size] = '\0';
  ret         = (!empty_string(tbuff)) ? dup_string(tbuff) : NULL ;
    
  MemFree(tbuff);
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
    MemFree(line);
  }  
}

/*************************************************************************/

size_t (RFC822HeadersWrite)(const Stream out,const List *list)
{
  struct pair  *ppair;
  int         (*conv)(int);
  size_t        size;
  char         *name;
  char         *t;
  
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
    for (conv = (toupper) , t = name ; *t ; t++)
    {
      *t = (*conv)(*t);
      conv  = isalpha(*t) ? (tolower) : (toupper) ;
    }

    size += LineSFormat(out,"$ $","%a: %b\n",name,ppair->value);
    MemFree(name);
  }
  return(size);
}

/*************************************************************************/

