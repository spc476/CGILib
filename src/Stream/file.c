
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "../ddt.h"
#include "../memory.h"
#include "../errors.h"
#include "../util.h"

#include "../stream.h"

#define FHBUFFER_SIZE	8192

/***********************************************************************/

struct fhmod
{
  char  *name;
  int    fh;
  void  *data;
  off_t  rsize;
};

static Stream        fh_create_read	(char *,int);
static Stream        fh_create_write	(char *,int);
static struct fhmod *fh_create		(char *,int);
static int	     fh_refill		(struct stream *,struct streamvector *);
static int	     fh_flush		(struct stream *,struct streamvector *);
static int	     fh_close		(struct stream *,struct streamvector *);

/************************************************************************/

Stream (FHStreamRead)(int fh)
{
  ddt(fh > -1);
  return(fh_create_read("",fh));
}  

/************************************************************************/

Stream (FHStreamWrite)(int fh)
{
  ddt(fh > -1);
  return(fh_create_write("",fh));
}

/***********************************************************************/

Stream (FileStreamRead)(char *name)
{
  int fh;
  
  ddt(name != NULL);
  
  fh = open(name,O_RDONLY);
  if (fh == -1)
    return(NULL);

  return(fh_create_read(name,fh));
}

/***********************************************************************/

Stream (FileStreamWrite)(char *name,int mode)
{
  int fh;
  
  ddt(name != NULL);
  
  fh = open(name,O_WRONLY | mode,0666);
  if (fh == -1)
    return(NULL);
    
  return(fh_create_write(name,fh));  
}

/**********************************************************************/

static Stream fh_create_read(char *name,int fh)
{
  Stream s;
  struct fhmod *mod;
  
  ddt(name != NULL);
  ddt(fh   >  -1);
  
  mod = fh_create(name,fh);
  s   = MemoryStreamRead(mod->data,FHBUFFER_SIZE);
  
  s->calls.refillflush = fh_refill;
  s->calls.close       = fh_close;
  s->calls.user        = mod;
  StreamRefill(s);
  return(s);
}

/**********************************************************************/

static Stream fh_create_write(char *name,int fh)
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
  return(s);
}

/**********************************************************************/

static struct fhmod *fh_create(char *name,int fh)
{
  struct fhmod *mod;
  
  ddt(name != NULL);
  ddt(fh   >  -1);
  
  mod        = MemAlloc(sizeof(struct fhmod));
  mod->data  = MemAlloc(FHBUFFER_SIZE);
  mod->name  = dup_string(name);
  mod->fh    = fh;
  mod->rsize = 0;
  return(mod);
}

/***********************************************************************/

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
  struct fhmod *mod;
  ssize_t       rc;
  ssize_t       size;
  char         *data;
  
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
  close(mod->fh);
  MemFree(mod->name,strlen(mod->name) + 1);
  MemFree(mod->data,FHBUFFER_SIZE);
  MemFree(mod,sizeof(struct fhmod));
  return(0);
}

/*********************************************************************/

