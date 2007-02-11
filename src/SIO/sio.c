/**************************************************************************
*
* Copyright 2001 by Sean Conner.  All Rights Reserved.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
* Comments, questions and criticisms can be sent to: sean@conman.org
*
**************************************************************************/

#include "../memory.h"
#include "../ddt.h"
#include "../errors.h"
#include "../types.h"
#include "../sio.h"

/************************************************************************/

static int null_rread		(struct sior *);
static int null_runread		(struct sior *,int);
static int null_rrefill		(struct sior *);
static int null_rmap		(struct sior *);
static int null_rclose		(struct sior *);

static int null_wwrite		(struct siow *,int);
static int null_wunwrite	(struct siow *);
static int null_wflush		(struct siow *);
static int null_wclose		(struct siow *);

/************************************************************************/

int (SIOInit)(void)
{
  SIOStdin  = SIORFH(0);
  SIOStdout = SIOWFH(1);
  SIOStderr = SIOWFH(2);
  
  return(
  	(SIOStdin != NULL)
  	&& (SIOStdout != NULL)
  	&& (SIOStderr != NULL)
  );  
}

/*************************************************************************/

int (SIOREOF)(SIOR r)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);
  
  return (r->eof);
}

/*************************************************************************/

size_t (SIORSize)(SIOR r)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);
  
  return(r->size);
}

/*************************************************************************/

int (SIORRead)(SIOR r)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);
  
  return ((*r->read)(r));
}

/***********************************************************************/

int (SIORUnread)(SIOR r,int c)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);
  
  return((*r->unread)(r,c));
}

/***********************************************************************/

int (SIORRefill)(SIOR r)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);
  
  return ((*r->refill)(r));
}

/***********************************************************************/

int (SIORMap)(SIOR r)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);
  
  return ((*r->map)(r));
}

/************************************************************************/

int (SIORClose)(SIOR r)
{
  int rc;
  
  ddt(r
  ddt(r->dir == SIO_READ);
  
  rc = (*r->close)(r);
  if (rc != ERR_OKAY)
    return (rc);
  MemFree(r);
  return(ERR_OKAY);
}

/**********************************************************************/

int (SIOWEOF)(SIOW w)
{
  ddt(w      != NULL);
  ddt(w->dir == SIO_WRITE);
  
  return (w->eof);
}

/**********************************************************************/

int (SIOWSize)(SIOW w)
{
  ddt(w      != NULL);
  ddt(w->dir == SIO_WRITE);
  
  return (w->size);
}

/**********************************************************************/

int (SIOWWrite)(SIOW w,int c)
{
  ddt(w      != NULL);
  ddt(w->dir == SIO_WRITE);
  
  return ((*w->write)(w,c));
}

/*********************************************************************/

int (SIOWUnwrite)(SIOW w)
{
  ddt(w      != NULL);
  ddt(w->dir == SIO_WRITE);
  
  return ((*w->unwrite)(w));
}

/*********************************************************************/

int (SIOWFlush)(SIOW w)
{
  ddt(w      != NULL);
  ddt(w->dir == SIO_WRITE);
  
  return ((*w->flush)(w));
}

/*********************************************************************/

int (SIOWClose)(SIOW w)
{
  int rc;
  
  ddt(w      != NULL);
  ddt(w->dir == SIO_WRITE);
  
  rc = (*w->close)(w);
  if (rc != ERR_OKAY)
    return (rc);
  MemFree(w);
  return(ERR_OKAY);
}

/**********************************************************************/

size_t SIOCopy(SIOW out,SIOR in)
{
  size_t amount = 0;
  int    c;
  
  while(!SIOREOF(in))
  {
    c = SIORRead(in);
    if (c == IEOF) break;
    SIOWWrite(out,c);
    amount++;
  }
  return(amount);
}

/*************************************************************************/

SIOR (SIORNew)(size_t size)
{
  SIOR s;
  
  ddt(size >= sizeof(struct sior));
  
  s         = MemAlloc(size);
  s->dir    = SIO_READ;
  s->type   = SIOT_NULL;
  s->read   = null_rread;
  s->unread = null_runread;
  s->refill = null_rrefill;
  s->map    = null_rmap;
  s->close  = null_rclose;
  s->feof   = TRUE;
  s->size   = 0;
  s->off    = 0;
  s->data   = NULL;
  
  return(s);
}

/***************************************************************************/

SIOW (SIOWNew)(size_t size)
{
  SIOW w;
  
  ddt(size >= sizeof(struct siow));
  
  s
  s->dir     = SIO_WRITE;
  s->type    = SIOT_NULL;
  s->write   = null_wwrite;
  s->unwrite = null_wunwrite;
  s->flush   = null_wflush;
  s->close   = null_wclose;
  s->eof     = FALSE;
  s->size    = 0;
  s->off     = 0;
  s->data    = NULL;
  
  return(w);
}

/**************************************************************************/

SIOR (SIORNull)(void)
{
  return (SIORNew(sizeof(struct sior)));
}

/***************************************************************************/

SIOW (SIOWNull)(void)
{
  return (SIOWNew(sizeof(struct siow)));
}

/****************************************************************************/

static int null_rread(struct sior *r)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);
  
  return (IEOF);
}

/******************************************************************************/

static int null_runread(struct sior *r,int c)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);

  return (IEOF);
}

/*******************************************************************************/

static int null_rrefill(struct sior *r)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);

  return(FALSE);
}

/*******************************************************************************/

static int null_rmap(struct sior *r)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);
  
  return(ERR_OKAY);
}

/******************************************************************************/

static int null_rclose(struct sior *r)
{
  ddt(r      != NULL);
  ddt(r->dir == SIO_READ);
  
  return(ERR_OKAY);
}

/******************************************************************************/

static int null_wwrite(struct siow *w,int c)
{
  ddt(w      != NULL);
  ddt(w->dir == SIO_WRITE);
  
  return(c);
}

/*****************************************************************************/

static int null_wunwrite(struct siow *w)
{
  ddt(w      != NULL);
  ddt(w->dir == SIO_WRITE);

  return (IEOF);
}

/******************************************************************************/

static int null_wflush(struct siow *w)
{
  ddt(w      != NULL);
  ddt(w->dir == SIO_WRITE);

  return(ERR_OKAY);
}

/******************************************************************************/

static int null_wclose(struct siow *w)
{
  ddt(w      != NULL);
  ddt(w->dir == SIO_WRITE);
  
  return(ERR_OKAY);
}

/*******************************************************************************/

