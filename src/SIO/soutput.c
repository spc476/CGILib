
#include <string.h>

#include "../types.h"
#include "../memory.h"
#include "../ddt.h"
#include "../sio.h"

/**************************************************************/

static size_t		sonull_write		(SOutput,int);
static size_t		sonull_writeblock	(SOutput,struct blockdata);
static size_t		sonull_writestr		(SOutput,const char *);
static int		sonull_rewind		(SOutput);
static int		sonull_close		(SOutput);

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

