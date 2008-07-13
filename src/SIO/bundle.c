
#include "../sio.h"
#include "../memory.h"
#include "../ddt.h"
#include "../errors.h"
#include "../types.h"

#define BUNDLE_BUMP	8

/*****************************************************/

struct sibuffer
{
  SInput *data;
  size_t  size;		/* max size of buffer */
  size_t  max;		/* max entry used */
  size_t  idx;		/* current entry */
};

struct sibundle
{
  struct sinput   base;
  struct sibuffer data;
};

struct sobuffer
{
  SOutput *data;
  size_t   size;
  size_t   max;
  size_t   idx;
};

struct sobundle
{
  struct soutput  base;
  struct sobuffer data;
};

/******************************************************/

static int	readchar	(struct sinput *);
static int	in_close	(struct sinput *);

/******************************************************/

SInput (BundleSInput)(void)
{
  struct sibundle *si;
  
  si                 = MemAlloc(sizeof(struct sibundle));
  si->base.bytes     = 0;
  si->base.eof       = TRUE;
  si->base.error     = 0;
  si->base.readchar  = readchar;
  si->base.readblock = slow_readblock;
  si->base.readline  = slow_readline;
  si->base.close     = in_close;
  si->data.data      = MemAlloc(BUNDLE_BUMP * sizeof(SInput));
  si->data.size      = BUNDLE_BUMP;
  si->data.max       = 0;
  si->data.idx       = 0;
  SIEoln((SInput)si,"\n");
  return((SInput)si);
}

/********************************************************/

void (BundleSInputAdd)(SInput me,SInput add)
{
  struct sibundle *si = (struct sibundle *)me;
  
  ddt(b   != NULL);
  ddt(add != NULL);
  
  if (si->data.max == si->data.size)
  {
    si->data.size += BUNDLE_BUMP;
    si->data.data  = MemResize(si->data.data,si->data.size * sizeof(SInput));
  }
  
  si->data.data[si->data.max++] = add;
  si->base.eof                  = FALSE;
}

/**********************************************************/

static int readchar(struct sinput *me)
{
  struct sibundle *si;
  struct sibuffer *b;
  int              c;
  
  ddt(me != NULL);
  
  si = (struct sibundle *)me;
  b  = &si->data;
  
  if (si->base.eof) return(IEOF);
  
  while(1)
  {
    if (b->data[b->idx]->eof)
    {
      b->idx++;
      if (b->idx == b->max)
      {
        si->base.eof = TRUE;
        return(IEOF);
      }
      continue;
    }
    
    c = SIChar(b->data[b->idx]);
    if (c == IEOF)
      continue;
    
    return(c);
  }
  
  ddt(0);
  return(IEOF);
}

/*****************************************************************/

static int in_close(struct sinput *me)
{
  struct sibundle *si = (struct sibundle *)me;
  
  ddt(me != NULL);
  
  MemFree(si->data.data);
  MemFree(si);
  return(ERR_OKAY);
}

