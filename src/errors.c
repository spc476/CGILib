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

#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define ERRORS_C
#include "conf.h"
#include "types.h"
#include "errors.h"
#include "memory.h"
#include "util.h"
#include "ddt.h"

#define MAXSTACK	16

/***************************************************************************/

static Error		 error_make	(const char *,int,int,const char *,va_list);

/**************************************************************************/

static Error		 error_stack[MAXSTACK];
static size_t		 error_sp    = MAXSTACK;

/***************************************************************************/

Error (ErrorNew)(char *tag,int point,int err,char *format, ... )
{
  va_list ap;
  Error   error;

  ddt(tag    != NULL);
  ddt(point  >	0);
  ddt(err    >	0);
  ddt(format != NULL);

  va_start(ap,format);
  error = error_make(tag,point,err,format,ap);
  va_end(ap);
  return(error);
}

/*************************************************************************/

int ErrorPush(const char *tag,int point,int err,const char *format, ... )
{
  va_list ap;
  Error   error;

  ddt(tag    != NULL);
  ddt(point  >	0);
  ddt(err    >	0);
  ddt(format != NULL);

  va_start(ap,format);
  error = error_make(tag,point,err,format,ap);
  va_end(ap);

  if (error_sp > 0)
    error_stack[--error_sp] = error;
  return(err);
}

/*********************************************************************/

int ErrorRepush(Error error)
{
  if (error_sp > 0)
    error_stack[--error_sp] = error;
  return(error->error);
}

/********************************************************************/

Error ErrorTop(void)
{
  if (error_sp == MAXSTACK)
    return(NULL);
  else
    return(error_stack[error_sp]);
}

/*********************************************************************/

int ErrorTopError(void)
{
  if (error_sp == MAXSTACK)
    return(ERR_OKAY);
  else
    return(error_stack[error_sp]->error);
}

/*******************************************************************/

Error ErrorPop(void)
{
  Error error;

  if (error_sp < MAXSTACK)
    error = error_stack[error_sp++];
  else
  {
    /*ddt(0);*/
    error = NULL;
  }
  return(error);
}

/**************************************************************************/

char *ErrorInterpret(Error error,char *dest,size_t size)
{
  Buffer  outbuf;

  ddt(error != NULL);
  ddt(dest  != NULL);
  ddt(size  >  0);
    
  memset(dest,0,size);
  if (MemoryBuffer(&outbuf,dest,size-1) != ERR_OKAY)
    ErrorClear();
  else
  {
    ErrorInterpretBuffer(error,outbuf);
    BufferFree(&outbuf);
  }
  return(dest);
}
 
/***********************************************************************/
  
void ErrorInterpretBuffer(Error error,Buffer outbuf)
{
  char *format;
  int   cnt;
  
  ddt(error  != NULL);
  ddt(outbuf != NULL);

  BufferFormatWrite(
                     outbuf,
                     "$ i4.4 i5.5",
                     "%(class)a%(point)b%(error)c",
                     error->class,
                     error->point,
                     error->error
                   );
                   
  for (format = error->format , cnt = 0 ; *format ; format++ , cnt++)  
  {
    switch(*format)
    {
      case 'c':
      case 'C': BufferFormatWrite(outbuf,"c"," %a",error->data[cnt].c);  break;
      case 's': BufferFormatWrite(outbuf,"s"," %a",error->data[cnt].s);  break;
      case 'S': BufferFormatWrite(outbuf,"S"," %a",error->data[cnt].us); break;
      case 'i': BufferFormatWrite(outbuf,"i"," %a",error->data[cnt].i);  break;
      case 'I': BufferFormatWrite(outbuf,"I"," %a",error->data[cnt].ui); break;
      case 'l': BufferFormatWrite(outbuf,"l"," %a",error->data[cnt].l);  break;
      case 'L': BufferFormatWrite(outbuf,"L"," %a",error->data[cnt].ul); break;
      case 'o': BufferFormatWrite(outbuf,"o"," %a",error->data[cnt].ui); break;
      case 'x': BufferFormatWrite(outbuf,"x"," %a",error->data[cnt].ui); break;
      case 'f': BufferFormatWrite(outbuf,"f"," %a",error->data[cnt].f);  break;
      case 'd': BufferFormatWrite(outbuf,"d"," %a",error->data[cnt].d);  break;
      case '$': BufferFormatWrite(outbuf,"$"," \"%a\"",error->data[cnt].p); break;
      default: ddt(0);
    }
  }
}

/**************************************************************************/

