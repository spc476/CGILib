
#include <string.h>

#include "../types.h"
#include "../ddt.h"
#include "../sio.h"

/******************************************************************/

static int		simem_read		(SInput);
static struct blockdata	simem_readblock		(SInput);
static struct blockdata	simem_readstr		(SInput);
static int		simem_readreturn	(SInput,struct blockdata);
static int		simem_rewind		(SInput);

/********************************************************************/

SInput MemorySInput(void *data,size_t size)
{
  struct mem_sinput *si;
  
  si = (struct mem_sinput *)SInputNew(sizeof(struct mem_sinput));
  si->base.read       = simem_read;
  si->base.readblock  = simem_readblock;
  si->base.readstr    = simem_readstr;
  si->base.readreturn = simem_readreturn;
  si->base.rewind     = simem_rewind;
  si->data            = data;
  si->max             = size;
  si->idx             = 0;
  
  return (SInput)si;
}

/*****************************************************************/

static int simem_read(SInput me)
{
  struct mem_sinput *si = (struct mem_sinput *)me;
  
  ddt(me != NULL);
  
  if (si->idx == si->max)
  {
    si->base.f.eof = TRUE;
    return IEOF;
  }
  
  si->base.size++;
  return si->data[si->idx++];
}

/*****************************************************************/

static struct blockdata simem_readblock(SInput me)
{
  struct mem_sinput *si = (struct mem_sinput *)me;
  struct blockdata   result;

  if (si->base.f.eof)
  {
    result.size = 0;
    result.data = NULL;
    return result;
  }

  result.size = si->max - si->idx;
  result.data = (void *)&si->data[si->idx];
  return result;
}

/*******************************************************************/

static struct blockdata simem_readstr(SInput me)
{
  struct mem_sinput *si = (struct mem_sinput *)me;
  struct blockdata   result;
  char              *p;

  if (si->base.f.eof)
  {
    result.size = 0;
    result.data = "";
    return result;
  }

  result.size = si->max - si->idx;
  result.data = (void *)&si->data[si->idx];

  p = memchr(result.data,'\n',result.size);
  if (p)
    result.size = (size_t)(p - (char *)result.data);
  return result;
}

/********************************************************************/

static int simem_readreturn(SInput me,struct blockdata data)
{
  struct mem_sinput *si = (struct mem_sinput *)me;
  
  ddt(me        != NULL);
  ddt(data.size >  0);
  ddt(data.data != NULL);
  
  si->idx += data.size;
  ddt(si->idx <= si->max);
  return 0;
}

/********************************************************************/

static int simem_rewind(SInput me)
{
  struct mem_sinput *si = (struct mem_sinput *)me;
  
  ddt(me != NULL);
  
  si->base.size  = 0;
  si->base.err   = 0;
  si->base.f.eof = FALSE;
  si->base.f.err = FALSE;
  si->idx        = 0;
  return 0;
}

/********************************************************************/

