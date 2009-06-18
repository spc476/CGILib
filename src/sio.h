/************************************************************************
*
* Copyright 2009 by Sean Conner.  All Rights Reserved.
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
*************************************************************************/

#ifndef SIO_H
#define SIO_H

/************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define IEOF		-1

/*********************************************************************/

struct blockdata
{
  size_t  size;
  void   *data;
};

typedef struct sinput
{
  int              (*read)       (struct sinput *);
  struct blockdata (*readblock)  (struct sinput *);
  struct blockdata (*readstr)    (struct sinput *);
  int              (*readreturn) (struct sinput *,struct blockdata);
  int              (*rewind)     (struct sinput *);
  int              (*close)      (struct sinput *);
  struct
  {
    unsigned int eof  : 1;
    unsigned int err  : 1;
  } f;
  int    err;
  size_t size;
} *SInput;

typedef struct soutput
{
  size_t (*write)     (struct soutput *,int);
  size_t (*writeblock)(struct soutput *,struct blockdata);
  size_t (*writestr)  (struct soutput *,const char *);
  int    (*rewind)    (struct soutput *);
  int    (*close)     (struct soutput *);
  struct
  {
    unsigned int eof : 1;
    unsigned int err : 1;
  } f;
  int    err;
  size_t size;
} *SOutput;

typedef SOutput SOStringT;

struct mem_sinput
{
  struct sinput  base;
  const char    *data;
  size_t         max;
  size_t         idx;
};

/**************************************************************/

extern SInput	Stdin;
extern SOutput	Stdout;
extern SOutput	Stderr;

SInput		 SInputNew	(size_t);
SInput		 MemorySInput	(void *,size_t);
SInput		 FileSInput	(const char *);
SInput		 FHSInput	(int);
SInput		 BundleSInput	(void);

SOutput		 SOutputNew	(size_t);
SOutput		 FileSOutput	(const char *,int);
SOutput		 FHSOutput	(int);
SOStringT	 StringSOutput	(void);
SOutput		 TeeSOutput	(SOutput,SOutput);

size_t		 SOFormat	(SOutput,const char *,const char *, ... );
size_t		 SOFormatv	(SOutput,const char *,const char *,va_list);
size_t		 SIOCopy	(SOutput,SInput);
size_t		 SIOCopyN	(SOutput,SInput,size_t);
struct blockdata SOToString	(SOStringT);

/************************************************************/

static inline size_t SInputSize(SInput si)
{
  return si->size;
}

static inline int SIEof(SInput si)
{
  return si->f.eof;
}

static inline int SIErr(SInput si)
{
  return si->f.err;
}

static inline int SIErrCode(SInput si)
{
  return si->err;
}

static inline void SIErrClear(SInput si)
{
  si->f.err = FALSE;
  si->err   = 0;
}

static inline int SIChar(SInput si)
{
  return (*si->read)(si);
}

static inline struct blockdata SIBlock(SInput si)
{
  return (*si->readblock)(si);
}

static inline struct blockdata SIString(SInput si)
{
  return (*si->readstr)(si);
}

static inline int SIUpdate(SInput si,struct blockdata data)
{
  return (*si->readreturn)(si,data);
}

static inline int SIRewind(SInput si)
{
  return (*si->rewind)(si);
}

static inline int SIFree(SInput si)
{
  return (*si->close)(si);
}

static inline size_t SOutputSize(SOutput so)
{
  return so->size;
}

static inline int SOEof(SOutput so)
{
  return so->f.eof;
}

static inline int SOErr(SOutput so)
{
  return so->f.err;
}

static inline int SOErrCode(SOutput so)
{
  return so->err;
}

static inline int SOErrClear(SOutput so)
{
  so->f.err = FALSE;
  so->err   = 0;
  return 0;
}

static inline size_t SOChar(SOutput so,int c)
{
  return (*so->write)(so,c);
}

static inline size_t SOBlock(SOutput so,struct blockdata data)
{
  return (*so->writeblock)(so,data);
}

static inline size_t SOString(SOutput so,const char *s)
{
  return (*so->writestr)(so,s);
}

static inline int SORewind(SOutput so)
{
  return (*so->rewind)(so);
}

static inline int SOFree(SOutput so)
{
  return (*so->close)(so);
}

/***********************************************************************/

#endif

