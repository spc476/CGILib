
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

#include <stdio.h>

#include "../types.h"
#include "../conf.h"
#include "../buffer.h"
#include "../sio.h"
#include "../errors.h"
#include "../memory.h"
#include "../ddt.h"
#include "../util.h"

/************************************************************************/

struct pipe_buffer
{
  struct buffer   buf;
  FILE           *fp;
  char           *cmd;
};

/**********************************************************************/

int PipeBuffer(Buffer *pbuf,char *cmd,Flags flag)
{
  struct pipe_buffer *buffer;
  char                mode[3];
  int                 rc;
  
  ddt(pbuf != NULL);
  ddt(cmd  != NULL);
  
  if ((flag & MODE_READ) == MODE_READ)
    mode[0] = 'r' , mode[1] = '\0';
  else if ((flag & MODE_WRITE) == MODE_WRITE)
    mode[0] = 'w' , mode[1] = '\0';
  
  if ((flag & MODE_APPEND) == MODE_APPEND)
    mode[1] = '+' , mode[2] = '\0';
    
  buffer            = MemAlloc(sizeof(struct pipe_buffer));
  buffer->buf.size  = sizeof(struct pipe_buffer));
  buffer->buf.tag   = PIPEBUFFER;
  buffer->buf.ioreq = pipe_ioreq;
  buffer->cmd       = dup_string(cmd);
  
  do
  {
    buffer->fp = popen(cmd,mode);
    if (buffer->fp == NULL)
    {
      ErrorPush(KernErr,KERNPOPEN,ENOENT,"$",cmd);
      ErrorPush(CgiErr,PIPEBUFFER,BUFERR_BADCMD,"$",cmd);
      rc = ErrThrow(&BufferErr);
      if ((rc != ERR_OKAY) && (rc != RETRY))
      {
        MemFree(buffer->cmd,strlen(cmd)+1);
        MemFree(buffer,sizeof(struct pipe_buffer));
        return(ErrorTopError());
      }
      else
      {
        rc = ERR_OKAY;
        ErrorClear();
      }
    }
    else
      rc = ERR_OKAY;
  } while(rc != ERR_OKAY);

  *pbuf = (Buffer)buffer;
  return(ERR_OKAY);
}

/**********************************************************************/

static int pipe_ioreq(struct buffer *pbuf,int cmd,va_list list)
{
}

/**********************************************************************/
