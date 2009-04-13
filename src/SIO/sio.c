




#define min(a,b)	(a) < (b) ? (a) : (b)


/***************************************************************/

size_t SIOCopy(SOutput dest,SInput src)
{
  size_t amount = 0;
  
  while(!SIEof(src))
  {
    struct blockdata data;
    
    data    = SIReadBlock(src);
    amount += SOWriteBlock(dest,data);
    SIReadUpdate(src,data);
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
    
    data       = SIReadBlock(src);
    data.size  = min(data.size,size);
    amount    += SOWriteBlock(dest,data);
    SIReadUpdate(src,data);
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
  
  assert(si != NULL);
  
  result.size = 0;
  result.data = NULL;
  return result;
}

/**********************************************************************/

static struct blockdata sinull_readstr(SInput si)
{
  struct blockdata result;
  
  assert(si != NULL);
  
  result.size = 0;
  result.data = "";
  return result;
}

/*********************************************************************/

static int sinull_rewind(SInput si)
{
  assert(si != NULL);
  
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
  
  assert(size >= sizeof(struct soutput));
  
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
  assert(so != NULL);
  
  if (c == IEOF)
  {
    so->eof = TRUE;
    return 0;
  }
  
  so->size++;
  return 1;
}

/*********************************************************************/

static size_t sonull_writeblock(SOutput so,struct blockdata data)
{
  assert(so        != NULL);
  assert(data.data != NULL);
  assert(data.size >  0);
  
  so->size += data.size;
  return size;
}

/**********************************************************************/

static size_t sonull_writestr(SOutput so,const char *src)
{
  assert(so  != NULL);
  assert(src != NULL);
  
  size = strlen(src);
  so->size += size;
  return size;
}

/**********************************************************************/

static int sonull_rewind(SOutput so)
{
  assert(so != NULL);
  
  so->f.eof = FALSE;
  return 0;
}

/******************************************************************/

static int sonull_close(SOutput so)
{
  assert(so != NULL);
  
  MemFree(so);
  return 0;
}

/*******************************************************************/

