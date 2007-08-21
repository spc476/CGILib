
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

#include "../sio.h"
#include "../memory.h"
#include "../util.h"
#include "../errors.h"
#include "../types.h"
#include "../ddt.h"

#define FILE_BUFFER_SIZE	BUFSIZ
#ifndef SHUT_RD
#  define SHUT_RD 0
#endif
#ifndef SHUT_WR
#  define SHUT_WR 1
#endif

/*********************************************************************/

struct sifile
{
  struct sinput base;
  struct buffer data;
  int           fh;
};

struct sofile
{
  struct soutput base;
  struct buffer  data;
  int            fh;
};

/**********************************************************************/

static int	 readchar	(struct sinput *);
static size_t	 readblock	(struct sinput *,void *,size_t);
static char	*readline	(struct sinput *);
static int	 in_close	(struct sinput *);

static size_t	 writechar	(struct soutput *,int);
static size_t	 writeblock	(struct soutput *,void *,size_t);
static size_t	 flush		(struct soutput *);
static int	 out_close	(struct soutput *);

static int	 tcp_in_close	(struct sinput  *);
static int	 tcp_out_close	(struct soutput *);

/**********************************************************************/

int TCPSInputOutput(SInput *psi,SOutput *pso,const char *host,int port)
{
  struct hostent     *hp;
  struct sockaddr_in  lsin;
  int                 fh;
  int                 rc;
  
  ddt(psi  != NULL);
  ddt(pso  != NULL);
  ddt(host != NULL);
  ddt(port >  0);
  ddt(port <  65536uL);
  
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
  
  *psi          = FHSInput(fh);
  *pso          = FHSOutput(fh);
  (*psi)->close = tcp_in_close;
  (*pso)->close = tcp_out_close;
  
  return(ERR_OKAY);
}

/************************************************************************/

SInput (FileSInput)(const char *name)
{
  int fh;
  
  ddt(name != NULL);
  
  fh = open(name,O_RDONLY);
  if (fh == -1)
  {
    ddtlog(ddtstream,"$ $","open('%a') = %b",name,strerror(errno));
    return(NULL);
  }
  
  return (FHSInput(fh));
}

/**********************************************************************/

SInput (FHSInput)(int fh)
{
  struct sifile *in;
  
  ddt(fh >= 0);
  
  in                  = MemAlloc(sizeof(struct sifile));
  in->fh              = fh;
  in->base.bytes      = 0;
  in->base.eof        = FALSE;
  in->base.error      = 0;
  in->base.readchar   = readchar;
  in->base.readblock  = readblock;
  in->base.readline   = readline;
  in->base.close      = in_close;
  in->data.data       = MemAlloc(FILE_BUFFER_SIZE);
  in->data.size       = FILE_BUFFER_SIZE;
  in->data.max        = 0;
  in->data.idx        = 0;
  
  return((SInput)in);
}

/*************************************************************************/

static size_t refill(struct sifile *si)
{
  ssize_t rrc;

  ddt(si != NULL);  
  
  si->base.error = 0;
  
  do
  {
    rrc = read(si->fh,si->data.data,si->data.size);
  } while ((rrc == -1) && (errno == EINTR));
  
  if (rrc == -1)
  {
    si->base.error = errno;
    rrc = 0;
  }
  
  if (rrc == 0)
  {
    si->base.eof = TRUE;
    si->data.idx = 0;
    si->data.max = 0;
    return(0);
  }
  
  si->data.idx = 0;
  si->data.max = rrc;
  return(si->data.max);
}

/********************************************************************/
    
static int readchar(struct sinput *me)
{
  struct sifile *si = (struct sifile *)me;
  
  ddt(me != NULL);
  
  if (si->data.idx == si->data.max)
  {
    if (refill(si) == 0)
      return(IEOF);
  }
  
  si->base.bytes++;
  return (si->data.data[si->data.idx++]);
}

/*********************************************************************/

static size_t readblock(struct sinput *me,void *data,size_t size)
{
  struct sifile *si;
  struct buffer *b;
  char          *dest;
  size_t         delta;
  size_t         trans;
  
  ddt(me   != NULL);
  ddt(data != NULL);
  ddt(size >  0);
  
  si    = (struct sifile *)me;
  b     = &si->data;
  trans = 0;
  
  while(size)
  {
    if (b->idx == b->max)
    {
      if (refill(si) == 0)
        return(trans);
    }

    delta = b->max - b->idx;
    
    if (size <= delta)	/* we can do this in one shot */
    {
      memcpy(dest,&b->data[b->idx],size);
      b->idx += size;
      trans  += size;
      
      ddt(b->idx <= b->max);
      
      si->base.bytes += trans;
      return(trans);
    }

    memcpy(dest,&b->data[b->idx],delta);
    
    b->idx += delta;
    trans  += delta;
    dest   += delta;
    size   -= delta;
    ddt(b->idx <= b->max);
  }
  
  ddt(0);
  si->base.bytes += trans;
  return(trans);
}

/************************************************************************/

