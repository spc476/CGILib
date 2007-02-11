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

typedef struct sior
{
  SIODir   dir;
  SIOType  type;
  int    (*read)  (struct sior *);
  int    (*unread)(struct sior *,int);
  int    (*refill)(struct sior *);
  int    (*map)   (struct sior *);
  int    (*close) (struct sior *);
  int      feof;
  size_t   size;
  size_t   off;
  char    *data;
} *SIOR;

typedef struct siow
{
  SIODir   dir;
  SIOType  type;
  int    (*write)  (struct siow *,int);
  int    (*unwrite)(struct siow *);
  int    (*flush)  (struct siow *);
  int    (*close)  (struct siow *);
  int      feof;
  size_t   size;
  size_t   off;
  char    *data;
} *SIOW;

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

extern SIOR SIOStdin;
extern SIOW SIOStdout;
extern SIOW SIOStderr;

int		 (SIOInit)		(void);

SIOR		 (SIORNew)		(size_t);
SIOW		 (SIOWNew)		(size_t);

SIOR		 (SIORNull)		(void);
SIOW	 	 (SIOWNull)		(void);
SIOR	 	 (SIORMemory)		(void *,size_t);
SIOW	 	 (SIOWMemory)		(void *,size_t);
SIOR	 	 (SIORString)		(const char *);
SIOW		 (SIOWString)		(char *,size_t);
SIOR	 	 (SIORFH)		(int);
SIOW		 (SIOWFH)		(int);
SIOR	 	 (SIORFile)		(const char *);
SIOW		 (SIOWFile)		(const char *,int);

SIOR	 	 (SIORToEntit)		(SIOR);
SIOR	 	 (SIORFromEntity)	(SIOR);
SIOW		 (SIOWToEntity)		(SIOW);
SIOW		 (SIOWFromEntity)	(SIOW);

SIOW		 (SIOWTee)		(SIOW, ... );
int		 (SIORWTCP)		(SIOR *,SIOW *,const char *,unsigned short);

int		 (SIOREOF)		(SIOR);
size_t		 (SIORSize)		(SIOR);
int		 (SIORRead)		(SIOR);
int		 (SIORUnread))		(SIOR,int);
int		 (SIORRefill)		(SIOR);
int		 (SIORMap)		(SIOR);
int		 (SIORClose)		(SIOR);

int		 (SIOWEOF)		(SIOW);
size_t		 (SIOWSize)		(SIOW);
int		 (SIOWWrite)		(SIOW,int);
int		 (SIOWUnwrite)		(SIOW);
int		 (SIOWFlush)		(SIOW);
int		 (SIOWClose)		(SIOW);

size_t		 (SIOCopy)		(SIOW,SIOR);

size_t		 (LineSIOW)		(const SIOW,const char *);
size_t		 (LineSIOWFormat)	(const SIOW,const char *,const char *, ... );
size_t		 (LineSIOWFormatv)	(const SIOW,const char *,const char *,va_list);

char		*(LineSIOR)		(const SIOR);
char		*(StringFromSIO)	(const SIOW);

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

