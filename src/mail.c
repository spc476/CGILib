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

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "util.h"
#include "rfc822.h"
#include "pair.h"
#include "mail.h"
#include "errors.h"

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
  
  email            = malloc(sizeof(struct email));
  email->from      = m_from;
  email->to        = m_to;
  email->replyto   = m_replyto;
  email->subject   = m_subject;
  email->tbody     = NULL;
  email->bsize     = 0;
  email->timestamp = time(NULL);
  email->body      = open_memstream(&email->tbody,&email->bsize);
  ListInit(&email->headers);
  return (email);
}

/*****************************************************************/

int (EmailSend)(Email email)
{
  FILE      *output;
  struct tm *ptm;
  char       cmd   [BUFSIZ];
  char       date  [BUFSIZ];
  
  fflush(email->body);
  sprintf(cmd,SENDMAIL " %s",email->to);

  output = popen(cmd,"w");
  if (output == NULL) return ERR_ERR;
  
  ptm = localtime(&email->timestamp);
  strftime(date,BUFSIZ,"%a, %d %b %Y %H:%M:%S %Z",ptm);

  if (!empty_string(email->replyto))
    fprintf(output,"Reply-To: <%s>\n",email->replyto);
  
  fprintf(
  	output,
  	"From: <%s>\n"
  	"To: <%s>\n"
  	"Subject: %s\n"
  	"Date: <%s>\n"
  	"\n"
  	"%s\n",
  	email->from,
  	email->to,
  	email->subject,
  	date,
  	email->tbody
  );
  
  fclose(output);
  return(ERR_OKAY);  
}

/****************************************************************/

int (EmailFree)(Email email)
{
  PairListFree(&email->headers);
  fclose(email->body);
  free(email->tbody);
  if (email->subject != m_subject) free((void *)email->subject);
  if (email->replyto != m_replyto) free((void *)email->replyto);
  if (email->to      != m_to)      free((void *)email->to);
  if (email->from    != m_from)    free((void *)email->from);
  free(email);
  return(ERR_OKAY);
}

/*****************************************************************/

