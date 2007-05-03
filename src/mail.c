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

#define _GNU_SOURCE

#include <stdio.h>
#include <time.h>

#include "memory.h"
#include "stream.h"
#include "ddt.h"
#include "util.h"
#include "rfc822.h"
#include "pair.h"
#include "mail.h"

#define SENDMAIL	"/usr/sbin/sendmail"

/*****************************************************************/

const char m_from[]    = "fred@example.net";
const char m_to[]      = "wilma@example.net";
const char m_replyto[] = "";
const char m_subject[] = "I think Barney's up to no good";

/*****************************************************************/

Email (EmailNew)(void)
{
  Email email;
  
  email            = MemAlloc(sizeof(struct email));
  email->from      = m_from;
  email->to        = m_to;
  email->replyto   = m_replyto;
  email->subject   = m_subject;
  email->timestamp = time(NULL);
  email->body      = StringStreamWrite();
  ListInit(&email->headers);
  return (email);
}

/*****************************************************************/

static Stream open_sendmail(Email email)
{
  char cmd[BUFSIZ];

  ddt(email != NULL);
  
  sprintf(cmd,SENDMAIL " %s",email->to);
  email->pipe = popen(cmd,"w");
  if (email->pipe == NULL)
    return(NULL);
  return(FHStreamWrite(fileno(email->pipe)));
}

static void close_sendmail(Email email,Stream output)
{
  ddt(email  != NULL);
  ddt(output != NULL);
  
  StreamFree(output);
  pclose(email->pipe);
}

/*****************************************************************/

int (EmailSend)(Email email)
{
  Stream     output;
  struct tm *ptm;
  char       date[BUFSIZ];
  char      *body;
  
  output = open_sendmail(email);
  
  ptm    = localtime(&email->timestamp);
  strftime(date,BUFSIZ,"%a, %d %b %Y %H:%M:%S %Z",ptm);
  
  RFC822HeaderWrite(output,"from",email->from);
  if (!empty_string(email->replyto)) RFC822HeaderWrite(output,"reply-to",email->replyto);
  RFC822HeaderWrite(output,"to",email->to);
  RFC822HeaderWrite(output,"subject",email->subject);
  RFC822HeaderWrite(output,"date",date);
  RFC822HeadersWrite(output,&email->headers);
  LineS(output,"\n");
  
  body = StringFromStream(email->body);

  LineS(output,body);
  MemFree(body);
  close_sendmail(email,output);
  return(ERR_OKAY);  
}

/****************************************************************/

int (EmailFree)(Email email)
{
  PairListFree(&email->headers);
  StreamFree(email->body);
  if (email->subject != m_subject) MemFree((void *)email->subject);
  if (email->replyto != m_replyto) MemFree((void *)email->replyto);
  if (email->to      != m_to)      MemFree((void *)email->to);
  if (email->from    != m_from)    MemFree((void *)email->from);
  MemFree(email);
  return(ERR_OKAY);
}

/*****************************************************************/

