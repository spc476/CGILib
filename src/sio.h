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

#ifndef SIO_H
#define SIO_H

/************************************************************************/

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#include "stream.h"

#ifndef STREAM_H
#  define IEOF		 	  -1
#  define ENTITY_NOAMP	 	   0
#  define ENTITY_AMP	 	   1

#  define FHBUFFER_SIZE   	8192
#  define FHBUFFER_WBSIZE 	  64

#  ifdef __unix__
#    include <sys/types.h>
#    include <sys/stat.h>
#    include <fcntl.h>

#    define FILE_CREATE		O_CREAT
#    define FILE_EXCLUSIVE	O_EXCL
#    define FILE_TRUNCATE	O_TRUNC
#    define FILE_APPEND		O_APPEND
#    define FILE_RECREATE	(O_CREAT | O_TRUNC)

#    ifndef BUFFER_H
#      define MODE_READ		O_RDONLY
#      define MODE_WRITE	O_WRONLY
#    endif

#  else
#    error Please define your OS
#  endif
#endif

typedef enum siodir
{
  SIO_READ,
  SIO_WRITE
} SIODir;

typedef enum siotype
{
  SIOT_NULL,
  SIOT_MEMORY,
  SIOT_STRING,
  SIOT_FH,
  SIOT_FILE,
  SIOT_TOENTITY,
  SIOT_FROMENTITY,
  SIOT_TEE,
  SIOT_TCP
} SIOType;

#include "nodelist.h"

/************************************************************************/

typedef struct sread
{
  SIODir   dir;
  SIOType  type;
  int    (*read)  (struct sread *);
  int    (*unread)(struct sread *,int);
  int	 (*rewind)(struct sread *);
  int    (*refill)(struct sread *);
  int    (*map)   (struct sread *);
  int    (*close) (struct sread *);
  int      feof;
  size_t   size;
  size_t   off;
  char    *data;
} *SRead;

typedef struct swrite
{
  SIODir   dir;
  SIOType  type;
  int    (*write)  (struct swrite *,int);
  int    (*unwrite)(struct swrite *);
  int    (*flush)  (struct swrite *);
  int    (*close)  (struct swrite *);
  int      feof;
  size_t   size;
  size_t   off;
  char    *data;
} *SWrite;

struct siorfh
{
  struct sior     base;
  char           *name;
  int             fh;
  int           (*prevread)();
  unsigned char   wb[FHBUFFER_SIZE];
  size_t          wbsize;
};

struct siowfh
{
  struct siow     base;
  char           *name;
  int             fh;
  int           (*lowwrite)();
  unsigned char   wb[FHBUFFER_SIZE];
  size_t          wbsize;
};

/***********************************************************************/

extern SRead  SRStdin;
extern SWrite SWStdout;
extern SWrite SWStderr;

int		 (SIOInit)		(void);

SRead		 (SReadNew)		(size_t);
SWrite		 (SWriteNew)		(size_t);

SRead		 (SReadNull)		(void);
SWrite	 	 (SWriteNull)		(void);
SRead	 	 (SReadMemory)		(void *,size_t);
SWrite	 	 (SWriteMemory)		(void *,size_t);
SRead	 	 (SReadString)		(const char *);
SWrite		 (SWriteString)		(char *,size_t);
SRead	 	 (SReadFH)		(int);
SWrite		 (SWriteFH)		(int);
SRead	 	 (SReadFile)		(const char *);
SWrite		 (SWriteFile)		(const char *,int);

SRead	 	 (SRreadToEntit)	(SRead);
SRead	 	 (SRreadFromEntity)	(SRead);
SWrite		 (SWwriteToEntity)	(SWrite);
SWrite		 (SWwriteFromEntity)	(SWrite);

SWrite		 (SWriteTee)		(SWrite, ... );
int		 (STCP)			(SRead *,SWrite *,const char *,unsigned short);

int		 (SReadEOF)		(SRead);
size_t		 (SReadSize)		(SRead);
int		 (SRead)		(SRead);
int		 (SReadUndo)		(SRead,int);
int		 (SReadRefill)		(SRead);
int		 (SReadMap)		(SRead);
int		 (SReadClose)		(SRead);

int		 (SWriteEOF)		(SWrite);
size_t		 (SWriteSize)		(SWrite);
int		 (SWriteWrite)		(SWrite,int);
int		 (SWriteUndo)		(SWrite);
int		 (SWriteFlush)		(SWrite);
int		 (SWriteClose)		(SWrite);

size_t		 (SCopy)		(SWrite,SRead);

size_t		 (LineSWrite)		(const SWrite,const char *);
size_t		 (LineSWriteFormat)	(const SWrite,const char *,const char *, ... );
size_t		 (LineSWriteFormatv)	(const SWrite,const char *,const char *,va_list);

char		*(LineSRead)		(const SRead);
char		*(StringFromSWrite)	(const SWrite);

/***********************************************************************************/

#ifdef SCREAM

#  define SIOREOF(sior)		((sior)->eof)
#  define SIORSIZE(sior)	((sior)->size)
#  define SIORRead(sior)	(*((sior)->read))   	((sior))
#  define SIORUnread(sior,c)	(*((sior)->unread)) 	((sior),(c))
#  define SIORRefill(sior)	(*((sior)->refill)) 	((sior))
#  define SIORMap(sior)		(*((sior)->map))    	((sior))

#  define SIOWEOF(siow)		((siow)->eof)
#  define SIOWSize(siow)	((siow)->size)
#  define SIOWWrite(siow,c)	(*((sior)->write)) 	((sior),(c))
#  define SIOWUnwrite(siow)	(*((sior)->unwrite))	((sior))
#  define SIOWFlush(siow)	(*((sior)->flush))	((sior))

#endif

/***********************************************************************/

#endif

