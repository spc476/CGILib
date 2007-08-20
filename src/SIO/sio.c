

#include "../sio.h"
#include "../memory.h"
#include "../types.h"
#include "../util.h"
#include "../ddt.h"

/****************************************************************/

static int	 readchar	(struct sinput *);
static size_t	 readblock	(struct sinput *,void *,size_t);
static char	*readstring	(struct sinput *);
static int	 unread		(struct sinput *,int);
static int	 in_close	(struct sinput *);

static size_t 	 writechar	(struct soutput *,int);
static size_t	 writeblock	(struct soutput *,void *,size_t);
static size_t	 writestring	(struct soutput *,const char *);
static size_t	 flush		(struct soutput *);
static int	 out_close	(struct soutput *);

/***********************************************************/

SInput (NullSInput)(void)
{
  SInput in;
  
  in            = MemAlloc(sizeof(struct sinput));
  in->readchar   = readchar;
  in->readblock  = readblock;
  in->readstring = readstring;
  in->unread     = unread;
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

char *(SIString)(SInput in)
{
  ddt(in != NULL);
  
  return((*in->readstring)(in));
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
  int rc;
  
  ddt(in != NULL);
  
  rc = (*in->close)(in);
  if (rc != ERR_OKAY)
    return(rc);
  
  MemFree(in);
  return(ERR_OKAY);
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

static char *readstring(struct sinput *me)
{
  ddt(me != NULL);
  
  return(dup_string(""));
}

/************************************************************/

static int unread(struct sinput *me,int c)
{
  ddt(me != NULL);
  ddt(c  != IEOF);
  
  return(c);
}

/************************************************************/

static int in_close(struct sinput *me)
{
  ddt(me != NULL);
  
  return(ERR_OKAY);
}

/*************************************************************/

SOutput (NullSOutput)(void)
{
  SOutput out;
  
  out              = MemAlloc(sizeof(struct soutput));
  out->writechar   = writechar;
  out->writeblock  = writeblock;
  out->writestring = writestring;
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

size_t (SOString)(SOutput out,char *s)
{
  ddt(out != NULL);
  ddt(s   != NULL);
  
  return((*out->writestring)(out,s));
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
  int rc;
  
  ddt(out != NULL);
  
  rc = (*out->close)(out);
  if (rc != ERR_OKAY)
    return(rc);
  
  MemFree(out);
  return(ERR_OKAY);
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

static size_t writestring(struct soutput *me,const char *s)
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
  
  return(ERR_OKAY);
}

/**************************************************************************/

