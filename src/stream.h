/************************************************************************
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
*************************************************************************/

#ifndef STREAM_H
#define STREAM_H

/************************************************************************/

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#define IEOF		-1
#define ENTITY_NOAMP	 0
#define ENTITY_AMP	 1

#define FHBUFFER_SIZE   8192
#define FHBUFFER_WBSIZE   64

#if defined(__unix__) || defined(__MACH__)
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>

#  define FILE_CREATE		O_CREAT
#  define FILE_EXCLUSIVE	O_EXCL
#  define FILE_TRUNCATE		O_TRUNC
#  define FILE_APPEND		O_APPEND
#  define FILE_RECREATE		(O_CREAT | O_TRUNC)

#  ifndef BUFFER_H
#    define MODE_READ		O_RDONLY
#    define MODE_WRITE		O_WRONLY
#  endif

#else
#  error Please define your OS
#endif

#include "nodelist.h"

/************************************************************************/

struct stream;

struct streamvector
{
  Node   node;
  void  *user;
  int  (*readwrite)  ();	/* warning on this */
  int  (*unrw)       ();	/* warning on this */
  int  (*refillflush)(struct stream *,struct streamvector *);
  int  (*close)      (struct stream *,struct streamvector *);
};

typedef struct stream
{
  int                  type;
  int                  eof;
  struct streamvector  calls;
  List                 mods;
  unsigned char       *data;
  size_t               size;
  size_t               off;
} *Stream;

struct fhmod
{
  char           *name;
  int             fh;
  void           *data;
  off_t           rsize;
  int           (*lowwrite)();
  int           (*prevread)();
  unsigned char   wb[FHBUFFER_WBSIZE];
  size_t          wbsize;
};

/***********************************************************************/

extern Stream StdinStream;
extern Stream StdoutStream;
extern Stream StderrStream;

int		 (StreamInit)		(void);
Stream		 (MemoryStreamRead)	(void *,size_t);
Stream		 (MemoryStreamWrite)	(void *,size_t);
Stream		 (FHStreamRead)		(int);
Stream		 (FHStreamWrite)	(int);
Stream		 (FileStreamRead)	(const char *);
Stream		 (FileStreamWrite)	(const char *,int);
Stream		 (StreamNewRead)	(void);
Stream		 (StreamNewWrite)	(void);
int		 (TCPStream)		(Stream [2],char *,unsigned short);
Stream		 (StringStreamWrite)	(void);
Stream		 (EntityStreamRead)	(Stream,int);
Stream		 (EntityStreamWrite)	(Stream);
Stream		 (TeeStreamWrite)	(Stream,Stream);
Stream		 (BundleStreamRead)	(void);

int		 (StreamRead)		(Stream);
int		 (StreamUnRead)		(Stream,int);
int		 (StreamRefill)		(Stream);
int		 (StreamWrite)		(Stream,int);
int		 (StreamUnWrite)	(Stream);
int		 (StreamFlush)		(Stream);
int		 (StreamEOF)		(Stream);
size_t		 (StreamCopy)		(Stream,Stream);
size_t		 (StreamCopyN)		(Stream,Stream,size_t);
int		 (StreamFree)		(Stream);

size_t		 (LineS)		(const Stream,const char *);
size_t		 (LineSFormat)		(const Stream,const char *,const char *, ... );
size_t		 (LineSFormatv)		(const Stream,const char *,const char *,va_list);
char		*(LineSRead)		(const Stream);
int		 (Line_ReadChar)	(const Stream);
char		*(StringFromStream)	(const Stream);
void		 (BundleReadAdd)	(const Stream,const Stream);

/***********************************************************************/

#ifdef SCREAM
#  define StreamRead(s)		(*((s)->calls.readwrite))  ((s),&(s)->calls)
#  define StreamUnRead(s,c)	(*((s)->calls.unrw))       ((s),&(s)->calls,(c))
#  define StreamRefill(s)	(*((s)->calls.refillflush))((s),&(s)->calls)
#  define StreamWrite(s,c)	(*((s)->calls.readwrite))  ((s),&(s)->calls,(c))
#  define StreamUnWrite(s)	(*((s)->calls.unrw))       ((s),&(s)->calls)
#  define StreamFlush(s)	(*((s)->calls.refillflush))((s),&(s)->calls)
#  define StreamEOF(s)		((s)->eof)
#endif

#endif