static char *readline(struct sinput *me)
{
  struct sifile *si;
  struct buffer *b;
  char          *s;
  char          *p;
  size_t         delta;
  size_t         sz;
  size_t         total;

  ddt(me != NULL);
  
  si    = (struct sifile *)me;
  b     = &si->data;
  s     = dup_string("");
  total = 0;
  
  while(1)
  {    
    ddt(b->idx <= b->max);

    if (b->idx == b->max)
    {
      if (refill(si) == 0)
        return(s);
    }
    
    delta = b->max - b->idx;
    p     = memchr(&b->data[b->idx],'\n',delta);
    
    if (p != NULL)
    {
      sz = (size_t)(p - &b->data[b->idx]);
      s  = MemResize(s,total + sz + 1);
      memcpy(&s[total],&b->data[b->idx],sz);
      s[total + sz]   = '\0';
      b->idx         += sz + 1;
      si->base.bytes += (total + sz);
      ddt(b->idx <= b->max);
      return(s);
    }

    s = MemResize(s,total + b->max);
    memcpy(&s[total],b->data,b->max);
    total += b->max;
    b->idx = b->max;
  }
}

/************************************************************************/

static int in_close(struct sinput *me)
{
  struct sifile *si = (struct sifile *)me;
  
  ddt(me != NULL);

  close(si->fh);  
  MemFree(si->data.data);
  MemFree(si);
  return(ERR_OKAY);
}

/*************************************************************************/

SOutput (FileSOutput)(const char *name,int mode)
{
  int fh;
  
  ddt(name != NULL);
  
  fh = open(name,O_WRONLY | mode,0666);
  if (fh == -1)
  {
    ddtlog(ddtstream,"$ o $","open('%a',0%b) = %c",name,mode,strerror(errno));
    return(NULL);
  }
  
  return (FHSOutput(fh));
}

/**********************************************************************/

SOutput (FHSOutput)(int fh)
{
  struct sofile *so;
  
  ddt(fh >= 0);
  
  so                   = MemAlloc(sizeof(struct sofile));
  so->fh               = fh;
  so->base.bytes       = 0;
  so->base.eof         = FALSE;
  so->base.error       = 0;
  so->base.writechar   = writechar;
  so->base.writeblock  = writeblock;
  so->base.writeline   = slow_writeline;
  so->base.flush       = flush;
  so->base.close       = out_close;
  so->data.data        = MemAlloc(FILE_BUFFER_SIZE);
  so->data.size        = FILE_BUFFER_SIZE;
  so->data.max         = FILE_BUFFER_SIZE;
  so->data.idx         = 0;
  
  return((SOutput)so);
}

/*************************************************************************/

static size_t flush(struct soutput *me)
{
  struct sofile *so = (struct sofile *)me;
  char          *data;
  size_t         trans;
  ssize_t        rrc;
  
  ddt(me != NULL);
  
  so->base.error = 0;
  trans          = 0;
  data           = so->data.data;
  
  while(so->data.idx)
  {
    rrc = write(so->fh,data,so->data.idx);
    if (rrc == -1)
    {
      if (errno == EINTR) continue;
      so->base.error = errno;
      rrc = 0;
    }
    
    if (rrc == 0)
    {
      so->base.eof = TRUE;
      return(trans);
    }
    
    so->data.idx -= rrc;
    trans        += rrc;
    data         += rrc;
  }
  
  ddt(so->data.idx == 0);
  return(trans);
}

/******************************************************************/

static size_t writechar(struct soutput *me,int c)
{
  struct sofile *so = (struct sofile *)me;
  
  ddt(me != NULL);
  ddt(c  != IEOF);
  
  if (so->data.idx == so->data.max)
  {
    if (flush(me) < so->data.max)
      return(0);
  }
  
  so->base.bytes++;
  so->data.data[so->data.idx++] = c;
  return(1);
}

/********************************************************************/

static size_t writeblock(struct soutput *me,void *data,size_t size)
{
  struct sofile *so;
  struct buffer *b;
  char          *src;
  size_t         delta;
  size_t         trans;
  
  ddt(me   != NULL);
  ddt(data != NULL);
  ddt(size >  0);
  
  so    = (struct sofile *)me;
  src   = data;
  b     = &so->data;
  trans = 0;
  
  while(size)
  {
    if (b->idx == b->max)
    {
      if (flush(me) < b->max)
        return(trans);
    }
    
    delta = b->max - b->idx;
    
    if (size <= delta)	/* one shot deal */
    {
      memcpy(&b->data[b->idx],src,size);
      b->idx += size;
      trans  += size;
      ddt(b->idx <= b->max);
      so->base.bytes += trans;
      return(trans);
    }
    
    memcpy(&b->data[b->idx],src,delta);
    
    b->idx += delta;
    trans  += delta;
    src    += delta;
    size   -= delta;
    ddt(b->idx <= b->max);
  }
  
  ddt(0);
  so->base.bytes += trans;
  return(trans);
}

/*************************************************************************/

static int out_close(struct soutput *me)
{
  struct sofile *so = (struct sofile *)me;
  
  ddt(me != NULL);
  
  MemFree(so->data.data);
  MemFree(so);
  return(ERR_OKAY);
}

/***************************************************************************/

static int tcp_in_close(struct sinput *me)
{
  struct sifile *si = (struct sifile *)me;
  
  ddt(me != NULL);
  
  shutdown(si->fh,SHUT_RD);
  MemFree(si->data.data);
  MemFree(si);
  return(ERR_OKAY);
}

/*************************************************************************/

static int tcp_out_close(struct soutput *me)
{
  struct sofile *so = (struct sofile *)me;
  
  ddt(me != NULL);
  
  shutdown(so->fh,SHUT_WR);
  MemFree(so->data.data);
  MemFree(so);
  return(ERR_OKAY);
}

/***************************************************************************/