void ErrorLog(void)
{
  Buffer outbuf;
  char	 outbuffer[8192];
  int    i;
  size_t size;
    
  memset(outbuffer,0,sizeof(outbuffer));
  if (MemoryBuffer(&outbuf,outbuffer,sizeof(outbuffer)-1) != ERR_OKAY)
  {
    ddtlog(ddtstream,"","an error occured trying to log an error.  Sorry");
    return;
  }
  
  BufferFormatWrite(
                     outbuf,
                     "L L",
                     "%(timestamp)a %(pid)b",
                     (unsigned long)error_stack[error_sp]->timestamp,
                     (unsigned long)error_stack[error_sp]->pid
                   );

  for (i = error_sp ; i < MAXSTACK ; i++)
  {
    size = 1;
    BufferWrite(outbuf," ",&size);
    ErrorInterpretBuffer(error_stack[i],outbuf);
  }
  
  BufferFree(&outbuf);
  ddtlog(ddtstream,"$","%a",outbuffer);
}

/***********************************************************************/

void ErrorFree(Error error)
{
  ddt(error         != NULL);
  ddt(error->class  != NULL);
  ddt(error->format != NULL);

  MemFree(error->class,strlen(error->class)+1);
  MemFree(error->format,strlen(error->format)+1);
  MemFree(error,sizeof(struct error));
}

/**************************************************************************/

void ErrorClear(void)
{
  Error error;

  while((error = ErrorPop()) != NULL)
  {
    ErrorFree(error);
  }
}

/**************************************************************************/

static Error error_make(const char *tag,int point,int err,const char *format,va_list ap)
{
  Error  error;
  size_t cnt;

  ddt(tag    != NULL);
  ddt(point  >	0);
  ddt(err    >	0);
  ddt(format != NULL);

  error 	   = MemAlloc(sizeof(struct error));	/* we either return, or not */
  error->timestamp = time(NULL);
  error->pid	   = getpid();
  error->class	   = dup_string(tag);
  error->point	   = point;
  error->error	   = err;
  error->format    = dup_string(format);
  cnt		   = 0;

  while((*format) && (cnt < MAXDATA))
  {
    switch(*format)
    {
      case 'c': error->data[cnt].c  = va_arg(ap,int);                break;
      case 'C': error->data[cnt].uc = va_arg(ap,int);                break;
      case 's': error->data[cnt].s  = va_arg(ap,int);                break;
      case 'S': error->data[cnt].us = va_arg(ap,int);                break;
      case 'o':
      case 'i': error->data[cnt].i  = va_arg(ap,signed int);         break;
      case 'I': error->data[cnt].ui = va_arg(ap,unsigned int);       break;
      case 'l': error->data[cnt].l  = va_arg(ap,signed long int);    break;
      case 'L': error->data[cnt].ul = va_arg(ap,unsigned long int);  break;
      case 'x': error->data[cnt].ul = va_arg(ap,unsigned long int);  break;
      case 'f': error->data[cnt].f  = va_arg(ap,double);             break;
      case 'd': error->data[cnt].d  = va_arg(ap,double);             break;
      case '$': error->data[cnt].p  = va_arg(ap,char *);             break;
      case 'p': error->data[cnt].p  = va_arg(ap,void *);             break;
      case ' ': break;
      default:	ddt(0);
    }
    cnt++;
    format++;
  }
  return(error);
}

/**************************************************************************/

int ErrHandlerNew(ErrorHandler *peh)
{
  ddt(peh != NULL);

  ListInit(peh);
  return(ERR_OKAY);
}

/************************************************************************/

int ErrHandlerPush(ErrorHandler *peh,int (*handler)(Error))
{
  struct ehandler *p;

  ddt(peh != NULL);
  ddt(handler);

  p = MemAlloc(sizeof(struct ehandler));
  p->handler = handler;
  ListAddHead(peh,&p->node);
  return(ERR_OKAY);
}

/********************************************************************/

int ErrHandlerPop(ErrorHandler *peh)
{
  struct ehandler *p;

  ddt(peh != NULL);

  p = (struct ehandler *)ListRemHead(peh);
  if (NodeValid(&p->node)) 
  {
    MemFree(p,sizeof(struct ehandler));
    return(ERR_OKAY);
  }
  return(ERR_GENERAL);
}

/********************************************************************/

int ErrThrow(ErrorHandler *peh)
{
  Error            error;
  struct ehandler *p;

  ddt(peh != NULL);

  error = ErrorTop();
  if (error)
  {
    for (
	  p = (struct ehandler *)ListGetHead(peh);
	  NodeValid(&p->node);
	  p = (struct ehandler *)NodeNext(&p->node)
	)
    {
      switch((*p->handler)(error))
      {
	case CONTINUE: continue;
	case IGNORE:   return(ERR_OKAY);
	case ABORT:
	     ddtlog(ddtstream,"","Error Abort");
	     exit(1);
	case RETRY:    return(ERR_RETRY);
	default:       ddt(0);
      }
    }
    return(error->error);
  }
  return(ERR_OKAY);
}

/******************************************************************/

int ErrHandlerFree(ErrorHandler *peh)
{
  ddt(peh != NULL);

  while(ErrHandlerPop(peh) == ERR_OKAY)
    ;
  return(ERR_OKAY);
}

/******************************************************************/

