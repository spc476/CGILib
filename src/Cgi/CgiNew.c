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

static http__e makelist(List *vars,char const *data)
{
  while (*data != '\0')
  {
    struct pair *psp = PairNew(&data,'=','&');
    
    if (psp == NULL)
      return HTTP_ISERVERERR;
      
    if ((UrlDecodeString(psp->name) == NULL) || (UrlDecodeString(psp->value) == NULL))
    {
      PairFree(psp);
      PairListFree(vars);
      return HTTP_BADREQ;
    }
    ListAddTail(vars,&psp->node);
    if (*data == '&')
      data++;
  }
  
  return HTTP_OKAY;
}

/**************************************************************************/

static int crq(int c)
{
  return c == '\r';
}

/**************************************************************************/

static http__e makeplainlist(List *vars,char const *data)
{
  while(*data != '\0')
  {
    struct pair *psp = PairNew(&data,'=','\n');
    
    if (psp == NULL)
      return HTTP_ISERVERERR;
      
    remove_char(psp->name,crq);
    remove_char(psp->value,crq);
    ListAddTail(vars,&psp->node);
    if (*data == '\n')
      data++;
  }
  
  return HTTP_OKAY;
}

/**************************************************************************/

static bool parsequery(Cgi cgi)
{
  assert(cgi != NULL);
  
  /*-----------------------------------------------------------------------
  ; CGI-3875 mandates the QUERY environment variable must be sent.  If it's
  ; not, it's a server error.
  ;------------------------------------------------------------------------*/
  
  char *query = getenv("QUERY_STRING");
  
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

static http__e cgi_new_head(Cgi cgi)
{
  assert(cgi != NULL);
  cgi->method = HEAD;
  return HTTP_OKAY;
}

/***************************************************************/

static http__e cgi_new_get(Cgi cgi)
{
  assert(cgi != NULL);
  cgi->method = GET;
  return HTTP_OKAY;
}

/***************************************************************/

static http__e cgi_new_post(Cgi cgi)
{
  char    *content_length;
  char    *buffer;
  http__e  result;
  
  assert(cgi != NULL);
  
  cgi->method       = POST;
  cgi->content_type = getenv("CONTENT_TYPE");
  content_length    = getenv("CONTENT_LENGTH");
  
  if (cgi->content_type == NULL)
    return HTTP_MEDIATYPE;
    
  if (content_length == NULL)
    return HTTP_LENGTHREQ;
    
  errno = 0;
  cgi->content_length = strtoul(content_length,NULL,10);
  if ((cgi->content_length == LONG_MAX) && (errno == ERANGE))
    return HTTP_TOOLARGE;
    
  buffer = malloc(cgi->content_length + 1);
  if (buffer == NULL)
    return HTTP_ISERVERERR;
    
  if (fread(buffer,1,cgi->content_length,stdin) < cgi->content_length)
  {
    free(buffer);
    return HTTP_METHODFAILURE;
  }
  buffer[cgi->content_length] = '\0';
  
  if (strncmp(cgi->content_type,"application/x-www-form-urlencoded",33) == 0)
    result = makelist(&cgi->pvars,buffer);
  else if (strncmp(cgi->content_type,"multipart/form-data",19) == 0)
    result = HTTP_MEDIATYPE;
  else if (strncmp(cgi->content_type,"text/plain",10) == 0)
    result = makeplainlist(&cgi->pvars,buffer);
  else
    result = HTTP_MEDIATYPE;
    
  free(buffer);
  return result;
}

/**********************************************************************/

static int cgi_new_put(Cgi cgi)
{
  char *content_length;
  
  cgi->method       = PUT;
  cgi->content_type = getenv("CONTENT_TYPE");
  content_length    = getenv("CONTENT_LENGTH");
  
  if (cgi->content_type == NULL)
    return HTTP_MEDIATYPE;
    
  if (content_length == NULL)
    return HTTP_LENGTHREQ;
    
  errno  = 0;
  cgi->content_length = strtoul(content_length,NULL,10);
  if ((cgi->content_length == LONG_MAX) && (errno == ERANGE))
    return HTTP_TOOLARGE;
    
  return HTTP_OKAY;
}

/*************************************************************/

static int cgi_new_delete(Cgi cgi)
{
  assert(cgi != NULL);
  cgi->method = DELETE;
  return HTTP_OKAY;
}

/***************************************************************/

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
    
  cgi->query          = NULL;
  cgi->content_type   = NULL;
  cgi->content_length = 0;
  cgi->status         = HTTP_OKAY;
  cgi->method         = OTHER;
  ListInit(&cgi->qvars);
  ListInit(&cgi->pvars);
  
  if (strcmp(request_method,"HEAD") == 0)
    cgi->status = cgi_new_head(cgi);
  if (strcmp(request_method,"GET") == 0)
    cgi->status = cgi_new_get(cgi);
  else if (strcmp(request_method,"POST") == 0)
    cgi->status = cgi_new_post(cgi);
  else if (strcmp(request_method,"PUT") == 0)
    cgi->status = cgi_new_put(cgi);
  else if (strcmp(request_method,"DELETE") == 0)
    cgi->status = cgi_new_delete(cgi);
    
  if ((cgi->status == HTTP_OKAY) && !parsequery(cgi))
    cgi->status = HTTP_BADREQ;
    
  return cgi;
}

/*******************************************************************/
