
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "../types.h"
#include "../conf.h"
#include "../buffer.h"
#include "../sio.h"
#include "../errors.h"
#include "../memory.h"
#include "../ddt.h"
#include "../util.h"

/************************************************************************/

struct ufh_buffer
{
  struct buffer  buf;
  int            fh;
  char          *hostfile;
  int            modeport;
  int            eof;
};

/**********************************************************************/

static struct ufh_buffer *ufh_buffer_new(Tag,int,const char *,int);
static int		  ufh_ioreq	(struct buffer *const,int,va_list);
static int		  ufh_read	(struct ufh_buffer *,void *,size_t *);
static int		  ufh_write	(struct ufh_buffer *,void *,size_t *);
static int		  ufh_size	(struct ufh_buffer *,unsigned long *);
static int		  ufh_seek	(struct ufh_buffer *,long *,int);

/************************************************************************/

static struct ufh_buffer *ufh_buffer_new(Tag tag,int fh,const char *hf,int mp)
{
  struct ufh_buffer *buf;
  
  ddt((tag == FILEBUFFER) || (tag == FHBUFFER) || (tag == TCPBUFFER));
  ddt(fh   >= 0);
  ddt(hf   != NULL);
  
  buf            = MemAlloc(sizeof(struct ufh_buffer));
  buf->buf.size  = sizeof(struct ufh_buffer);
  buf->buf.tag   = tag;
  buf->buf.ioreq = ufh_ioreq;
  buf->fh        = fh;
  buf->hostfile  = dup_string(hf);
  buf->modeport  = mp;
  buf->eof       = FALSE;
  return(buf);
}

/***********************************************************************/

int FHBuffer(Buffer *pbuf,int fh)
{
  ddt(pbuf != NULL);
  *pbuf = (Buffer)ufh_buffer_new(FHBUFFER,fh,"(internal)",0);
  return(ERR_OKAY);
}

/************************************************************************/

int FileBuffer(Buffer *pbuf,const char *name,Flags mode)
{
  int fh;
  int rc;
    
  ddt(pbuf != NULL);
  ddt(name != NULL);

  do
  {  
    fh = open(name,mode,0644);
    if (fh == -1)
    {
      ErrorPush(KernErr,KERNOPEN,errno,"$i",name,mode);
      ErrorPush(CgiErr,FILEBUFFER,BUFERR_OPEN,"$i",name,mode);
      rc = ErrThrow(&BufferErr);
      if ((rc != ERR_OKAY) && (rc != RETRY))
        return(ErrorTopError());
      else
      {
        rc = ERR_OKAY;
        ErrorClear();
      }
    }
    else
      rc = ERR_OKAY;
  } while(rc != ERR_OKAY);
  
  *pbuf = (Buffer)ufh_buffer_new(FILEBUFFER,fh,name,mode);
  return(ERR_OKAY);
}

/*************************************************************************/

char *FileTmpName(void)
{
  char tmpname[BUFSIZ];
  
  sprintf(tmpname,"/tmp/%lu",(unsigned long)getpid());
  return(dup_string(tmpname));
}

/***********************************************************************/

int FileDelete(const char *fname)
{
  int rc;
  
  ddt(fname != NULL);
  
  do
  {
    rc = unlink(fname);
    if (rc == -1)
    {
      ErrorPush(KernErr,KERNUNLINK,errno,"$",fname);
      ErrorPush(CgiErr,FILEDELETE,FILERR_DELETE,"$",fname);
      rc = ErrThrow(&BufferErr);
      if ((rc != ERR_OKAY) && (rc != ERR_RETRY))
        return(ErrorTopError());
      else
      {
        rc = ERR_OKAY;
        ErrorClear();
      }
    }
    else
      rc = ERR_OKAY;
  } while(rc != ERR_OKAY);
  
  return(rc);
}

/************************************************************************/

int FileRename(const char *fdest,const char *fsrc)  
{
  int rc;
  
  ddt(fdest != NULL);
  ddt(fsrc  != NULL);

  do
  {
    rc = rename(fsrc,fdest);
    if (rc == -1) 
    {
      if (errno == EXDEV)
      {
        char buffer[BUFSIZ];
        
        /*--------------------------------------------------
        ; lazy cheap hack -- sigh.  Ain't Unix wonderful?
        ;
        ; What's going on here is that the rename() command
        ; can't work across file systems, but the shell mv(1) command
        ; can.  Figures.
        ;---------------------------------------------------*/
        
        formatstr(buffer,BUFSIZ,"$ $","mv %a %b",fsrc,fdest);
        system(buffer);
	rc = ERR_OKAY;
      }
      else
      {
        ErrorPush(KernErr,KERNRENAME,errno,"$$",fdest,fsrc);
        ErrorPush(CgiErr,FILERENAME,FILERR_RENAME,"$$",fdest,fsrc);
        rc = ErrThrow(&BufferErr);
        if ((rc != ERR_OKAY) && (rc != ERR_RETRY))
          return(ErrorTopError());
        else
        {
          rc = ERR_OKAY;
          ErrorClear();
        }
      }
    }
    else
      rc = ERR_OKAY;
  } while(rc != ERR_OKAY);
  
  return(rc);
}
  
