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

#ifndef BUFFER_H_INCLUDE
#define BUFFER_H_INCLUDE
#  ifdef __unix__
#    include <sys/types.h>
#    include <unistd.h>
#    include <fcntl.h>
#  endif
#  include <stdio.h>
#  include <stdarg.h>
#  include "types.h"
#  include "errors.h"
#endif

/************************************************************************/

#ifndef BUFFER_H_DEFINE
#define BUFFER_H_DEFINE

#  include "errors.h"

#  ifdef __unix__
#    define MODE_READ		O_RDONLY
#    define MODE_WRITE		O_WRONLY
#    define MODE_RW 		O_RDWR
#    define MODE_CREATE		O_CREAT
#    define MODE_TRUNCATE	O_TRUNC
#    define MODE_APPEND		O_APPEND
#    define SEEK_START		SEEK_SET
#    define SEEK_CURRENT	SEEK_CUR
#  endif

#  define C_READ	0	/* ( ..., void *,size_t *);	*/
#  define C_WRITE 	1	/* ( ..., void *,size_t *);	*/
#  define C_SEEK	2	/* ( ..., unsigned long *,int);	*/
#  define C_FLUSH 	3	/* ( ... )			*/
#  define C_FREE	4	/* ( ... )			*/
#  define C_IOCTL 	5	/* ( ..., ... )			*/
#  define C_SIZE	6	/* ( ..., unsigned long *);	*/
#  define C_BLOCKSIZE	7	/* ( ..., unsigned long *);	*/
#  define C_RETIBUFF	8	/* ( ..., void **,size_t *);	*/
#  define C_NAME	9	/* ( ..., char *,size_t *);	*/

#  define CL_READ822	10
#  define CL_READCLINE	11
#  define CL_READC	12
#  define CL_UNREADC	13
#  define CL_SETEOL	14
#  define CL_WRITEC	15
#  define CL_WRITEF	16	/* ( ... , char *f,char *m, ... );	*/
#  define CL_EOF	17
#  define CL_READBUFFED	18	/* hack for GeoFind */

#  define CF_HANDLE	100	/* hack for GeoFind */

#  define CM_SETSIZE	101	/* hack for mod_blog */
#  define CD_READEOF	102	/* hack for mod_blog */

#  define BUFERR_EOF		(ERR_BUFFER + 1)
#  define BUFERR_NOPUSH		(ERR_BUFFER + 2)
#  define BUFERR_READ		(ERR_BUFFER + 3)
#  define BUFERR_WRITE		(ERR_BUFFER + 4)
#  define BUFERR_CLOSE		(ERR_BUFFER + 5)
#  define BUFERR_OPEN		(ERR_BUFFER + 6)
#  define BUFERR_SIZE		(ERR_BUFFER + 7)
#  define BUFERR_SEEK		(ERR_BUFFER + 8)
#  define FILERR_DELETE		(ERR_BUFFER + 10)
#  define FILERR_RENAME		(ERR_BUFFER + 11)
#  define BUFERR_BADCMD		(ERR_BUFFER + 12)
#  define BUFERR_NULLPTR	(ERR_BUFFER + 13)
#  define BUFERR_NULLHANDLER	(ERR_BUFFER + 14)

#  define BUFINIT		(ERR_BUFFER + 0)
#  define BUFREAD 		(ERR_BUFFER + 1)
#  define BUFWRITE		(ERR_BUFFER + 2)
#  define BUFSEEK 		(ERR_BUFFER + 3)
#  define BUFFLUSH		(ERR_BUFFER + 4)
#  define BUFFREE 		(ERR_BUFFER + 5)
#  define BUFSIZE		(ERR_BUFFER + 6)
#  define BUFBLOCKSIZE		(ERR_BUFFER + 7)
#  define BUFFERCLOSE		(ERR_BUFFER + 8)
#  define BUFFERFORMATREAD	(ERR_BUFFER + 9)
#  define BUFFERFORMATWRITE	(ERR_BUFFER + 10)
#  define BUFFERIOCTL		(ERR_BUFFER + 11)

#  define LINEREAD		BUFREAD
#  define LINEWRITE		BUFWRITE
#  define LINEFREE		BUFFREE
#  define LINESETEOL		(ERR_BUFFER + 20)
#  define LINEREAD822		(ERR_BUFFER + 21)
#  define LINEREADCLINE		(ERR_BUFFER + 22)
#  define LINEREADC		(ERR_BUFFER + 23)
#  define LINEWRITEC		(ERR_BUFFER + 24)
#  define LINEWRITEF		(ERR_BUFFER + 25)

#  define NULLBUFFER		(ERR_BUFFER + 80)
#  define ECHOBUFFER		(ERR_BUFFER + 81)
#  define FILEBUFFER		(ERR_BUFFER + 82)
#  define FHBUFFER		(ERR_BUFFER + 83)
#  define LINEBUFFER		(ERR_BUFFER + 84)
#  define MEMORYBUFFER		(ERR_BUFFER + 85)
#  define DYNAMICBUFFER		(ERR_BUFFER + 86)
#  define TCPBUFFER		(ERR_BUFFER + 87)
#  define UDPBUFFER		(ERR_BUFFER + 88)
#  define FILEDELETE		(ERR_BUFFER + 89)
#  define FILERENAME		(ERR_BUFFER + 90)
#  define FILERENAMEDEL		(ERR_BUFFER + 91)
#  define PIPEBUFFER		(ERR_BUFFER + 92)

