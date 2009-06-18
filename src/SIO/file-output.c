
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "../types.h"
#include "../memory.h"
#include "../ddt.h"
#include "../sio.h"

/************************************************************/

static int		mmap_close	(SInput);
static int		file_read	(SInput);
static struct blockdata	file_readblock	(SInput);
static struct blockdata	file_readstr	(SInput);
static int		file_readreturn	(SInput,struct blockdata);
static int		file_rewind	(SInput);
static int		file_close	(SInput);

/****************************************************************/

struct file_sinput
{
  struct sinput  base;
  int            fh;
  size_t         max;
  size_t         used;
  size_t         idx;
  char           buffer[BUFSIZ];
};

/*********************************************************************/

SInput FileSInput(const char *fname)
{
  struct stat  status;
  int          fh;
  int          rc;
  void        *data;
  
  ddt(fname != NULL);

  fh = open(fname,O_RDONLY);
  if (fh == -1) 
    goto handle_error;
  
  rc = fstat(fh,&status);
  if (rc < 0)
  {
    close(fh);
    goto handle_error;
  }

  data = mmap(NULL,status.st_size,PROT_READ,MAP_PRIVATE,fh,0);
  if (data == (void *)-1)
  {
    struct file_sinput *si;
    
    si = (struct file_sinput *)SInputNew(sizeof(struct file_sinput));
    si->base.read       = file_read;
    si->base.readblock  = file_readblock;
    si->base.readstr    = file_readstr;
    si->base.readreturn = file_readreturn;
    si->base.rewind     = file_rewind;
    si->base.close      = file_close;
    si->fh              = fh;
    si->max             = sizeof(si->buffer);
    si->used            = sizeof(si->buffer);
    si->idx             = sizeof(si->buffer);
    return (SInput)si;
  }
  else
  {
    SInput si;
    
    close(fh);
    si = MemorySInput(data,status.st_size);
    si->close = mmap_close;
    return si;
  }
  
  handle_error:
  {
    int err   = errno;
    SInput si = SInputNew(sizeof(struct sinput));
    
    si->err   = err;
    si->f.err = TRUE;
    si->f.eof = TRUE;
    return si;
  }
}

/***********************************************************************/

static int mmap_close(SInput me)
{
  struct mem_sinput *si = (struct mem_sinput *)me;
  
  ddt(me != NULL);
  
  munmap((void *)si->data,si->max);
  MemFree(si);
  return 0;
}

/************************************************************************/

static int file_read(SInput me)
{
  struct file_sinput *si = (struct file_sinput *)me;
  int                 rc;
  
  ddt(me != NULL);
  
  if (si->idx == si->used)
  {
    rc = read(si->fh,si->buffer,si->max);
    if (rc < 0)
    {
      si->base.err   = errno;
      si->base.f.err = TRUE;
      return IEOF;
    }
    
    if (rc == 0)
    {
      si->base.f.eof = TRUE;
      return IEOF;
    }
    
    si->used = rc;
    si->idx  = 0;
  }
  
  si->base.size++;
  return si->buffer[si->idx++];
}

/***********************************************************************/

static struct blockdata file_readblock(SInput me)
{
  struct file_sinput *si = (struct file_sinput *)me;
  struct blockdata    result;
  
  ddt(me != NULL);
  
  if (si->base.f.eof)
  {
    result.size = 0;
    result.data = NULL;
    return result;
  }
  
  result.size = si->idx;
  result.data = si->buffer;
  return result;
}

/***********************************************************************/

static struct blockdata file_readstr(SInput me)
{
  struct file_sinput *si = (struct file_sinput *)me;
  struct blockdata    result;
  char               *dest;
  size_t              dmax;
  size_t              duse;
  int                 rc;
  
  ddt(me      != NULL);
  ddt(si->idx <= si->used);
  
  dmax        = 0;
  duse        = 0;
  dest        = NULL;

  while(1)
  {
    if (duse == dmax)
    {
      dmax += BUFSIZ;
      dest  = MemResize(dest,dmax);
    }
    
    if (si->idx == si->used)
    {
      rc = read(si->fh,si->buffer,si->max);
      if (rc < 0)
      {
        si->base.err   = errno;
        si->base.f.err = TRUE;
        goto do_return;
      }
      
      if (rc == 0)
      {
        si->base.f.eof = TRUE;
        goto do_return;
      }
      
      si->used = rc;
      si->idx  = 0;
    }
    
    if (si->buffer[si->idx] == '\n')
    {
      si->idx++;
      goto do_return;
    }
    
    dest[duse++] = si->buffer[si->idx++];
    dest[duse]   = '\0';
  }
  
  do_return:
  {
    result.size = duse;
    result.data = dest;
    return result;
  }
}

/*****************************************************************/

static int file_readreturn(SInput me,struct blockdata data)
{
  struct file_sinput *si = (struct file_sinput *)me;
  
  ddt(me        != NULL);
  ddt(data.size >  0);
  ddt(data.data != NULL);
  
  if (((char *)data.data >= si->buffer) && ((char *)data.data < &si->buffer[si->max]))
  {
    si->idx += data.size;
    return 0;
  }
  
  MemFree(data.data);
  return 0;
}

/*******************************************************************/

static int file_rewind(SInput me)
{
  struct file_sinput *si = (struct file_sinput *)me;
  int                 rc;
  
  ddt(me != NULL);
  
  si->used = sizeof(si->buffer);
  si->idx  = sizeof(si->buffer);
  rc       = lseek(si->fh,0,SEEK_SET);
  if (rc == -1)
  {
    si->base.f.err = TRUE;
    si->base.err   = errno;
    return 1;
  }
  
  return 0;
}

/******************************************************************/

static int file_close(SInput me)
{
  struct file_sinput *si = (struct file_sinput *)me;
  
  ddt(me != NULL);
  
  close(si->fh);
  MemFree(si);
  return 0;
}

/******************************************************************/

