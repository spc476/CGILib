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
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "../types.h"
#include "../memory.h"
#include "../errors.h"
#include "../buffer.h"
#include "../util.h"
#include "../rawfmt.h"
#include "../ddt.h"

/*********************************************************************/

static void	buffer_fwvcb	(char,void *);

/************************************************************************/

void (BufferInit)(void)
{
  ErrHandlerNew(&BufferErr);
}

/************************************************************************/

int (BufferIOCtl)(const Buffer buf,int cmd, ... )
{
  va_list alist;
  int     rc;
  
  ddt(buf        != NULL);
  ddt(buf->ioreq != NULL);
  ddt(cmd        >  -1);
  
  va_start(alist,cmd);
  rc = (*buf->ioreq)(buf,cmd,alist);
  va_end(alist);
  return(rc);
}
 
/***********************************************************************/

int (BufferRead)(const Buffer buf,const void *dest,size_t *psize)
{
  ddt(buf   != NULL);
  ddt(dest  != NULL);
  ddt(psize != NULL);

  return(BufferIOCtl(buf,C_READ,dest,psize));
}

/***********************************************************************/

int (BufferFormatRead)(const Buffer buf,const char *format,const char *msg, ... )
{
  ddt(buf    != NULL);
  ddt(format != NULL);
  ddt(0);

  return(ErrorPush(CgiErr,BUFFERFORMATREAD,ERR_NOTIMP,""));
}
/**********************************************************************/

int (BufferWrite)(const Buffer buf,const void *src,size_t *psize)
{
  ddt(buf   != NULL);
  ddt(src   != NULL);
  ddt(psize != NULL);
  
  return(BufferIOCtl(buf,C_WRITE,src,psize));
}

/*********************************************************************/

int (BufferFormatWrite)(const Buffer buf,const char *format,const char *msg, ... )
{
  va_list list;
  int     rc;
  
  ddt(buf    != NULL);
  ddt(format != NULL);
  ddt(msg    != NULL);

  va_start(list,msg);
  rc = BufferFormatWritev(buf,format,msg,list);
  va_end(list);
  return(rc);  
}

/**********************************************************************/

int (BufferFormatWritev)(const Buffer buf,const char *format,const char *msg,va_list list)
{
  ddt(buf    != NULL);
  ddt(format != NULL);
  ddt(msg    != NULL);

  RawDoFmtv(format,msg,buffer_fwvcb,buf,list);
  return(ERR_OKAY);
}

/***********************************************************************/

static void buffer_fwvcb(char c,void *datum)
{
  size_t size   = 1;
  
  ddt(datum != NULL);
  
  BufferWrite((Buffer)datum,&c,&size);
}

/********************************************************************/

int (BufferSeek)(const Buffer buf,long *pl,int whence)
{
  ddt(buf != NULL);
  ddt(pl  != NULL);
  ddt((whence == SEEK_START) || (whence == SEEK_CURRENT) || (whence == SEEK_END));

  return(BufferIOCtl(buf,C_SEEK,pl,whence));
}

/********************************************************************/

int (BufferFlush)(const Buffer buf)
{
  ddt(buf != NULL);

  return(BufferIOCtl(buf,C_FLUSH));
}

/********************************************************************/

int (BufferEOF)(const Buffer buf)
{
  ddt(buf != NULL);

  return(BufferIOCtl(buf,CL_EOF));
}

/******************************************************************/

unsigned long (BufferSize)(const Buffer buf)
{
  unsigned long l;

  ddt(buf != NULL);
  BufferIOCtl(buf,C_SIZE,&l);
  return(l);
}

/*******************************************************************/

int (BufferCopy)(const Buffer dest,const Buffer src)
{
  char   buffer[BUFSIZ];
  size_t size;
  int    rc;

  ddt(dest != NULL);
  ddt(src  != NULL);

  while(!BufferIOCtl(src,CD_READEOF))
  {
    size = BUFSIZ;
    rc   = BufferRead(src,buffer,&size);
    if (rc != ERR_OKAY) return(ErrorPush(CgiErr,BUFFERCOPY,rc,"$","input"));
    rc   = BufferWrite(dest,buffer,&size);
    if (rc != ERR_OKAY) return(ErrorPush(CgiErr,BUFFERCOPY,rc,"$","output"));
  }
  return(ERR_OKAY);
}

/******************************************************************/

int (BufferFree)(Buffer *pbuf)
{
  int rc;

  ddt(pbuf != NULL);

  rc    = BufferIOCtl(*pbuf,C_FREE);
  *pbuf = NULL;
  return(rc);
}

/*******************************************************************/

int (LineSetEOL)(const Buffer buf,const char *eol)
{
  ddt(buf != NULL);
  ddt(eol != NULL);

  return(BufferIOCtl(buf,CL_SETEOL,eol));
}

/*******************************************************************/

int (LineRead822)(const Buffer buf,const char *dest,size_t *pdsize)
{
  ddt(buf    != NULL);
  ddt(dest   != NULL);
  ddt(pdsize != NULL);

  return(BufferIOCtl(buf,CL_READ822,dest,pdsize));
}

/******************************************************************/

int (LineReadCLine)(const Buffer buf,const char *dest,size_t *pdsize)
{
  ddt(buf    != NULL);
  ddt(dest   != NULL);
  ddt(pdsize != NULL);

  return(BufferIOCtl(buf,CL_READCLINE,dest,pdsize));
}

/*****************************************************************/

int (LineReadC)(const Buffer buf,const char *dest)
{
  ddt(buf  != NULL);
  ddt(dest != NULL);

  return(BufferIOCtl(buf,CL_READC,dest));
}

/*********************************************************************/

int (LineUnReadC)(const Buffer buf,char c)
{
  ddt(buf != NULL);

  return(BufferIOCtl(buf,CL_UNREADC,c));
}

/*******************************************************************/

int (LineWriteC)(const Buffer buf,char c)
{
  ddt(buf != NULL);

  return(BufferIOCtl(buf,CL_WRITEC,c));
}

/********************************************************************/

static void line_writefcb(char c,void *datum)
{
  LineWriteC(datum,c);
}

/*********************************************************************/

int (LineWritef)(const Buffer buf,const char *fmt,const char *msg, ... )
{
  va_list alist;

  ddt(buf != NULL);
  ddt(fmt != NULL);
  ddt(msg != NULL);
  
  va_start(alist,msg);
  if (buf->tag != LINEBUFFER)
    return(ERR_NOTIMP);
  RawDoFmtv(fmt,msg,line_writefcb,buf,alist);
  return(ERR_OKAY);
}

/*********************************************************************/

int (LineEOF)(const Buffer buf)
{
  ddt(buf != NULL);
  
  return(BufferIOCtl(buf,CL_EOF));
}

/**********************************************************************/

