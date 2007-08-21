
#include <stddef.h>
#include <string.h>

#include "../sio.h"
#include "../memory.h"
#include "../types.h"
#include "../util.h"
#include "../ddt.h"

#define min(a,b)	((a) < (b)) ? (a) : (b)

/**********************************************************************/

struct simem
{
  struct sinput base;
  struct buffer data;
};

struct somem
{
  struct soutput base;
  struct buffer  data;
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

/**********************************************************************/

SInput (MemorySInput)(void *data,size_t size)
{
  struct simem *in;
  
  ddt(data != NULL);
  ddt(size >  0);
  
  in                 = MemAlloc(sizeof(struct simem));
  in->base.bytes     = 0;
  in->base.eof       = FALSE;
  in->base.error     = 0;
  in->base.readchar  = readchar;
  in->base.readblock = readblock;
  in->base.readline  = readline;
  in->base.close     = in_close;
  in->data.data      = data;
  in->data.size      = size;
  in->data.max       = size;
  in->data.idx       = 0;
  
  return((SInput)in);
}

/***********************************************************************/

static int readchar(struct sinput *me)
{
  struct simem *si = (struct simem *)me;
  
  ddt(me != NULL);
  
  if (si->data.idx == si->data.max)
  {
    si->base.eof = TRUE;
    return(IEOF);
  }
  
  si->base.bytes++;
  return (si->data.data[si->data.idx++]);
}

/***********************************************************************/

static size_t readblock(struct sinput *me,void *data,size_t size)
{
  struct simem *si = (struct simem *)me;
  size_t        delta;
  
  ddt(me   != NULL);
  ddt(data != NULL);
  ddt(size >  0);
  
  delta = si->data.max - si->data.idx;
  
  if (delta == 0)
  {
    si->base.eof = TRUE;
    return(0);
  }
  
  delta = min(size,delta);
  memcpy(data,&si->data.data[si->data.idx],delta);
  si->data.idx   += delta;
  si->base.bytes += delta;
  return(delta);
}

/*************************************************************************/

static char *readline(struct sinput *me)
{
  struct simem  *si;
  struct buffer *b;
  char          *p;
  char          *s;
  size_t         delta;
  size_t         adjust;
  
  ddt(me != NULL);
  
  si     = (struct simem *)me;
  b      = &si->data;
  adjust = 0;
  delta  = b->max - b->idx;

  if (delta == 0)
  {
    si->base.eof = TRUE;
    return(dup_string(""));
  }

  p = memchr(&b->data[b->idx],'\n',delta);
  
  if (p != NULL)
  {
    adjust = 1;
    delta  = (size_t)(p - &b->data[b->idx]);
  }
  
  s = MemAlloc(delta + 1);
  memcpy(s,&b->data[b->idx],delta);
  s[delta] = '\0';
  b->idx += delta + adjust;
  si->base.bytes += delta;
  return(s);
}

/*************************************************************************/

static int in_close(struct sinput *me)
{
  struct simem *si = (struct simem *)me;
  
  ddt(me != NULL);
  
  MemFree(si->data.data);
  MemFree(si);
  return(ERR_OKAY);
}

/***************************************************************************/

SOutput MemSOutput(void *data,size_t size)
{
  struct somem *so;
  
  ddt(data != NULL);
  ddt(size >  0);
  
  so                  = MemAlloc(sizeof(struct somem));
  so->base.bytes      = 0;
  so->base.eof        = FALSE;
  so->base.error      = 0;
  so->base.writechar  = writechar;
  so->base.writeblock = writeblock;
  so->base.writeline  = slow_writeline;
  so->base.flush      = flush;
  so->base.close      = out_close;
  so->data.data       = data;
  so->data.size       = size;
  so->data.max        = size;
  so->data.idx        = 0;
  
  return((SOutput)so);
}

/***************************************************************************/

static size_t writechar(struct soutput *me,int c)
{
  struct somem *so = (struct somem *)me;
  
  ddt(me != NULL);
  ddt(c  != IEOF);
  
  if (so->data.idx == so->data.max)
  {
    so->base.eof = TRUE;
    return(0);
  }
  
  so->base.bytes++;
  so->data.data[so->data.idx++] = c;
  return(1);
}

/******************************************************************************/

static size_t writeblock(struct soutput *me,void *data,size_t size)
{
  struct somem *so = (struct somem *)me;
  size_t        delta;
  
  ddt(me   != NULL);
  ddt(data != NULL);
  ddt(size >  0);
  
  delta = so->data.max - so->data.idx;
  
  if (delta == 0)
  {
    so->base.eof = TRUE;
    return(0);
  }
  
  delta = min(size,delta);
  memcpy(&so->data.data[so->data.idx],data,delta);
  so->data.idx   += delta;
  so->base.bytes += delta;
  return(delta);
}

/*******************************************************************************/

static size_t flush(struct soutput *me)
{
  ddt(me != NULL);
  
  return(me->bytes);
}

/*********************************************************************************/

static int out_close(struct soutput *me)
{
  struct somem *so = (struct somem *)me;
  
  ddt(me != NULL);
  
  MemFree(so->data.data);
  MemFree(so);
  return(ERR_OKAY);
}

/********************************************************************************/

