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

#ifdef __GNUC__
#  define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <paths.h>

#include "util.h"
#include "rfc822.h"
#include "pair.h"
#include "mail.h"

/*****************************************************************/

static char const m_from[]    = "fred@example.net";
static char const m_to[]      = "wilma@example.net";
static char const m_replyto[] = "";
static char const m_subject[] = "I think Barney's up to no good";

/*****************************************************************/

Email EmailNew(void)
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

int EmailSend(Email const email)
{
  FILE        *output;
  struct pair *hdr;
  struct tm   *ptm;
  char         cmd   [BUFSIZ];
  char         date  [BUFSIZ];
  
  fflush(email->body);
  sprintf(cmd,_PATH_SENDMAIL " %s",email->to);
  
  output = popen(cmd,"w");
  if (output == NULL) return errno;
  
  ptm = localtime(&email->timestamp);
  strftime(date,BUFSIZ,"%a, %d %b %Y %H:%M:%S %Z",ptm);
  
  if (!empty_string(email->replyto))
    fprintf(output,"Reply-To: <%s>\n",email->replyto);
    
  for (
        hdr = (struct pair *)ListGetHead(&email->headers);
        NodeValid(&hdr->node);
        hdr = (struct pair *)NodeNext(&hdr->node)
      )
  {
    fprintf(output,"%s: %s\n",hdr->name,hdr->value);
  }
  
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
  return(0);
}

/****************************************************************/

int EmailFree(Email email)
{
  PairListFree(&email->headers);
  fclose(email->body);
  free(email->tbody);
  if (email->subject != m_subject) free((void *)email->subject);
  if (email->replyto != m_replyto) free((void *)email->replyto);
  if (email->to      != m_to)      free((void *)email->to);
  if (email->from    != m_from)    free((void *)email->from);
  free(email);
  return(0);
}

/*****************************************************************/

