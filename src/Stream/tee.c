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

#include "../ddt.h"
#include "../memory.h"
#include "../stream.h"

/*************************************************************************/

struct teemod
{
  Stream a;
  Stream b;
};

/************************************************************************/

static int	tee_write	(struct stream *,struct streamvector *v,int);
static int	tee_flush	(struct stream *,struct streamvector *v);
static int	tee_close	(struct stream *,struct streamvector *v);

/************************************************************************/

Stream TeeStreamWrite(Stream a,Stream b)
{
  Stream s;
  struct teemod *tm;
  
  ddt(a != NULL);
  ddt(b != NULL);
  
  tm    = MemAlloc(sizeof(struct teemod));
  tm->a = a;
  tm->b = b;
  
  s                    = StreamNewRead();
  s->calls.readwrite   = tee_write;
  s->calls.refillflush = tee_flush;
  s->calls.close       = tee_close;
  s->calls.user        = tm;
  return(s);
}

/*************************************************************************/

static int tee_write(struct stream *s,struct streamvector *v,int c)
{
  struct teemod *tm;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  tm = v->user;
  StreamWrite(tm->a,c);
  StreamWrite(tm->b,c);
  return(c);
}

/*************************************************************************/

static int tee_flush(struct stream *s,struct streamvector *v)
{
  struct teemod *tm;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  tm = v->user;
  StreamFlush(tm->a);
  StreamFlush(tm->b);
  return(0);
}

/**************************************************************************/

static int tee_close(struct stream *s,struct streamvector *v)
{
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  MemFree(v->user);
  return(0);
}

/***************************************************************************/

