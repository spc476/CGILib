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

#ifndef ERRORS_H_INCLUDE
#define ERRORS_H_INCLUDE
#  if defined(__unix__) || defined(__MACH__)
#    include <sys/types.h>
#    include <unistd.h>
#  endif
#  include <time.h>
#  include <errno.h>
#  include "types.h"
#  include "nodelist.h"
#  include "buffer.h"
#endif

/***********************************************************************/

#ifndef ERRORS_H_DEFINE
#define ERRORS_H_DEFINE

#  define MAXDATA 		8

#  define ERR_GENERAL		1000
#  define ERR_DDT 		2000
#  define ERR_URL 		3000
#  define ERR_BUFFER		4000
#  define ERR_CGI 		5000
#  define ERR_HTTP		6000
#  define ERR_HT		7000
#  define ERR_MEM		8000
#  define ERR_PAIR		9000
#  define ERR_COOKIE		10000
#  define ERR_APP 		20000

#  define ERR_OKAY		0
#  define ERR_NOTIMP		(ERR_GENERAL + 1)
#  define ERR_ERROR		(ERR_GENERAL + 2)
#  define ERR_NOMEMORY		(ERR_GENERAL + 3)
#  define ERR_RETRY		(ERR_GENERAL + 4)

#  define ABORT			(ERR_GENERAL + 100)
#  define IGNORE		(ERR_GENERAL + 101)
#  define RETRY			(ERR_GENERAL + 102)
#  define CONTINUE		(ERR_GENERAL + 103)

#  define KERNOPEN		1
#  define KERNREAD		2
#  define KERNWRITE		3
#  define KERNSOCKET		4
#  define KERNCLOSE		5
#  define KERNSELECT		6
#  define KERNSIGACTION		7
#  define KERNCHDIR		8
#  define KERNSTAT		9
#  define KERNSEEK		10
#  define KERNUNLINK		11
#  define KERNRENAME		12
#  define KERNMKDIR		13
#  define KERNACCESS		14

#  define KERNPOPEN		1000

#endif

/************************************************************************/

#ifndef ERRORS_H_TYPES
#define ERRORS_H_TYPES

  typedef struct error
  {
    time_t     timestamp;
    pid_t      pid;
    char      *class;
    int	       point;
    int	       error;
    char      *format;
    union all  data[MAXDATA];
  } *Error;

  typedef List ErrorHandler;

  struct ehandler
  {
    Node   node;
    int  (*handler)(Error);
  };

#endif

/**********************************************************************/

#ifndef ERRORS_H_API
#define ERRORS_H_API
# include "buffer.h"

  extern const char CgiErr [];
  extern const char KernErr[];

  Error		 (ErrorNew)		(char *,int,int,char *, ... );
  int		 (ErrorPush)		(const char *,int,int,const char *, ... );
  int		 (ErrorRepush)		(Error);
  Error		 (ErrorTop)		(void);
  int		 (ErrorTopError)	(void);
  Error		 (ErrorPop)		(void);
  void		 (ErrorInterpretBuffer)	(Error,Buffer);
  char		*(ErrorInterpret) 	(Error,char *,size_t);
  void		 (ErrorLog)		(void);
  void		 (ErrorFree)		(Error);
  void		 (ErrorClear)		(void);

  int		 (ErrHandlerNew)	(ErrorHandler *);
  int		 (ErrHandlerPush) 	(ErrorHandler *,int (*handler)(Error));
  int		 (ErrHandlerPop)	(ErrorHandler *);
  int		 (ErrThrow)		(ErrorHandler *);
  int		 (ErrHandlerFree) 	(ErrorHandler *);

#endif

/***********************************************************************/

