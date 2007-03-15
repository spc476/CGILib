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

#include "../memory.h"
#include "../ddt.h"
#include "../errors.h"
#include "../types.h"
#include "../stream.h"

/************************************************************************/

static int	null_read	(struct stream *,struct streamvector *);
static int	null_unread	(struct stream *,struct streamvector *,int);
static int	null_refill	(struct stream *,struct streamvector *);
static int	null_write	(struct stream *,struct streamvector *,int);
static int	null_unwrite	(struct stream *,struct streamvector *);
static int	null_flush	(struct stream *,struct streamvector *);
static int	null_close	(struct stream *,struct streamvector *);

/************************************************************************/

int (StreamInit)(void)
{
  StdinStream  = FHStreamRead(0);
  StdoutStream = FHStreamWrite(1);
  StderrStream = FHStreamWrite(2);

  return( 
  	  (StdinStream != NULL) 
  	  && (StdoutStream != NULL) 
  	  && (StderrStream != NULL)
  	);
}

/*************************************************************************/

Stream (StreamNewRead)(void)
{
  Stream s;
  
  s = MemAlloc(sizeof(struct stream));
  s->type              = MODE_READ;
  s->eof               = TRUE;
  s->data              = NULL;
  s->size              = 0;
  s->off               = 0;
  s->calls.user        = NULL;
  s->calls.readwrite   = null_read;
  s->calls.unrw        = null_unread;
  s->calls.refillflush = null_refill;
  s->calls.close       = null_close;
  ListInit(&s->mods);
  ListAddTail(&s->mods,&s->calls.node);
  
  return(s);
}

/**************************************************************************/

Stream (StreamNewWrite)(void)
{
  Stream s;
  
  s = MemAlloc(sizeof(struct stream));
  s->type              = MODE_WRITE;
  s->eof               = TRUE;
  s->data              = NULL;
  s->size              = 0;
  s->off               = 0;
  s->calls.user        = NULL;
  s->calls.readwrite   = null_write;
  s->calls.unrw        = null_unwrite;
  s->calls.refillflush = null_flush;
  s->calls.close       = null_close;
  ListInit(&s->mods);
  ListAddTail(&s->mods,&s->calls.node);
  
  return(s);
}

/************************************************************************/

int (StreamRead)(Stream s)
{
  ddt(s != NULL);
  
  return((*s->calls.readwrite)(s,&s->calls));
}

/*************************************************************************/

int (StreamUnRead)(Stream s,int c)
{
  ddt(s != NULL);
  
  return ((*s->calls.unrw)(s,&s->calls,c));
}

/************************************************************************/

int (StreamRefill)(Stream s)
{
  ddt(s != NULL);
  
  return ((*s->calls.refillflush)(s,&s->calls));
}

/************************************************************************/

int (StreamWrite)(Stream s,int c)
{
  ddt(s != NULL);
  return((*s->calls.readwrite)(s,&s->calls,c));
}

/*************************************************************************/

int (StreamUnWrite)(Stream s)
{
  ddt(s != NULL);
  return((*s->calls.unrw)(s,&s->calls));
}

/**************************************************************************/

int (StreamFlush)(Stream s)
{
  ddt(s != NULL);
  return((*s->calls.refillflush)(s,&s->calls));
}

/**************************************************************************/

int (StreamEOF)(Stream s)
{
  ddt(s != NULL);
  return(s->eof);
}

/*************************************************************************/

size_t (StreamCopy)(Stream dest,Stream src)
{
  ddt(dest != NULL);
  ddt(src  != NULL);

  return (StreamCopyN(dest,src,SIZET_MAX));
}

/**********************************************************************/

size_t (StreamCopyN)(Stream dest,Stream src,size_t size)
{
  size_t amount = 0;
  int    c;

  ddt(dest != NULL);
  ddt(src  != NULL);
  ddt(size >  0);
  
  while(size-- && !StreamEOF(src))
  {
    c = StreamRead(src);
    if (c == IEOF) break;
    StreamWrite(dest,c);
    amount++;
  }
  return(amount);
}

/**************************************************************************/
  
int (StreamFree)(Stream s)
{
  struct streamvector *pv;
  
  ddt(s != NULL);
  
  if (s->type == MODE_WRITE)
    StreamFlush(s);
  
  for 
  (
    pv = (struct streamvector *)ListRemHead(&s->mods);
    NodeValid(&pv->node);
    pv = (struct streamvector *)ListRemHead(&s->mods)
  )
  {
    (*pv->close)(s,pv);
    if (pv != &s->calls)
      MemFree(pv);
  }
  MemFree(s);
  return(ERR_OKAY);
}

/**************************************************************************/

struct streamvector *(StreamVectorClone)(Stream s)
{
  struct streamvector *svec;
  
  ddt(s != NULL);
  
  svec              = MemAlloc(sizeof(struct streamvector));
  svec->user        = s->calls.user;
  svec->readwrite   = s->calls.readwrite;
  svec->unrw        = s->calls.unrw;
  svec->refillflush = s->calls.refillflush;
  svec->close       = s->calls.close;

  return(svec);
}

/*************************************************************************/

static int null_read(struct stream *s,struct streamvector *v)
{
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(s->type == MODE_READ);
  
  return(EOF);
}

/*************************************************************************/

static int null_unread(struct stream *s,struct streamvector *v,int c)
{
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(s->type == MODE_READ);
  
  return(EOF);
}

/*************************************************************************/

static int null_refill(struct stream *s,struct streamvector *v)
{
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(s->type == MODE_READ);
  
  return(FALSE);
}

/*************************************************************************/

static int null_write(struct stream *s,struct streamvector *v,int c)
{
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(s->type == MODE_WRITE);
  
  return(c);
}

/*************************************************************************/

static int null_unwrite(struct stream *s,struct streamvector *v)
{
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(s->type == MODE_WRITE);
  
  return(EOF);
}

/*************************************************************************/
  
static int null_flush(struct stream *s,struct streamvector *v)
{
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(s->type == MODE_WRITE);
  
  return(FALSE);
}

/************************************************************************/

static int null_close(struct stream *s,struct streamvector *v)
{
  ddt(s != NULL);
  ddt(v != NULL);
  
  return(0);
}

/************************************************************************/