/***********************************************************************/

int FileRenameDel(const char *fdest,const char *fsrc)
{
  int rc;
  
  ddt(fdest != NULL);
  ddt(fsrc  != NULL);
  
  if ((rc = FileRename(fdest,fsrc)) != ERR_OKAY)
    ErrorPush(CgiErr,FILERENAMEDEL,rc,"$$",fdest,fsrc);
  return(rc);
}

/**********************************************************************/

int (StdinBuffer)(Buffer *pbuf)
{
  ddt(pbuf != NULL);
  
  return(FHBuffer(pbuf,0));
}

/*********************************************************************/

int (StdoutBuffer)(Buffer *pbuf)
{
  ddt(pbuf != NULL);
  
  return(FHBuffer(pbuf,1));
}

/*********************************************************************/

int (StderrBuffer)(Buffer *pbuf)
{
  ddt(pbuf != NULL);
  
  return(FHBuffer(pbuf,2));
}

/*********************************************************************/
  
int TCPBuffer(Buffer *pbuf,const char *host,int port)
{
  int fh;
  int rc;
  
  ddt(pbuf != NULL);
  ddt(host != NULL);
  ddt(port >= 0);
    
  do
  {
    fh = netopen(TOHOST,port,host);
    if (fh == -1)
    {
      ErrorPush(KernErr,KERNSOCKET,1,"$i",host,port);
      ErrorPush(CgiErr,TCPBUFFER,1,"$i",host,port);
      rc = ErrThrow(&BufferErr);
      if ((rc != ERR_OKAY) && (rc != ERR_RETRY))
        return(ErrorTopError());
      else
      {
        rc = ERR_OKAY;
        ErrorClear();
      }  
    }
    else
      rc = ERR_OKAY;
  } while(rc != ERR_OKAY);
  
  *pbuf = (Buffer)ufh_buffer_new(TCPBUFFER,fh,host,port);
  return(ERR_OKAY);
}

/**********************************************************************/

static int ufh_ioreq(struct buffer *const pb,int cmd,va_list alist)
{
  struct ufh_buffer *pbuf = (struct ufh_buffer *)pb;
  int                rc;
  unsigned long     *pul;
  void              *pd;
  size_t            *ps;
  long              *pl;
  int               *pi;
  int                wh;
  
  ddt(pbuf != NULL);
  
  rc = ERR_OKAY;
  
  switch(cmd)
  {
    case CF_HANDLE:
         pi = va_arg(alist,int *);
         ddt(pi != NULL);
         *pi = pbuf->fh;
         break;
    case C_BLOCKSIZE:
         pul = va_arg(alist,unsigned long *);
         ddt(pul != NULL);
         *pul = 512;
         break;
    case C_SIZE:
         pul = va_arg(alist,unsigned long *);
         rc  = ufh_size(pbuf,pul);
         break;
    case C_READ:
         pd = va_arg(alist,void *);
         ps = va_arg(alist,size_t *);
         rc = ufh_read(pbuf,pd,ps);
         break;
    case C_WRITE:
         pd = va_arg(alist,void *);
         ps = va_arg(alist,size_t *);
         rc = ufh_write(pbuf,pd,ps);
         break;
    case C_SEEK:
         pl = va_arg(alist,long *);
         wh = va_arg(alist,int);
         rc = ufh_seek(pbuf,pl,wh);
         break;
    case C_FREE:
         if (pb->tag != FHBUFFER)
	 {
           rc = close(pbuf->fh);
           if (rc == -1)
           {
             ErrorPush(KernErr,KERNCLOSE,errno,"$i",pbuf->hostfile,pbuf->modeport);
             ErrorPush(CgiErr,BUFFERCLOSE,BUFERR_CLOSE,"$i",pbuf->hostfile,pbuf->modeport);
             ErrorLog();
           }
	 }

         MemFree(pbuf->hostfile,strlen(pbuf->hostfile)+1);  
         MemFree(pbuf,sizeof(struct ufh_buffer));
         break;
    case CL_EOF:
    case CD_READEOF:
         rc = (pbuf->eof) ? BUFERR_EOF : ERR_OKAY;
         break;
    case C_FLUSH:
         sync();
	 break;
    case C_IOCTL:
    default:
         D(ddtlog(ddtstream,"i","Bad buffer command: %a",cmd);)
         ddt(0);
	 rc = ERR_NOTIMP;
         break;
  }
  return(rc);
}

/**********************************************************************/

