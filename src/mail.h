/***************************************************************************
*
* Copyright 2007,2013 by Sean Conner.
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, see <http://www.gnu.org/licenses/>.
*
* Comments, questions and criticisms can be sent to: sean@conman.org
*
*************************************************************************/

#ifndef MAIL_H
#define MAIL_H

#include <stdio.h>

#include "nodelist.h"

typedef struct email
{
  const char *from;
  const char *to;
  const char *replyto;
  const char *subject;
  char       *tbody;
  size_t      bsize;
  time_t      timestamp;
  List        headers;
  FILE       *body;
} *Email;

/********************************************************************/

extern Email		EmailNew		(void);
extern int		EmailSend		(Email);
extern int		EmailFree		(Email);

/********************************************************************/

#endif

