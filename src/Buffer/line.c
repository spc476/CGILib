
/*************************************************************************
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

#define READBACK	1024
#define LINEBUFSIZE	8192
#define FULLSIZE	(READBACK + LINEBUFSIZE)

struct line_buffer
{
  struct buffer  buf;
  Buffer         cbuf;
  unsigned long  idx;
  unsigned long  odx;
  unsigned long  max;
  size_t         size;
  int            eof;
  char           buffer[FULLSIZE];
  char          *eol;
};

/*****************************************************************/

static int	 line_ioreq	(struct buffer      *,int,va_list);
static int	 line_read	(struct line_buffer *,char *,size_t *);
static int	 line_readbuffed(struct line_buffer *,char *,size_t *);
static int	 line_read822	(struct line_buffer *,char *,size_t *);
static int	 line_readchar	(struct line_buffer *,char *);
static int	 line_flush	(struct line_buffer *);
static int	 line_write	(struct line_buffer *,char *,size_t *);
static int	 line_writechar	(struct line_buffer *,char);
static int	 line_unreadchar(struct line_buffer *,char);

/*********************************************************************/

int LineBuffer(Buffer *pbuf,Buffer bold)
{
  struct line_buffer *buf;

  ddt(pbuf != NULL);
  ddt(bold != NULL);
  
  buf            = MemAlloc(sizeof(struct line_buffer));
  buf->buf.size  = sizeof(struct line_buffer);
  buf->buf.tag   = LINEBUFFER;
  buf->buf.ioreq = line_ioreq;
  buf->cbuf      = bold;
  buf->idx       = 0;
  buf->odx       = 0;
  buf->max       = 0;
  buf->size      = 0;
  buf->eol       = dup_string("\n");
  buf->eof       = FALSE;
  *pbuf          = (Buffer)buf;
  return(ERR_OKAY);
}

/************************************************************************/

static int line_ioreq(struct buffer *buf,int cmd,va_list list)
{
  struct line_buffer *pbuf;
  size_t             *ps;
  char               *pd;
  int                 c;
  
  ddt(buf != NULL);
  
  pbuf = (struct line_buffer *)buf;
  
  switch(cmd)
  {
    case C_FREE:
         MemFree(pbuf->eol,strlen(pbuf->eol)+1);
         MemFree(pbuf,sizeof(struct line_buffer));
         return(ERR_OKAY);
    case CL_READBUFFED:
         pd = va_arg(list,char *);
         ps = va_arg(list,size_t *);
         return(line_readbuffed(pbuf,pd,ps));
    case CL_READ822:
         pd = va_arg(list,char *);
         ps = va_arg(list,size_t *);
         return(line_read822(pbuf,pd,ps));
    case CL_READCLINE:
    case C_READ:
         pd = va_arg(list,char *);
         ps = va_arg(list,size_t *);
	 return(line_read(pbuf,pd,ps));
    case CL_READC:
         pd = va_arg(list,char *);
         return(line_readchar(pbuf,pd));
    case CL_UNREADC:
         c = va_arg(list,int);
         return(line_unreadchar(pbuf,c));
    case CL_SETEOL:
         MemFree(pbuf->eol,strlen(pbuf->eol)+1);
         pd        = va_arg(list,char *);
         pbuf->eol = dup_string(pd);
         return(ERR_OKAY);
    case C_WRITE:
         pd = va_arg(list,char *);
         ps = va_arg(list,size_t *);
         return(line_write(pbuf,pd,ps));
    case CL_WRITEC:
         c = va_arg(list,int);
         return(line_writechar(pbuf,c));
    case C_FLUSH:
         return(line_flush(pbuf));
    case CD_READEOF:
    case CL_EOF:
         return (pbuf->eof);
    default:
         return((*pbuf->cbuf->ioreq)(buf,cmd,list));
  }
  ddt(0);
}

/*************************************************************************/

static int line_readbuffed(struct line_buffer *pbuf,char *pd,size_t *ps)
{
  size_t bufsize;
  size_t trans;

  ddt(pbuf != NULL);
  ddt(pd   != NULL);
  ddt(ps   != NULL);
  ddt(*ps  >  0);
    
  bufsize = pbuf->max - pbuf->idx;
  if (bufsize == 0)
  {
    *ps = 0;
    return(ERR_OKAY);
  }
    
  trans   = (*ps < bufsize) ? *ps : bufsize ;
  D(ddtlog(ddtstream,"L L L L L","m: %a i: %b b: %c s: %d t: %e",(unsigned long)pbuf->max,(unsigned long)pbuf->idx,(unsigned long)bufsize,(unsigned long)*ps,(unsigned long)trans);)
  ddt(trans > 0);
  memcpy(pd,&pbuf->buffer[pbuf->idx],trans);
  pbuf->idx += trans;
  *ps        = trans;
  return(ERR_OKAY);
}

