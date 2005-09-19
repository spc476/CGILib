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

#ifdef AIX
#  define _XOPEN_EXTENDED_SOURCE
#endif

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

#define DDT_C

#include "stream.h"
#include "errors.h"
#include "memory.h"
#include "ddt.h"
#include "util.h"

#if 0
#  define DDTOPEN	100
#  define DDTLOG	101
#  define DDTCLOSE	102
#endif

#ifdef DDT
#  define i_ddt(expr)						\
		((void)((expr) ? 				\
				0 				\
			:					\
				i_ddt__(#expr,__FILE__,__LINE__)))


#  define i_ddt__(expr,file,line)				\
		(syslog(					\
		         LOG_ERR,				\
		         "%s(%d): failed assertion `%s'",	\
		         file,					\
		         line,					\
		         expr					\
		       )					\
		,exit(1)					\
		,0)
#else
#  define i_ddt(expr)	((void)0)
#endif

/************************************************************************/

void (DdtInit)(void)
{
  if (DdtFileOpen(&ddtstream,DDTLOGFILE,"DDT") != ERR_OKAY)
  {
    syslog(LOG_ERR,__FILE__ ": - cannot open " DDTLOGFILE);
    exit(1);
  }
}

/************************************************************************/

int (DdtFileOpen)(Debug *pddt,const char *logfile,const char *errtag)
{
  Stream output;
  
  i_ddt(pddt    != NULL);
  i_ddt(logfile != NULL);
  i_ddt(errtag  != NULL);

  output = FileStreamWrite(logfile,FILE_APPEND);
  if (output == NULL)
    return(ERR_ERR);

  return(DdtStreamOpen(pddt,output,errtag));
}

/**************************************************************************/

int (DdtStreamOpen)(Debug *pddt,Stream output,const char *errtag)
{
  Debug sddt;
  
  i_ddt(pddt   != NULL);
  i_ddt(output != NULL);
  i_ddt(errtag != NULL);
  
  sddt         = MemAlloc(sizeof(struct ddtstruct));
  sddt->output = output;
  sddt->errtag = dup_string(errtag);
  *pddt        = sddt;
  return(ERR_OKAY);
}

/***********************************************************************/

void (ddtlog)(const Debug sddt,const char *format,const char *msg, ... )
{
  va_list list;
  
  i_ddt(sddt   != NULL);
  i_ddt(format != NULL);
  i_ddt(msg    != NULL);
  
  va_start(list,msg);
  ddtlogv(sddt,format,msg,list);
  va_end(list);
}

/*************************************************************************/

void (ddtlogv)(const Debug sddt,const char *format,const char *msg,va_list list)
{
  i_ddt(sddt   != NULL);
  i_ddt(format != NULL);
  i_ddt(msg    != NULL);
  
  LineS(sddt->output,sddt->errtag);
  StreamWrite(sddt->output,' ');
  LineSFormatv(sddt->output,format,msg,list);
  StreamWrite(sddt->output,'\n');
  StreamFlush(sddt->output);
}

/************************************************************************/

int (DdtFree)(Debug *pddt)
{
  Debug sddt;
  
  i_ddt(pddt  != NULL);
  i_ddt(*pddt != NULL);
  
  sddt = *pddt;
  StreamFree(sddt->output);
  MemFree(sddt->errtag);
  MemFree(sddt);
  *pddt = NULL;
  return(ERR_OKAY);
}

/************************************************************************/

