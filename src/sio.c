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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <string.h>
#include "sio.h"
#include "ddt.h"
#include "memory.h"

/***************************************************************
*
* From Forth, a typical call would be something like:
*
*	" sunrise.cse.fau.edu" port tohost    netopen ( to connect )
*	                       port listenfor netopen ( to wait connection )
*
*	netopen	( [ s.host ] i.port i.flag -- [ fd ] 0 | error )
*	
* From C, it would be something like:
*
*	netopen(TOHOST,PORT,"sunrise.cse.fau.edu");
*	netopen(LISTENTO,PORT);
*
* So, that in C, the prototype is:
*
*	int netopen(int type,int port, ... );
*
*****************************************************************/

int netopen(int type,int port, ... )
{
  va_list             arglist;
  struct hostent     *hp;
  struct sockaddr_in  lsin;
  int                 msgsock;
  unsigned int        length;
  int                 sock;
  int                 err;
  
  sock = socket(AF_INET,SOCK_STREAM,0);
  if (sock < 0) return(-1);

  memset(&lsin,0,sizeof(lsin));
  lsin.sin_family = AF_INET;
  lsin.sin_port   = htons(port);
  
  if (type == TOHOST)
  {
    va_start(arglist,port);
    hp   = gethostbyname(va_arg(arglist,char *));
    err  = errno;
    va_end(arglist);
    if (hp == NULL) 
    {
      errno = err;
      return(-1);
    }
    
    memcpy(&(lsin.sin_addr.s_addr),hp->h_addr,hp->h_length);
    
    if (connect(sock,(struct sockaddr *)&lsin,sizeof(lsin)) < 0)
    {
      err = errno;
      close(sock);
      errno = err;
      return(-1);
    }
    
    return(sock);
  }
  else if (type == LISTENTO)
  {
    lsin.sin_addr.s_addr = INADDR_ANY;
    
    if (bind (sock,(struct sockaddr *)&lsin,sizeof(lsin)))
    {
      err = errno;
      close(sock);
      errno = err;
      return(-1);
    }
    
    listen(sock,5);
    
    length  = sizeof(lsin);
    msgsock = accept(sock,(struct sockaddr *)&lsin,&length);
    if (msgsock < 0)
    {
      err = errno;
      close(sock);
      errno = err;
      return(-1);
    }
    
    close(sock);
    return(msgsock);
  }
  else
  {
    errno = 0;
    return(-1);
  }
}

