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

#include <stdarg.h>

#include "../types.h"
#include "../memory.h"
#include "../errors.h"
#include "../buffer.h"
#include "../util.h"
#include "../ddt.h"

/*********************************************************************/

struct null_buffer
{
  struct buffer buf;
  unsigned long seek;
};

/********************************************************************/

static int	 null_ioreq	(struct buffer *,int,va_list);

/************************************************************************/

int NullBuffer(Buffer *pbuf)
{
  struct null_buffer *buf;

  ddt(pbuf != NULL);

  buf		  = MemAlloc(sizeof(struct null_buffer));
  buf->buf.size   = sizeof(struct null_buffer);
  buf->buf.tag	  = NULLBUFFER;
  buf->buf.ioreq  = null_ioreq;
  buf->seek	  = 0;
  *pbuf 	  = (Buffer)buf;
  return(ERR_OKAY);
}

/*********************************************************************/

static int null_ioreq(struct buffer *pbuf,int cmd,va_list alist)
{
  struct null_buffer *pnf = (struct null_buffer *)pbuf;
  void		     *p;
  size_t             *ps;
  long		     *pl;
  int		      whence;
  int		      rc;

  ddt(pbuf != NULL);

  rc = ERR_OKAY;

  switch(cmd)
  {
    case C_READ:
    case C_WRITE:
	 p  = va_arg(alist,void *);
	 ps = va_arg(alist,size_t *);
	 pnf->seek += *ps;
	 if (cmd == C_READ)
	   *ps = 0;
	 break;
    case C_SEEK:
	 pl	= va_arg(alist,long *);
	 whence = va_arg(alist,int);
	 if (whence == SEEK_START)
	   pnf->seek = *pl;
	 else if ((whence == SEEK_CURRENT) || (whence == SEEK_END))
	   pnf->seek += *pl;
	 else
	   ddt(0);
#if 0
	 if (pnf->seek < 0)
	   pnf->seek = 0;
#endif
	 *pl = pnf->seek;
	 break;
    case C_FREE:
	 MemFree(pnf,sizeof(struct null_buffer));
	 break;
    case C_SIZE:
         pl  = va_arg(alist,long *);
         *pl = pnf->seek;
         break;
    case C_BLOCKSIZE:
         pl  = va_arg(alist,long *);
	 *pl = 1;
	 break;
    default:
	 break;
  }
  return(rc);
}

/**********************************************************************/

