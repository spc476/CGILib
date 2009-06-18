
#include <string.h>

#include "../types.h"
#include "../memory.h"
#include "../ddt.h"
#include "../sio.h"

/**************************************************************/

static int		sinull_read		(SInput);
static struct blockdata	sinull_readblock	(SInput);
static struct blockdata sinull_readstr		(SInput);
static int		sinull_rewind		(SInput);
static int		sinull_close		(SInput);

/***************************************************************/

SInput SInputNew(size_t size)
{
  SInput si;
  
  ddt(size >= sizeof(struct sinput));
  
  si            = MemAlloc(size);
  si->read      = sinull_read;
  si->readblock = sinull_readblock;
  si->readstr   = sinull_readstr;
  si->rewind    = sinull_rewind;
  si->close     = sinull_close;
  si->f.eof     = TRUE;
  si->f.err     = FALSE;
  si->err       = 0;
  si->size      = 0;
  return si;
}

/*********************************************************************/

static int sinull_read(SInput si)
{
  ddt(si != NULL);
  
  return IEOF;
}

/**********************************************************************/

static struct blockdata sinull_readblock(SInput si)
{
  struct blockdata result;
  
  ddt(si != NULL);
  
  result.size = 0;
  result.data = NULL;
  return result;
}

/**********************************************************************/

static struct blockdata sinull_readstr(SInput si)
{
  struct blockdata result;
  
  ddt(si != NULL);
  
  result.size = 0;
  result.data = "";
  return result;
}

/*********************************************************************/

static int sinull_rewind(SInput si)
{
  ddt(si != NULL);
  
  return 0;
}

/*********************************************************************/

static int sinull_close(SInput si)
{
  ddt(si != NULL);
  
  MemFree(si);
  return 0;
}

/********************************************************************/

