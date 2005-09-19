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

#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

#include "../ddt.h"
#include "../memory.h"
#include "../errors.h"
#include "../util.h"
#include "../types.h"
#include "../stream.h"

#define FHBUFFER_SIZE	8192	
#define FHBUFFER_WBSIZE	  64

#ifndef SHUT_RD
#  define SHUT_RD	0
#endif

#ifndef SHUT_WR
#  define SHUT_WR	1
#endif

/***********************************************************************/

struct fhmod
{
  char           *name;
  int             fh;
  void           *data;
  off_t           rsize;
  int           (*lowwrite)();
  int	        (*prevread)();
  unsigned char   wb[FHBUFFER_WBSIZE];
  size_t          wbsize;
};

static Stream        fh_create_read	(const char *,int,int);
static Stream        fh_create_write	(const char *,int,int);
static struct fhmod *fh_create		(const char *,int);
static int	     fh_writeflush	(struct stream *,struct streamvector *,int);
static int	     fh_refill		(struct stream *,struct streamvector *);
static int	     fh_flush		(struct stream *,struct streamvector *);
static int	     fh_close		(struct stream *,struct streamvector *);
static int	     tcp_closeread	(struct stream *,struct streamvector *);
static int	     tcp_closewrite	(struct stream *,struct streamvector *);
static int	     fh_unread		(struct stream *,struct streamvector *,int);
static int	     fh_readunread	(struct stream *,struct streamvector *);

/************************************************************************/

Stream (FHStreamRead)(int fh)
{
  ddt(fh > -1);
  return(fh_create_read("",fh,FALSE));
}  

/************************************************************************/

Stream (FHStreamWrite)(int fh)
{
  ddt(fh > -1);
  return(fh_create_write("",fh,TRUE));
}

/***********************************************************************/

Stream (FileStreamRead)(const char *name)
{
  int fh;
  
  ddt(name != NULL);
  
  fh = open(name,O_RDONLY);
  if (fh == -1)
    return(NULL);

  return(fh_create_read(name,fh,TRUE));
}

/***********************************************************************/

Stream (FileStreamWrite)(const char *name,int mode)
{
  int fh;
  
  ddt(name != NULL);
  
  fh = open(name,O_WRONLY | mode,0666);
  if (fh == -1)
    return(NULL);
    
  return(fh_create_write(name,fh,FALSE));  
}

/**********************************************************************/

int (TCPStream)(Stream io[2],char *host,unsigned short port)
{
  struct hostent     *hp;
  struct sockaddr_in  lsin;
  int                 fh;
  int                 rc;
  
  ddt(host != NULL);
    
  fh = socket(AF_INET,SOCK_STREAM,0);
  if (fh < 0)
    return(ERR_ERR);
  
  memset(&lsin,0,sizeof(lsin));
  lsin.sin_family = AF_INET;
  lsin.sin_port   = htons(port);

  hp = gethostbyname(host);
  if (hp == NULL)
    return(ERR_ERR);
  
  memcpy(&(lsin.sin_addr.s_addr),hp->h_addr,hp->h_length);

  rc = connect(fh,(struct sockaddr *)&lsin,sizeof(lsin));

  if (rc < 0)
  {
    close(fh);
    return(ERR_ERR);
  }
  
  io[0]              = fh_create_read(host,fh,FALSE);
  io[0]->calls.close = tcp_closeread;
  io[1]              = fh_create_write(host,fh,FALSE);
  io[1]->calls.close = tcp_closewrite;
  
  return(ERR_OKAY);
}

/***********************************************************************/

static Stream fh_create_read(const char *name,int fh,int refill)
{
  Stream s;
  struct fhmod *mod;
  
  ddt(name != NULL);
  ddt(fh   >  -1);
  
  mod = fh_create(name,fh);
  s   = MemoryStreamRead(mod->data,FHBUFFER_SIZE);
  
  s->calls.refillflush = fh_refill;
  s->calls.unrw        = fh_unread;
  s->calls.close       = fh_close;
  s->calls.user        = mod;
  
  /*------------------------------------------------------------------
  ; for interactive streams (like StdinSTream or a TCPStreamRead)
  ; we don't want to prefill the buffer, so we set s->off = s->size
  ; so that when we actually try to read, it will block *then*.
  ; For non-interactive streams like a disk, we can safely prefill
  ; the stream.
  ;-------------------------------------------------------------------*/
  
  if (refill) 
    StreamRefill(s);
  else
    s->off = s->size;
    
  return(s);
}

/**********************************************************************/