/**************************************************************************/

static int line_read(struct line_buffer *pbuf,char *pd,size_t *ps)
{
  size_t size;
  char   c;
  
  ddt(pbuf != NULL);
  ddt(pd   != NULL);
  ddt(ps   != NULL);
  ddt(*ps  >  1);

  size = *ps - 1;
  while(size)
  {
    line_readchar(pbuf,&c);
    if (c == '\0') break;
    if (c == '\r')
    {
      line_readchar(pbuf,&c);
      if ((c == '\n') || (c == '\0')) break;
      line_unreadchar(pbuf,c);
      break;
    }
    if (c == '\n')
    {
      line_readchar(pbuf,&c);
      if ((c == '\r') || (c == '\0')) break;
      line_unreadchar(pbuf,c);
      break;
    }
    *pd++ = c;
    size--;
  }
  *pd = '\0';
  *ps = *ps - size - 1;
  return(ERR_OKAY);
}

/***********************************************************************/

static int line_read822(struct line_buffer *pbuf,char *pd,size_t *ps)
{
  size_t size;
  size_t tsize;
  char   c;
  int    rc;
  
  ddt(pbuf != NULL);
  ddt(pd   != NULL);
  ddt(ps   != NULL);
  ddt(*ps  >  1);

  for (tsize = 0 ; ; )
  {
    size = *ps;
    rc   = line_read(pbuf,pd,&size);
  
    ddt(size <= *ps);
  
    /*------------------------------------------
    ; bail if we've read all we could, or nothing
    ;-----------------------------------------*/
  
    if ((size == *ps) || (size == 0)) 
    {
      *ps = size;
      return(rc);
    }

    tsize += size;

    line_readchar(pbuf,&c);
    if ((c == '\r') || (c == '\n') || (!isspace(c)))
    {
      line_unreadchar(pbuf,c);
      *ps = tsize;
      return(rc);
    }

    pd   += size;
    *pd++ = ' ';
    size--;
    *ps  -= size;
  }
}

/*****************************************************************/
    
static int line_readchar(struct line_buffer *pbuf,char *d)
{
  ddt(pbuf != NULL);
  ddt(d    != NULL);
  
  if (pbuf->idx == pbuf->max)
  {
    pbuf->size = LINEBUFSIZE;
    BufferRead(pbuf->cbuf,&pbuf->buffer[READBACK],&pbuf->size);
    if (pbuf->size == 0)
    {
      *d        = '\0';
      pbuf->eof = TRUE;
      return(ERR_OKAY);
    }
    pbuf->idx   = READBACK;
    pbuf->max   = READBACK + pbuf->size;
  }

  *d = pbuf->buffer[pbuf->idx++];
  pbuf->size--;

  if (pbuf->size == 0)
  {
    pbuf->size = LINEBUFSIZE;
    BufferRead(pbuf->cbuf,&pbuf->buffer[READBACK],&pbuf->size);
    if (pbuf->size == 0)
    {
      pbuf->eof = TRUE;
    }
    pbuf->idx = READBACK;
    pbuf->max = READBACK + pbuf->size;
  }

  return(ERR_OKAY);
}

/****************************************************************/

static int line_flush(struct line_buffer *pbuf)
{
  ddt(pbuf != NULL);
  
  if (pbuf->size > 0)
  {
    BufferWrite(pbuf->cbuf,&pbuf->buffer[pbuf->odx],&pbuf->size);
  }
  pbuf->max  = FULLSIZE;
  pbuf->idx  = READBACK;
  pbuf->odx  = READBACK;
  pbuf->size = 0;
  return(ERR_OKAY);
}

/********************************************************************/

static int line_write(struct line_buffer *pbuf,char *src,size_t *ps)
{
  size_t size;
  
  ddt(pbuf != NULL);
  ddt(src  != NULL);
  ddt(ps   != NULL);
  
  size = 0;
  while((size < *ps) && (*src))
  {
    line_writechar(pbuf,*src++);
    size++;
  }
  src = pbuf->eol;
  while(*src)
    line_writechar(pbuf,*src++);
  *ps = size;
  return(ERR_OKAY);
}

/*******************************************************************/
  
static int line_writechar(struct line_buffer *pbuf,char c)
{
  ddt(pbuf != NULL);
  
  if (pbuf->idx == pbuf->max)
    line_flush(pbuf);
  pbuf->buffer[pbuf->idx++] = c;
  pbuf->size++;
  return(ERR_OKAY);
}

/******************************************************************/

static int line_unreadchar(struct line_buffer *pbuf,char c)
{
  ddt(pbuf != NULL);
  
  if (pbuf->idx == 0) return(BUFERR_NOPUSH);
  pbuf->buffer[--pbuf->idx] = c;
  pbuf->size++;
  pbuf->eof = FALSE;
  return(ERR_OKAY);
}

/**********************************************************************/

