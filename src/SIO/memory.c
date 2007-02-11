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
#include "../types.h"
#include "../sio.h"

/***********************************************************************/

static int	memory_rread	(struct sior *);
static int	memory_runread	(struct sior *,int);

static int	memory_wwrite	(struct siow *,int);
static int	memory_wunwrite	(struct siow *);

/************************************************************************/

SIOR (SIORMemory)(void *data,size_t size)
{
  SIOR r;
  
  ddt(data != NULL);
  ddt(size >  0);
  
  r         = SIORNew(sizeof(struct sior));
  r->type   = SIOT_MEMORY;
  r->size   = size;
  r->off    = 0;
  r->eof    = FALSE;
  r->read   = memory_rread;
  r->unread = memory_runread;
  r->data   = data;
  
  return (r);
}

/************************************************************************/

SIOW (SIOWMemory)(void *data,size_t size)
{
  SIOW w;
  
  ddt(data != NULL);
  ddt(size >  0);
  
  w          = SIOWNew(sizeof(struct siow));
  w->type    = SIOT_MEMORY;
  w->size    = size;
  w->off     = 0;
  w->eof     = FALSE;
  w->write   = memory_wwrite;
  w->unwrite = memory_wunwrite;
  w->data    = data;
  
  return(w);
}

/***********************************************************************/

static int memory_rread(struct sior *r)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);
  
  while(1)
  {
    if (r->eof) return (IEOF);
    if (r->off == r->size)
    {
      if (SIORRefill(r))
        continue;
      else
      {
        r->eof = TRUE;
        return(IEOF);
      }
    }
    return(r->data[r->off++]);
  }
}

/***********************************************************************/

static int memory_runread(struct sior *r,int c)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);
  
  if (c == EOF)
    r->eof = TRUE;
  else
  {
    if (r->off) r->off--;
    r->data[r->off] = c;
    r->eof          = FALSE;
  }
  return(c);
}

/************************************************************************/

static int memory_wwrite(struct siow *w,int c)
{
  ddt(w
  ddt(w->dir == SIO_WRITE);

  while(1)
  { 
    if (w->eof) return(IEOF);
    if (w->off == w->size)
    {
      if (SIOWFlush(w))
        continue;
      else
      {
        w->eof = TRUE;
        return(IEOF);
      }
    }

    if (c == IEOF)
    {
      /*---------------------------------------------
      ; a couple of options here ... one is to ignore
      ; any IEOF that is sent here (which is what I'm
      ; leaning towards) but really, I need to figure
      ; out why StreamEOF() usually fails.  It might
      ; have something to do with Unix semantics or
      ; something silly like that.
      ;---------------------------------------------*/
      
      /*ddt(0);*/
      /*w->eof = TRUE;*/
    }
    else
      w->data[w->off++] = c; 
    return(c);
  }
}

/**********************************************************************/