static Stream fh_create_write(const char *name,int fh,int flush)
{
  Stream        s;
  struct fhmod *mod;
  
  ddt(name != NULL);
  ddt(fh   >  -1);
  
  mod = fh_create(name,fh);
  s   = MemoryStreamWrite(mod->data,FHBUFFER_SIZE);
  
  s->calls.refillflush = fh_flush;
  s->calls.close       = fh_close;
  s->calls.user        = mod;
  
  if (flush)
  {
    mod->lowwrite      = s->calls.readwrite;
    s->calls.readwrite = fh_writeflush;
  }
  
  return(s);
}

/**********************************************************************/

static struct fhmod *fh_create(const char *name,int fh)
{
  struct fhmod *mod;
  
  ddt(name != NULL);
  ddt(fh   >  -1);
  
  mod         = MemAlloc(sizeof(struct fhmod));
  mod->data   = MemAlloc(FHBUFFER_SIZE);
  mod->name   = dup_string(name);
  mod->fh     = fh;
  mod->rsize  = 0;
  mod->wbsize = 0;
  return(mod);
}

/***********************************************************************/

static int fh_writeflush(struct stream *s,struct streamvector *v,int c)
{
  struct fhmod *mod;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  mod = v->user;
  (*mod->lowwrite)(s,v,c);
  fh_flush(s,v);
  return(c);
}

/**********************************************************************/

static int fh_refill(struct stream *s,struct streamvector *v)
{
  struct fhmod *mod;
  ssize_t       rc;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  mod  = v->user;
  
  while(1)
  {
    rc = read(mod->fh,s->data,FHBUFFER_SIZE);
    if (rc < 0)
    {
      if (errno == EINTR) continue;
      return(FALSE);
    }
    else if (rc == 0)
    {
      s->eof = TRUE;
      return(FALSE);
    }
    else
    {
      mod->rsize += rc;
      s->size     = rc;
      s->off      = 0;
      return(TRUE);
    }
  }
}

/**********************************************************************/

static int fh_flush(struct stream *s,struct streamvector *v)
{
  struct fhmod  *mod;
  ssize_t        rc;
  ssize_t        size;
  unsigned char *data;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  mod   = v->user;
  data  = s->data;
  size  = s->off;
  
  while(size)
  {
    rc = write(mod->fh,data,size);
    if (rc < 0)
    {
      if (errno == EINTR) continue;
      rc = errno;
      return(FALSE);
    }
    else if (rc == 0)
    {
      s->eof = TRUE;
      return(FALSE);
    }
    else
    {
      size       -= rc;
      data       += rc;
      mod->rsize += rc;
    }
  }
  s->off = 0;
  return(TRUE);
}

/*********************************************************************/

static int fh_close(struct stream *s,struct streamvector *v)
{
  struct fhmod *mod;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  mod = v->user;
  ddt(mod->fh != 0);
  ddt(mod->fh != 1);
  ddt(mod->fh != 2);
  close(mod->fh);
  MemFree(mod->name);
  MemFree(mod->data);
  MemFree(mod);
  return(0);
}

/*********************************************************************/

static int tcp_closeread(struct stream *s,struct streamvector *v)
{
  struct fhmod *mod;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  mod = v->user;
  shutdown(mod->fh,SHUT_RD);
  MemFree(mod->name);
  MemFree(mod->data);
  MemFree(mod);
  return(0);
}

/***********************************************************************/

static int tcp_closewrite(struct stream *s,struct streamvector *v)
{
  struct fhmod *mod;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  mod = v->user;
  shutdown(mod->fh,SHUT_WR);
  MemFree(mod->name);
  MemFree(mod->data);
  MemFree(mod);
  return(0);
}

/**************************************************************************/

static int fh_unread(struct stream *s,struct streamvector *v,int c)
{
  struct fhmod *mod;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  if (c == IEOF)
  {
    s->eof = TRUE;
    return(c);
  }

  mod = v->user;
  
  if (mod->wbsize == 0)
  {
    mod->prevread = v->readwrite;
    v->readwrite  = fh_readunread;
  }
  
  if (mod->wbsize < FHBUFFER_WBSIZE)
    mod->wb[mod->wbsize++] = c;
  return(c);
}

/************************************************************************/

static int fh_readunread(struct stream *s,struct streamvector *v)
{
  struct fhmod *mod;
  int           c;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  if (s->eof)
    return(IEOF);
  
  mod = v->user;
  
  ddt(mod->wbsize > 0);
  
  c = mod->wb[--mod->wbsize];
  if (mod->wbsize == 0)
    v->readwrite = mod->prevread;
    
  return(c);
}

/***********************************************************************/

