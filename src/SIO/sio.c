
#include <stdio.h>

#include "../sio.h"
#include "../memory.h"
#include "../types.h"
#include "../util.h"
#include "../ddt.h"

#define min(a,b)	((a) < (b)) ? (a) : (b)

/****************************************************************/

static int	 readchar	(struct sinput *);
static size_t	 readblock	(struct sinput *,void *,size_t);
static char	*readline	(struct sinput *);
static int	 in_close	(struct sinput *);

static size_t 	 writechar	(struct soutput *,int);
static size_t	 writeblock	(struct soutput *,void *,size_t);
static size_t	 writeline	(struct soutput *,const char *);
static size_t	 flush		(struct soutput *);
static int	 out_close	(struct soutput *);

/***********************************************************/

size_t (SIOCopy)(SOutput so,SInput si)
{
  size_t trans;
  size_t amount;
  char   buffer[BUFSIZ];
  
  ddt(so != NULL);
  ddt(si != NULL);
  
  for (trans = 0 ; ; )
  {
    amount = SIBlock(si,buffer,BUFSIZ);
    if (amount == 0) break;
    SOBlock(so,buffer,amount);
    trans += amount;
  }
  
  return(trans);
}

/*******************************************************************/  

size_t (SIOCopyN)(SOutput so,SInput si,size_t size)
{
  size_t trans;
  size_t amount;
  char   buffer[BUFSIZ];
  
  for (trans = 0 ; size > 0 ; )
  {
    amount = SIBlock(si,buffer,min(size,sizeof(buffer));
    if (amount == 0) break;
    SOBlock(so,buffer,amount);
    trans += amount;
    size  -= amount;
  }
  
  return(trans);
}

/********************************************************************/
    
SInput (NullSInput)(void)
{
  SInput in;
  
  in            = MemAlloc(sizeof(struct sinput));
  in->readchar   = readchar;
  in->readblock  = readblock;
  in->readline   = readline;
  in->close      = in_close;
  in->eof        = TRUE;
  in->bytes      = 0;
  return(in);
}

/**********************************************************/

int (SIChar)(SInput in)
{
  ddt(in != NULL);
  
  return ((*in->readchar)(in));
}

/**********************************************************/

size_t (SIBlock)(SInput in,void *data,size_t size)
{
  ddt(in != NULL);
  ddt(data != NULL);
  ddt(size >  0);
  
  return ((*in->readblock)(in,data,size));
}

/***********************************************************/

char *(SILine)(SInput in)
{
  ddt(in != NULL);
  
  return((*in->readline)(in));
}

/************************************************************/

int (SIEof)(SInput in)
{
  ddt(in != NULL);
  
  return(in->eof);
}

/*************************************************************/

int (SIFree)(SInput in)
{
  ddt(in != NULL);
  return ((*in->close)(in));
}

/**********************************************************/

static int readchar(struct sinput *me)
{
  ddt(me != NULL);
  
  return(IEOF);
}

/**********************************************************/

static size_t readblock(struct sinput *me,void *data,size_t size)
{
  ddt(me   != NULL);
  ddt(data != NULL);
  ddt(size >  0);
  
  return(0);
}

/***********************************************************/

static char *readline(struct sinput *me)
{
  ddt(me != NULL);
  
  return(dup_string(""));
}

/************************************************************/

static int in_close(struct sinput *me)
{
  ddt(me != NULL);

  MemFree(me);
  return(ERR_OKAY);
}

/*************************************************************/

SOutput (NullSOutput)(void)
{
  SOutput out;
  
  out              = MemAlloc(sizeof(struct soutput));
  out->writechar   = writechar;
  out->writeblock  = writeblock;
  out->writeline   = writeline;
  out->flush       = flush;
  out->close       = out_close;
  out->eof         = FALSE;
  out->bytes       = 0;
  return(out);
}

/*******************************************************************/

size_t (SOChar)(SOutput out,int c)
{
  ddt(out != NULL);
  ddt(c   != IEOF);
  
  return ((*out->writechar)(out,c));
}

/*******************************************************************/

size_t (SOBlock)(SOutput out,void *data,size_t size)
{
  ddt(out  != NULL);
  ddt(data != NULL);
  ddt(size >  0);
  
  return ((*out->writeblock)(out,data,size));
}

/*****************************************************************/

size_t (SOLine)(SOutput out,char *s)
{
  ddt(out != NULL);
  ddt(s   != NULL);
  
  return((*out->writeline)(out,s));
}

/*****************************************************************/

size_t (SOFlush)(SOutput out)
{
  ddt(out != NULL);
  
  return ((*out->flush)(out));
}

/******************************************************************/

int (SOEof)(SOutput out)
{
  ddt(out != NULL);
  
  return(out->eof);
}

/*******************************************************************/

int (SOFree)(SOutput out)
{
  ddt(out != NULL);
  return ((*out->close)(out));
}

/*********************************************************************/

static size_t writechar(struct soutput *me,int c)
{
  ddt(me != NULL);
  ddt(c  != IEOF);
  
  me->bytes++;
  return(1);
}

/*********************************************************************/

static size_t writeblock(struct soutput *me,void *data,size_t size)
{
  ddt(me   != NULL);
  ddt(data != NULL);
  ddt(size >  0);
  
  me->bytes += size;
  return(size);
}

/***********************************************************************/

static size_t writeline(struct soutput *me,const char *s)
{
  size_t sz;
  
  ddt(me != NULL);
  ddt(s  != NULL);
  
  sz = strlen(s);
  me->bytes += sz;
  return(sz);
}

/**********************************************************************/

static size_t flush(struct soutput *me)
{
  ddt(me != NULL);
  
  return(me->bytes);
}

/*************************************************************************/

static int out_close(struct soutput *me)
{
  ddt(me != NULL);
  MemFree(me); 
  return(ERR_OKAY);
}

/***********************************************************************
* see comment in sio.h for explaination of these routines
************************************************************************/

size_t slow_readblock(struct sinput *me,void *data,size_t size)
{
  size_t  trans;
  char   *p;
  int     c;
  
  for (trans = 0 , p = data ; size ; size--)
  {
    c = (*me->readchar)(me);
    if (c == IEOF)
    {
      me->eof = TRUE;
      return(trans);
    }
    
    *p++ = c;
  }
  return(trans);
}

/**********************************************************************/

char *slow_readline(struct sinput *me)
{
  return(NULL);
}

/***********************************************************************/

size_t slow_writeblock(struct soutput *me,void *data,size_t size)
{
  size_t  trans;
  char   *p;
  char    c;
  int     rc;
  
  ddt(me   != NULL);
  ddt(data != NULL);
  ddt(size >  0);
  
  for (trans = 0 , p = data ; size ; size--)
  {
    c = *p++;
    rc = (*me->writechar)(me,c);
    if (rc == 0)
    {
      me->eof = TRUE;
      return(trans);
    }
  }
  return(trans);
}

/***********************************************************************/

size_t slow_writeline(struct soutput *me,const char *s)
{
  size_t len;
  
  ddt(me != NULL);
  ddt(s  != NULL);
  
  len = strlen(s);
  if (len)
    return((*me->writeblock)(me,(void *)s,len));
  else
    return(0);
}

/************************************************************************/