static int ufh_read(struct ufh_buffer *pbuf,void *pd,size_t *ps)
{
  char   *data;
  size_t  size;
  size_t  rsize;
  int     fh;
  int     rc;
  
  ddt(pbuf != NULL);
  ddt(pd   != NULL);
  ddt(ps   != NULL);
  if (*ps < 1)
    ddt(0);
  ddt(*ps  >  0);
  
  size  = *ps;
  rsize = 0;
  data  = pd;
  fh    = pbuf->fh;
  
  while(size)
  {
    rc = read(fh,data,size);
    if (rc < 0)
    {
      if (errno == EINTR) continue;
      ErrorPush(KernErr,KERNREAD,errno,"$i",pbuf->hostfile,pbuf->modeport);
      ErrorPush(CgiErr,BUFREAD,BUFERR_READ,"$i",pbuf->hostfile,pbuf->modeport);
      rc = ErrThrow(&BufferErr);
      if (rc == ERR_RETRY) continue;
      if (rc != ERR_OKAY) 
      {
        pbuf->eof = TRUE;
        return(rc);
      }  
      else
      {
        ErrorClear();
        rc = 0;
      }
    }
    if (rc == 0) 
    {
      pbuf->eof = TRUE;
      break;	/* nothing to read */
    }
    /*size  -= rc;*/
    if (size > (unsigned int)rc)	/* hack to fix ... */
      size -= rc;
    else		/* we read more than we asked ... how would this */
      size = 0;		/* be possible?  Somehow ... 			*/
    data  += rc;
    rsize += rc;
  }
  *ps = rsize;
  return(ERR_OKAY);
}

/*******************************************************************/

static int ufh_write(struct ufh_buffer *pbuf,void *pd,size_t *ps)
{
  char   *data;
  size_t  size;
  size_t  wsize;
  int     fh;
  int     rc;
  
  ddt(pbuf != NULL);
  ddt(pd   != NULL);
  ddt(ps   != NULL);

  /*-------------------------------------------------------------
  ; What does it mean to write 0 bytes?  Should I allow this?  Is
  ; this something that should be allowed or what?  I don't know.
  ;--------------------------------------------------------------*/

  ddt(*ps  >  0);
  
  size  = *ps;
  wsize = 0;
  data  = pd;
  fh    = pbuf->fh;
  
  while(size)
  {
    rc = write(fh,data,size);
    if (rc < 0)
    {
      if (errno == EINTR) continue;
      ErrorPush(KernErr,KERNWRITE,errno,"$i",pbuf->hostfile,pbuf->modeport);
      ErrorPush(CgiErr,BUFWRITE,BUFERR_WRITE,"$i",pbuf->hostfile,pbuf->modeport);
      rc = ErrThrow(&BufferErr);
      if (rc == ERR_RETRY) continue;
      if (rc != ERR_OKAY) 
        return(rc);
      else
      {
        ErrorClear();
        rc = 0;
      }
    }
    if (rc == 0) break;	/* didn't/can't write */
    size  -= rc;
    data  += rc;
    wsize += rc;
  }
  *ps = wsize;
  return(ERR_OKAY);  
}

/*******************************************************************/

static int ufh_size(struct ufh_buffer *pbuf,unsigned long *pul)
{
  int         rc;
  struct stat sbuf;
  
  ddt(pbuf != NULL);
  ddt(pul  != NULL);
  
  while(1)
  {
    rc = fstat(pbuf->fh,&sbuf);
    if (rc == -1)
    {
      ErrorPush(KernErr,KERNSTAT,errno,"$i",pbuf->hostfile,pbuf->modeport);
      ErrorPush(CgiErr,BUFSIZE,BUFERR_SIZE,"$i",pbuf->hostfile,pbuf->modeport);
      rc = ErrThrow(&BufferErr);
      sbuf.st_size = (off_t)1 - (off_t)0;
      if (rc == ERR_RETRY) continue;
    }
    else
      rc = ERR_OKAY;
    break;
  }
  *pul = sbuf.st_size;
  return(rc);
}

/********************************************************************/

static int ufh_seek(struct ufh_buffer *pbuf,long *pl,int wh)
{
  int   rc;
  off_t seek;
  
  ddt(pbuf != NULL);
  ddt(pl   != NULL);
  ddt((wh == SEEK_START) || (wh == SEEK_CURRENT) || (wh == SEEK_END));

  while(1)
  {  
    seek = lseek(pbuf->fh,wh,*pl);
    if (seek < 0)
    {
      ErrorPush(KernErr,KERNSEEK,errno,"$i",pbuf->hostfile,pbuf->modeport);
      ErrorPush(CgiErr,BUFSEEK,BUFERR_SEEK,"$i",pbuf->hostfile,pbuf->modeport);
      rc  = ErrThrow(&BufferErr);
      if (rc == ERR_RETRY) continue;
      seek = (off_t)0 - (off_t)1;
    }
    else
      rc = ERR_OKAY;
    break;
  }
  *pl = (long)seek;
  return(rc);
}

/**********************************************************************/

