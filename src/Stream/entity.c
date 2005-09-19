/************************************************************************
*
* Copyright 2005 by Sean Conner.  All Rights Reserved.
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

#include <stdlib.h>

#include "../types.h"
#include "../ddt.h"
#include "../stream.h"
#include "../memory.h"

/************************************************************************/

struct entmod
{
  Stream  in;
  char   *entity;
  int     amp;
};

static int	entity_read	(struct stream *,struct streamvector *);
static int	entity_unread	(struct stream *,struct streamvector *,int);
static int	entity_refill	(struct stream *,struct streamvector *);
static int	entity_close	(struct stream *,struct streamvector *);

/***********************************************************************/

Stream (EntityStreamRead)(Stream in,int convertamp)
{
  Stream         s;
  struct entmod *em;
  
  em         = MemAlloc(sizeof(struct entmod));
  em->in     = in;
  em->entity = NULL;
  em->amp    = convertamp;
  
  s = StreamNewRead();
  s->calls.readwrite   = entity_read;
  s->calls.refillflush = entity_refill;
  s->calls.unrw        = entity_unread;
  s->calls.close       = entity_close;
  s->calls.user        = em;
  s->eof               = FALSE;
  
  return(s);
}

/************************************************************************/

static int entity_read(struct stream *s,struct streamvector *v)
{
  struct entmod *em;
  int            c;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  em = v->user;
  while(1)
  {
    if (em->entity)
    {
      c = *(em->entity++);
      if (c) return(c);
      em->entity = NULL;
    }
  
    c  = StreamRead(em->in);
  
    switch(c)
    {
      case IEOF: s->eof = TRUE; return(c);
      case '>': em->entity = "&gt;"; continue;
      case '<': em->entity = "&lt;"; continue;
      case '"': em->entity = "&quot;"; continue;
      case '&':
           if (em->amp == ENTITY_NOAMP) return(c);
           em->entity = "&amp;"; continue;
      default: return(c);
    }
  }
  ddt(0);
  return(IEOF);
}
    
/************************************************************************/

static int entity_refill(struct stream *s,struct streamvector *v)
{
  struct entmod *em;

  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);

  em = v->user;
  return(StreamFlush(em->in));
}

/************************************************************************/

static int entity_unread(struct stream *s,struct streamvector *v,int c)
{
  struct entmod *em;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  em = v->user;
  return(StreamUnRead(em->in,c));
}

/*************************************************************************/

static int entity_close(struct stream *s,struct streamvector *v)
{
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  MemFree(v->user);
  return(0);
}

/***************************************************************************/

