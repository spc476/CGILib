
/*************************************************************************
*
* Copyright 2004 by Sean Conner.  All Rights Reserved.
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
************************************************************************/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../memory.h"
#include "../errors.h"
#include "../buffer.h"
#include "../util.h"
#include "../rawfmt.h"
#include "../ddt.h"

#define MAX_ENTITY	15

struct entityout_buffer
{
  struct buffer buf;
  Buffer        orig;
  int           do_amp;
  unsigned long idx;
  char          entity[MAX_ENTITY];
};

/***********************************************************************/

static int	entityout_ioreq		(struct buffer *,int,va_list);
static int	entityout_writechar	(struct entityout_buffer *,int);
static int	entityout_writestring	(struct entityout_buffer *,char *,size_t *);

/************************************************************************/

int EntityOutBuffer(Buffer *pbuf,Buffer orig)
{
  struct entityout_buffer *buf;
  
  ddt(pbuf != NULL);
  ddt(orig != NULL);
  
  buf            = MemAlloc(sizeof(struct entityout_buffer));
  buf->buf.size  = MAX_ENTITY;
  buf->buf.tag   = ENTITYOUTBUFFER;
  buf->buf.ioreq = entityout_ioreq;
  buf->orig      = orig;
  buf->do_amp    = TRUE;
  buf->idx       = 0;
  *pbuf          = (Buffer)buf;
  return(ERR_OKAY);
}

/************************************************************************/

static int entityout_ioreq(struct buffer *buf,int cmd,va_list list)
{
  struct entityout_buffer *pbuf;
  char                    *pd;
  size_t                  *ps;
  int                      c;
  
  ddt(buf != NULL);
  
  pbuf = (struct entityout_buffer *)buf;
  
  switch(cmd)
  {
    case CE_NOAMP:	/* do not convert & */
         pbuf->do_amp = FALSE;
	 return(ERR_OKAY);
    case C_FREE:
         MemFree(pbuf,sizeof(struct entityout_buffer));
         return(ERR_OKAY);
    case C_WRITE:
         pd = va_arg(list,char *);
         ps = va_arg(list,size_t *);
         return(entityout_writestring(pbuf,pd,ps));
    case CL_WRITEC:
         c  = va_arg(list,int);
         return(entityout_writechar(pbuf,c));
    default:
         return((*pbuf->orig->ioreq)(pbuf->orig,cmd,list));
  }
  ddt(0);
}

/***************************************************************************/

static int entityout_writechar(struct entityout_buffer *pbuf,int c)
{
  size_t size;
  char   cc;
  
  ddt(pbuf != NULL);
  
  switch(c)
  {
    case '<':
         size = 4;
         BufferWrite(pbuf->orig,"&lt;",&size);
         break;
    case '>':
         size = 4;
         BufferWrite(pbuf->orig,"&gt;",&size);
         break;
    case '&':
         if (pbuf->do_amp)
	 {
           size = 5;
           BufferWrite(pbuf->orig,"&amp;",&size);
	 }
	 else
	 {
	   size = 1;
	   cc   = c;
	   BufferWrite(pbuf->orig,&cc,&size);
	 }
         break;
    case '"':
         size = 6;
         BufferWrite(pbuf->orig,"&quot;",&size);
         break;
    default:
         size = 1;
         cc   = c;
         BufferWrite(pbuf->orig,&cc,&size);
         break;
  }
  return(ERR_OKAY);
}

/*********************************************************************/

static int entityout_writestring(
		struct entityout_buffer *pbuf,
		char                    *s,
		size_t                  *ps
	)
{
  ddt(pbuf != NULL);
  ddt(s    != NULL);
  ddt(ps   != NULL);
  ddt(*ps  >  0);
  
  while(*ps)
  {
    entityout_writechar(pbuf,*s);
    (*ps)--;
    s++;
  }
  
  return(ERR_OKAY);
}

/********************************************************************/

