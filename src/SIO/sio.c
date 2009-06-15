
#include <string.h>

#include "../types.h"
#include "../memory.h"
#include "../ddt.h"
#include "../sio.h"

#define min(a,b)	(a) < (b) ? (a) : (b)

/**************************************************************/

static int		sinull_read		(SInput);
static struct blockdata	sinull_readblock	(SInput);
static struct blockdata sinull_readstr		(SInput);
static int		sinull_rewind		(SInput);
static int		sinull_close		(SInput);

static size_t		sonull_write		(SOutput,int);
static size_t		sonull_writeblock	(SOutput,struct blockdata);
static size_t		sonull_writestr		(SOutput,const char *);
static int		sonull_rewind		(SOutput);
static int		sonull_close		(SOutput);

/***************************************************************/

size_t SIOCopy(SOutput dest,SInput src)
{
  size_t amount = 0;
  
  while(!SIEof(src))
  {
    struct blockdata data;
    
    data    = SIBlock(src);
    amount += SOBlock(dest,data);
    SIUpdate(src,data);
  }
  return amount;
}

/****************************************************************/

size_t SIOCopyN(SOutput dest,SInput src,size_t size)
{
  size_t amount = 0;
  
  while(!SIEof(src) && (size > 0))
  {
    struct blockdata data;
    
    data       = SIBlock(src);
    data.size  = min(data.size,size);
    amount    += SOBlock(dest,data);
    SIUpdate(src,data);
    size -= data.size;
  }
  return amount;
}

/****************************************************************/

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

SOutput SOutputNew(size_t size)
{
  SOutput so;
  
  ddt(size >= sizeof(struct soutput));
  
  so             = MemAlloc(size);
  so->write      = sonull_write;
  so->writeblock = sonull_writeblock;
  so->writestr   = sonull_writestr;
  so->rewind     = sonull_rewind;
  so->close      = sonull_close;
  so->f.eof      = FALSE;
  so->f.err      = FALSE;
  so->err        = 0;
  so->size       = 0;
  
  return so;
}

/********************************************************************/

static size_t sonull_write(SOutput so,int c)
{
  ddt(so != NULL);
  
  if (c == IEOF)
  {
    so->f.eof = TRUE;
    return 0;
  }
  
  so->size++;
  return 1;
}

/*********************************************************************/

static size_t sonull_writeblock(SOutput so,struct blockdata data)
{
  ddt(so        != NULL);
  ddt(data.data != NULL);
  ddt(data.size >  0);
  
  so->size += data.size;
  return data.size;
}

/**********************************************************************/

static size_t sonull_writestr(SOutput so,const char *src)
{
  size_t size;
  ddt(so  != NULL);
  ddt(src != NULL);
  
  size = strlen(src);
  so->size += size;
  return size;
}

/**********************************************************************/

static int sonull_rewind(SOutput so)
{
  ddt(so != NULL);
  
  so->f.eof = FALSE;
  return 0;
}

/******************************************************************/

static int sonull_close(SOutput so)
{
  ddt(so != NULL);
  
  MemFree(so);
  return 0;
}

/*******************************************************************/

