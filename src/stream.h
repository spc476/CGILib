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

#ifdef __unix__
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>

#  define FILE_CREATE		O_CREAT
#  define FILE_EXCLUSIVE	O_EXCL
#  define FILE_TRUNCATE		O_TRUNC
#  define FILE_APPEND		O_APPEND
#else
#  error Please define your OS
#endif

#include "nodelist.h"

/************************************************************************/

typedef struct streamin
{
  Node     node;
  int      type;
  int    (*ioctl)       (struct streamin *,int, ... );
  int    (*read)        (struct streamin *,void *,size_t *);
  int    (*unread)      (struct streamin *,void *,size_t *);
  int    (*seekforward) (struct streamin *,size_t *,int);
  int    (*seekbackward)(struct streamin *,size_t *,int);
  int	 (*seekstart)	(struct streamin *);
  int	 (*seekend)	(struct streamin *);
  int    (*refill)      (struct streamin *);
  int    (*close)       (struct streamin *);
  void    *data;
  size_t   size;
  size_t   off;
} *StreamIn;

typedef struct streamout
{
  Node     node;
  int      type;
  int    (*ioctl)       (struct streamout *,int, ... );
  int    (*write)       (struct streamout *,void *,size_t *);
  int    (*unwrite)     (struct streamout *,void *,size_t *);
  int    (*seekforward) (struct streamout *,size_t *);
  int    (*seekbackward)(struct streamout *,size_t *);
  int    (*seekstart)	(struct streamout *);
  int	 (*seekend)	(struct streamout *);
  int    (*flush)       (struct streamout *);
  int    (*close)       (struct streamout *);
  void    *data;
  size_t   size;
  size_t   off;
} *StreamOut;

/***********************************************************************/

StreamIn	(MemoryStreamIn)	(void *,size_t);
StreamIn	(FHStreamIn)		(int);
StreamIn	(FileStreamIn)		(char *);

int		(StreamInIOCtl)		(StreamIn,int, ... );
size_t		(StreamInRead)		(StreamIn,void *,size_t);
size_t		(StreamInUnRead)	(StreamIn,void *,size_t);
size_t		(StreamInPos)		(StreamIn);
size_t		(StreamInSeekForward)	(StreamIn,size_t);
size_t		(StreamInSeekBackward)	(StreamIn,size_t);
size_t		(StreamInSeekStart)	(StreamIn);
size_t		(StreamInSeekEnd)	(StreamIn);
int		(StreamInRefill)	(StreamIn);
int		(StreamInClose)		(StreamIn);

int		(StreamInVIOCtl)	(StreamIn,int, ... );
int		(StreamInVRead)		(StreamIn,void *,size_t *);
int		(StreamInVUnRead)	(StreamIn,void *,size_t *);
int		(StreamInVSeekForward)	(StreamIn,size_t *);
int		(StreamInVSeekBackward)	(StreamIn,size_t *);
int		(StreamInVSeekStart)	(StreamIn);
int		(StreamInVSeekEnd)	(StreamIn);
int		(StreamInVClose)	(StreamIn);

StreamOut	(MemoryStreamOut)	(void *,size_t);
StreamOut	(DynamicStreamOut)	(void);
StreamOut	(FHStreamOut)		(int);
StreamOut	(FileStreamOut)		(char *,int);

int		(StreamOutIOCtl)	(StreamOut,int, ... );
size_t		(StreamOutWrite)	(StreamOut,void *,size_t);
size_t		(StreamOutUnWrite)	(StreamOut,void *,size_t);
size_t		(StreamOutPos)		(StreamOut);
size_t		(StreamOutSeekForward)	(StreamOut,size_t);
size_t		(StreamOutSeekBackward)	(StreamOut,size_t);



int		(StreamInReadr)		(StreamIn,void *,size_t *);
int		(StreamInUnReadr)	(StreamIn,void *,size_t *);

Stream		 (MemoryStreamRead)	(void *,size_t);
Stream		 (MemoryStreamWrite)	(void *,size_t);
Stream		 (FHStreamRead)		(int);
Stream		 (FHStreamWrite)	(int);
Stream		 (FileStreamRead)	(char *);
Stream		 (FileStreamWrite)	(char *,int);

Stream		 (StreamNewRead)	(void);
Stream		 (StreamNewWrite)	(void);

int		 (StreamRead)		(Stream);
int		 (StreamUnRead)		(Stream,int);
int		 (StreamRefill)		(Stream);
int		 (StreamWrite)		(Stream,int);
int		 (StreamUnWrite)	(Stream);
int		 (StreamFlush)		(Stream);
int		 (StreamEOF)		(Stream);
int		 (StreamClose)		(Stream);

size_t		 (LineS)		(const Stream,const char *);
size_t		 (LineSFormat)		(const Stream,const char *,const char *, ... );
size_t		 (LineSFormatv)		(const Stream,const char *,const char *,va_list);
char		*(LineSRead)		(const Stream);

int		 (Line_ReadChar)	(const Stream);

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

