/***************************************************************************
*
* Copyright 2001,2013 by Sean Conner.
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

#define _GNU_SOURCE

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#include "../util.h"
#include "../cgi.h"

/**************************************************************************/

static bool makelist(List *vars,char const *data)
{
  while (*data != '\0')
  {
    struct pair *psp = PairNew(&data,'=','&');
    
    if (psp == NULL)
      return false;
      
    UrlDecodeString(psp->name);
    UrlDecodeString(psp->value);
    ListAddTail(vars,&psp->node);
  }
  
  return true;
}

/**************************************************************************/

static bool parsequery(Cgi cgi)
{
  assert(cgi != NULL);
  
  /*-----------------------------------------------------------------------
  ; CGI-3875 mandates the QUERY environment variable must be sent.  If it's
  ; not, it's a server error.
  ;------------------------------------------------------------------------*/
  
  char *query = getenv("QUERY");
  
  /*----------------------------------------------------------------------
  ; If there's no '=', then the query is NOT a series of name/value pairs,
  ; but a, or lack of a better term, plain query string.
  ;-----------------------------------------------------------------------*/
  
  if (strchr(query,'=') == NULL)
  {
    cgi->query = strdup(query);
    if (cgi->query == NULL)
      return false;
    UrlDecodeString(cgi->query);
    return true;
  }
  
  /*--------------------------------------
  ; We have name/value pairs to deal with
  ;---------------------------------------*/
  
  else
    return makelist(&cgi->qvars,query);
}

/***************************************************************/

static http__e cgi_new_get(Cgi const cgi)
{
  assert(cgi != NULL);
  cgi->method = GET;
  return HTTP_OKAY;
}

/***************************************************************/

static http__e cgi_new_post(Cgi const cgi)
{
  char   *content_type;
  char   *content_length;
  char   *buffer;
  size_t  length;
  bool    okay;
  
  assert(cgi != NULL);
  
  cgi->method    = POST;
  content_type   = getenv("CONTENT_TYPE");
  content_length = getenv("CONTENT_LENGTH");
  
  if ((content_type == NULL) || (content_length == NULL))
    return HTTP_LENGTHREQ;
    
  if (strncmp(content_type,"application/x-www-form-urlencoded",33) != 0)
    return HTTP_MEDIATYPE;
    
  errno  = 0;
  length = strtoul(content_length,NULL,10);
  if ((length == LONG_MAX) && (errno == ERANGE))
    return HTTP_TOOLARGE;
    
  buffer = malloc(length + 1);
  
  if (fread(buffer,1,length,stdin) < length)
    return HTTP_METHODFAILURE;
    
  buffer[length] = '\0';
  okay           = makelist(&cgi->pvars,buffer);
  free(buffer);
  return okay ? HTTP_OKAY : HTTP_ISERVERERR;
}

/**********************************************************************/

static int cgi_new_put(Cgi const cgi)
{
  char   *content_length;
  size_t  length;
  
  content_length = getenv("CONTENT_LENGTH");
  
  if (content_length == NULL)
    return HTTP_LENGTHREQ;
    
  errno  = 0;
  length = strtoul(content_length,NULL,10);
  if ((length == LONG_MAX) && (errno == ERANGE))
    return HTTP_TOOLARGE;
    
  cgi->bufsize  = length;
  cgi->datatype = getenv("CONTENT_TYPE");
  cgi->method   = PUT;
  return HTTP_OKAY;
}

/*************************************************************/

Cgi CgiNew(void)
{
  char *gateway_interface;
  char *request_method;
  Cgi   cgi;
  
  gateway_interface = getenv("GATEWAY_INTERFACE");
  if ((gateway_interface == NULL) || (strcmp(gateway_interface,"CGI/1.1") != 0))
    return NULL;
    
  request_method = getenv("REQUEST_METHOD");
  
  if (request_method == NULL)
    return NULL;
    
  cgi = malloc(sizeof(struct cgi));
  if (cgi == NULL)
    return NULL;
    
  cgi->query    = NULL;
  cgi->datatype = NULL;
  cgi->status   = HTTP_OKAY;
  cgi->method   = OTHER;
  ListInit(&cgi->qvars);
  ListInit(&cgi->pvars);
  
  if (strcmp(request_method,"GET") == 0)
    cgi->status = cgi_new_get(cgi);
  else if (strcmp(request_method,"POST") == 0)
    cgi->status = cgi_new_post(cgi);
  else if (strcmp(request_method,"HEAD") == 0)
    cgi->status = cgi_new_get(cgi);
  else if (strcmp(request_method,"PUT") == 0)
    cgi->status = cgi_new_put(cgi);
    
  if ((cgi->status == HTTP_OKAY) && !parsequery(cgi))
    cgi->status = HTTP_ISERVERERR;
    
  return cgi;
}

/*******************************************************************/
