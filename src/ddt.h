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

#ifndef DDT_H
#  define DDT_H
#  ifdef __unix__
#    define DDTLOGFILE	"/dev/null"
#    define DDTNULFILE	"/dev/null"
#  else
#    error Please define your system
#  endif

#  include <stdarg.h>
#  include "stream.h"

   typedef struct ddtstruct
   {
     Stream  output;
     char   *errtag;
   } *Debug;

   extern Debug ddtstream;
   
   void		(DdtInit)	(void);
   int          (DdtFileOpen)	(Debug *,const char *,const char *);
   int		(DdtStreamOpen)	(Debug *,Stream,const char *);
   void		(ddtlog)	(const Debug,const char *,const char *, ... );
   void		(ddtlogv)	(const Debug,const char *,const char *, va_list);
   int		(DdtFree)	(Debug *);
   
#  define DDTFILEOPEN		(ERR_DDT + 0)

#endif

#undef ddt
#undef ddt__

#ifdef DDT
#  define D(x)		x
#  define ddt(expr)						\
		((void)((expr) ? 				\
				0 				\
			:					\
				ddt__(#expr,__FILE__,__LINE__)))


#  define ddt__(expr,file,line)					\
		(ddtlog(					\
		         ddtstream,				\
		         "$ I $",				\
		         "%a:%b: failed assertion `%c'\n",	\
		         file,					\
		         line,					\
		         expr					\
		       )					\
		 ,exit(1)					\
		 ,0)

#else
#  define D(x)
#  define ddt(expr)	((void)0)
#endif