#endif

/***************************************************************************/

#ifndef BUFFER_H_TYPES
#define BUFFER_H_TYPES

  typedef struct buffer
  {
    Size   size;
    int	   tag;
    int  (*ioreq)(struct buffer *const,int,va_list);
  } *Buffer;

#endif

/*************************************************************************/

#ifndef BUFFER_H_API
#define BUFFER_H_API

  extern ErrorHandler BufferErr;

  int		 NullBuffer		(Buffer *);
  int		 DynamicBuffer		(Buffer *);
  int		 FileBuffer		(Buffer *,const char *,Flags);
  char	 	*FileTmpName		(void);
  int		 FileDelete		(const char *);
  int		 FileRename		(const char *,const char *);
  int		 FileRenameDel		(const char *,const char *);
  int		 FHBuffer		(Buffer *,int);
  int		 TCPBuffer		(Buffer *,const char *,int);
  int		 LineBuffer		(Buffer *,Buffer);
  int		 MemoryBuffer		(Buffer *,void *,size_t);
  int            StdinBuffer		(Buffer *);
  int            StdoutBuffer		(Buffer *);
  int            StderrBuffer		(Buffer *);
  
#  if 0
    int		 PipeBuffer		(Buffer *,char *,Flags);
    int		 EchoBuffer		(Buffer *);
    int		 UDPBuffer		(Buffer *,char *,int);
#  endif

  void		 (BufferInit)		(void);
  int		 (BufferIOCtl)		(const Buffer,int, ... );
  int		 (BufferRead)		(const Buffer,const void *,size_t *);	/* */
  int		 (BufferFormatRead)	(const Buffer,const char *,const char *, ... );
  int		 (BufferWrite)		(const Buffer,const void *,size_t *);	/* */
  int		 (BufferFormatWrite)	(const Buffer,const char *,const char *, ... );
  int		 (BufferFormatWritev)	(const Buffer,const char *,const char *,va_list);
  int		 (BufferSeek)		(const Buffer,long *,int);		/* */
  int		 (BufferFlush)		(const Buffer);			/* */
  int            (BufferEOF)		(const Buffer);
  unsigned long  (BufferSize)		(const Buffer);			
  unsigned long	 (BufferBlockSize)	(const Buffer);
  int		 (BufferFree)		(Buffer *);			/* */

	/* Line specific calls ... */

#  define LineFree	BufferFree
#  define LineRead	BufferRead
#  define LineWrite	BufferWrite

  int		 (LineSetEOL)		(const Buffer,const char *);		/* */
  int		 (LineRead822)		(const Buffer,const char *,size_t *);	/* */
  int		 (LineReadCLine)	(const Buffer,const char *,size_t *);	/* */
  int		 (LineReadC)		(const Buffer,const char *);		/* */
  int		 (LineUnReadC)		(const Buffer,char);			/* */
  int		 (LineWriteC)		(const Buffer,char);			/* */
  int		 (LineWritef)		(const Buffer,const char *,const char *,... );
  int		 (LineEOF)		(const Buffer);			/* */

  /*-------------------------------------------------------------*/
  
#  ifdef SCREAM
#    define StdinBuffer(p)	FHBuffer((p),0)
#    define StdoutBuffer(p)	FHBuffer((p),1)
#    define StderrBuffer(p)     FHBuffer((p),2)
#    define BufferRead(b,d,s)	BufferIOCtl((b),C_READ,(d),(s))
#    define BufferWrite(b,d,s)	BufferIOCtl((b),C_WRITE,(d),(s))
#    define BufferSeek(b,pl,w)	BufferIOCtl((b),C_SEEK,(pl),(w))
#    define BufferFlush(b)	BufferIOCtl((b),C_FLUSH)
#    define BufferEOF(b)	BufferIOCtl((b),CL_EOF)
#    define BufferFree(pb)	BufferIOCtl(*(pb),C_FREE)

#    define LineSetEOL(b,eol)	BufferIOCtl((b),CL_SETEOL,(eol))
#    define LineRead822(b,d,s)	BufferIOCtl((b),CL_READ822,(d),(s))
#    define LineReadCLine(b,d,s) BufferIOCtl((b),CL_READCLINE,(d),(s))
#    define LineUnReadC(b,c)	BufferIOCtl((b),CL_UNREADC,(c))
#    define LineWriteC(b,c)	BufferIOCtl((b),CL_WRITEC,(c))
#    define LineReadC(b,d)	BufferIOCtl((b),CL_READC,(d))
#    define LineEOF(b)		BufferIOCtl((b),CL_EOF)
#  endif
#endif


