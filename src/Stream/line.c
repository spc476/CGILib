
/**************************************************************************
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
**************************************************************************/

#include <ctype.h>

#include "../stream.h"
#include "../memory.h"
#include "../rawfmt.h"
#include "../util.h"
#include "../ddt.h"

#define STRING_DELTA		1024

/*************************************************************************/

static void	line_fwvcb	(char,void *);

/*************************************************************************/

size_t (LineS)(const Stream out,const char *msg)
{
  size_t amt;
  
  ddt(out != NULL);
  ddt(msg != NULL);
  
  for (amt = 0 ; *msg ; msg++ , amt++)
    StreamWrite(out,*msg);
  return(amt);
}

/************************************************************************/

size_t (LineSFormat)(const Stream out,const char *format,const char *msg, ... )
{
  va_list args;
  size_t  s;
  
  ddt(out    != NULL);
  ddt(format != NULL);
  ddt(msg    != NULL);
  
  va_start(args,msg);
  s = LineSFormatv(out,format,msg,args);
  va_end(args);
  return(s);
}

/***********************************************************************/

size_t (LineSFormatv)(const Stream out,const char *format,const char *msg,va_list args)
{
  ddt(out    != NULL);
  ddt(format != NULL);
  ddt(msg    != NULL);
  
  return(RawDoFmt(format,msg,line_fwvcb,out,args));
}

/************************************************************************/

static void line_fwvcb(char c,void *datum)
{
  StreamWrite((Stream)datum,c);
}

/************************************************************************/

char *(LineSRead)(const Stream in)
{
  size_t  tbuffsize = 0;
  size_t  size      = 0;
  char   *tbuff     = NULL;
  char   *ret;
  int     c;
  
  ddt(in != NULL);
  
  while(!StreamEOF(in))
  {
    if (size == tbuffsize)
    {
      tbuffsize += STRING_DELTA;
      tbuff      = MemResize(tbuff,size,tbuffsize);
    }
    
    c = Line_ReadChar(in);
    if (c == '\n') break;
    tbuff[size++] = c;    
  }
  tbuff[size] = '\0';
  ret         = dup_string(tbuff);
  MemFree(tbuff,tbuffsize);
  return(ret);
}

/*************************************************************************/

int (Line_ReadChar)(const Stream in)
{
  int c1;
  int c2;
  
  c1 = StreamRead(in);
  if (c1 == '\r')
  {
    c2 = StreamRead(in);
    if (c2 != '\n')
    {
      StreamUnRead(in,c2);
      c1 = '\n';
    }
  }
  else if (c1 == '\n')
  {
    c2 = StreamRead(in);
    if (c2 != '\r')
      StreamUnRead(in,c2);
  }
  
  return(c1);
}

/***************************************************************************/

