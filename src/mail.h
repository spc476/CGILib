/************************************************************************
*
* Copyright 2007 by Sean Conner.  All Rights Reserved.
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

#ifndef MAIL_H
#define MAIL_H

#include <stdio.h>

#include "stream.h"
#include "nodelist.h"

typedef struct email
{
  char   *from;
  char   *to;
  char   *replyto;
  char   *subject;
  time_t  timestamp;
  List    headers;
  Stream  body;
  FILE   *pipe;
} *Email;

/********************************************************************/

Email		(EmailNew)		(void);
int		(EmailSend)		(Email);
int		(EmailFree)		(Email);

/********************************************************************/

#endif

