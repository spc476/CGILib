
/**********************************************************************
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

#include "../types.h"
#include "../memory.h"
#include "../errors.h"
#include "../buffer.h"
#include "../util.h"
#include "../ddt.h"

/************************************************************************/

struct mem_buffer
{
  struct buffer  buf;
  unsigned long  seek;
  unsigned long  usize;
  unsigned long  size;
  char          *buffer;
};

/************************************************************************/

static int		 mem_ioreq	(struct buffer     *,int,va_list);
static int		 mem_read	(struct mem_buffer *,void *,size_t *);
static int		 mem_write	(struct mem_buffer *,void *,size_t *);
static int		 mem_seek	(struct mem_buffer *,long *,int);

/************************************************************************/

int MemoryBuffer(Buffer *pbuf,void *mem,size_t size)
{
  struct mem_buffer *buf;
  
  ddt(pbuf != NULL);
  ddt(mem  != NULL);
  ddt(size >  0);
  
  buf             = MemAlloc(sizeof(struct mem_buffer));
  buf->buf.size   = sizeof(struct mem_buffer);
  buf->buf.tag    = MEMORYBUFFER;
  buf->buf.ioreq  = mem_ioreq;
  buf->seek       = 0;
  buf->usize      = 0;
  buf->size       = size;
  buf->buffer     = mem;
  *pbuf           = (Buffer)buf;
  return(ERR_OKAY);
}

/*********************************************************************/

static int mem_ioreq(struct buffer *pbuf,int cmd,va_list alist)
{
  struct mem_buffer *pmb = (struct mem_buffer *)pbuf;
  unsigned long     *pul;
  unsigned long      ul;
  void              *pd;
  size_t            *ps;
  long              *pl;
  int                wh;
  int                rc;

  ddt(pbuf != NULL);
  
  switch(cmd)
  {
    case C_BLOCKSIZE:
         pul  = va_arg(alist,unsigned long *);
	 ddt(pul != NULL);
	 *pul = 1;
	 rc   = ERR_OKAY;
	 break;
    case C_SIZE:
         pul  = va_arg(alist,unsigned long *);
         ddt(pul != NULL);
         *pul = pmb->size;
         rc   = ERR_OKAY;
         break;
    case CM_SETSIZE:
         ul         = va_arg(alist,unsigned long);
         pmb->usize = ul;
         rc         = ERR_OKAY;
         break;
    case C_READ:
         pd = va_arg(alist,void *);
         ps = va_arg(alist,size_t *);
         rc = mem_read(pmb,pd,ps);
         break;
    case C_WRITE:
         pd = va_arg(alist,void *);
         ps = va_arg(alist,size_t *);
         rc = mem_write(pmb,pd,ps);
         break;
    case C_SEEK:
         pl = va_arg(alist,long *);
         wh = va_arg(alist,int);
         rc = mem_seek(pmb,pl,wh);
         break;
    case C_FREE:
         MemFree(pmb,sizeof(struct mem_buffer));
         rc = ERR_OKAY;
         break;
    case CD_READEOF:
    case CL_EOF:
         rc = (pmb->seek == pmb->usize)
                ? BUFERR_EOF
                : ERR_OKAY;
         break;
    case C_FLUSH:
    case C_IOCTL:
    default:
         D(ddtlog(ddtstream,"i","unimpl memorybuff command %a",cmd);)
         ddt(0);
         rc = ERR_NOTIMP;
         break;
  }
  return(rc);
}

/*******************************************************************/

static int mem_read(struct mem_buffer *pbuf,void *pd,size_t *ps)
{
  size_t size;
  
  ddt(pbuf != NULL);
  ddt(pd   != NULL);
  ddt(ps   != NULL);
  ddt(*ps  >  0);
  
  size = (*ps < pbuf->size - pbuf->seek) 
           ? *ps 
           : pbuf->size - pbuf->seek;

  if (size)
  {
    memcpy(pd,&pbuf->buffer[pbuf->seek],size);
  }
  pbuf->seek += size;
  *ps         = size;
  return(ERR_OKAY);
}

/*********************************************************************/

static int mem_write(struct mem_buffer *pbuf,void *pd,size_t *ps)
{
  size_t size;
  
  ddt(pbuf != NULL);
  ddt(pd   != NULL);
  ddt(ps   != NULL);
  ddt(*ps  >= 0);
  
  size = (*ps < pbuf->size - pbuf->seek) 
           ? *ps 
           : pbuf->size - pbuf->seek;

  if (size) 
  {
    memcpy(&pbuf->buffer[pbuf->seek],pd,size);
  }
  pbuf->seek += size;
  *ps         = size;
  if (pbuf->seek > pbuf->usize) pbuf->usize = pbuf->seek;
  return(ERR_OKAY);
}

/*********************************************************************/

static int mem_seek(struct mem_buffer *pbuf,long *pl,int whence)
{
  long             seek;
  unsigned long    useek;
  unsigned long    stseek;	/* assigned before use (-Wall ... )*/
  unsigned long    nseek;
  int              rc;

  ddt(pbuf != NULL);
  ddt(pl   != NULL);
  ddt(
       (whence == SEEK_START) 
       || (whence == SEEK_CURRENT) 
       || (whence == SEEK_END)
     );

  seek  = *pl;
  useek = labs(seek);
  rc    = ERR_OKAY;
  
  switch(whence)
  {
    case SEEK_START:   stseek = 0;		 break;
    case SEEK_CURRENT: stseek = pbuf->seek; 	 break;
    case SEEK_END:     stseek = pbuf->size - 1;  break;
    default:           ddt(0);
  }

  if (seek < 0)
    if (useek > stseek)
      nseek = 0;
    else
      nseek = stseek - useek;
  else
    nseek = stseek + useek;

  if (nseek < pbuf->size)
    pbuf->seek = nseek;
  else
  {
    ErrorPush(CgiErr,BUFSEEK,BUFERR_SEEK,"");
    ErrThrow(&BufferErr);
    rc  = BUFERR_SEEK;
    *pl = -1;
  }  
  return(rc);
}

/************************************************************************/

