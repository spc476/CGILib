
/**************************************************************************
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
**************************************************************************/

#include "../ddt.h"
#include "../memory.h"
#include "../errors.h"

#include "../stream.h"

static int	memory_read	(struct stream *s,struct streamvector *v);
static int	memory_unread	(struct stream *s,struct streamvector *v,int);
static int	memory_write	(struct stream *s,struct streamvector *v,int);

/************************************************************************/

Stream (MemoryStreamRead)(void *data,size_t size)
{
  Stream s;
  
  ddt(data != NULL);
  ddt(size >  0);
  
  s                  = StreamNewRead();
  s->calls.readwrite = memory_read;
  s->calls.unrw      = memory_unread;
  s->data            = data;
  s->size            = size;
  s->eof             = FALSE;
  return(s);
}

/***********************************************************************/

Stream (MemoryStreamWrite)(void *data,size_t size)
{
  Stream s;
  
  ddt(data != NULL);
  ddt(size >  0);
  
  s = StreamNewWrite();
  s->calls.readwrite = memory_write;
  s->data            = data;
  s->size            = size;
  s->eof             = FALSE;
  return(s);
}

/***********************************************************************/

static int memory_read(struct stream *s,struct streamvector *v)
{
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(s->type == MODE_READ);
  
  while(1)
  {
    if (s->eof) return(EOF);
    if (s->off == s->size)
    {
      if (StreamRefill(s))
        continue;
      else
      {
        s->eof = TRUE;
        return(EOF);
      }
    }
    return(s->data[s->off++]);
  }
}

/***********************************************************************/

static int memory_unread(struct stream *s,struct streamvector *v,int c)
{
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(s->type == MODE_READ);
  
  if (c == EOF)
    s->eof = TRUE;
  else
  {
    if (s->off) s->off--;
    s->data[s->off] = c;
    s->eof          = FALSE;
  }
  return(c);
}

/************************************************************************/

static int memory_write(struct stream *s,struct streamvector *v,int c)
{
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(s->type == MODE_WRITE);

  while(1)
  { 
    if (s->eof) return(EOF);
    if (s->off == s->size)
    {
      if (StreamFlush(s))
        continue;
      else
      {
        s->eof = TRUE;
        return(EOF);
      }
    }
    if (c == EOF)
      s->eof = TRUE;
    else
      s->data[s->off++] = c;
    return(c);
  }
}

/**********************************************************************/

