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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

#include "../util.h"
#include "../cgi.h"

/**************************************************************************/

static void cgicookie_new(const Cgi cgi,const char *data,size_t size)
{
  assert(cgi  != NULL);
  assert(data != NULL);
  
  if (size)
  {
    cgi->buffer = malloc(size + 2);
    memcpy(cgi->buffer,data,size);
    cgi->buffer[size]     = '&';
    cgi->buffer[size + 1] = '\0';
    cgi->bufsize          = size + 1;
  }
  else
  {
    cgi->buffer    = malloc(1);
    cgi->buffer[0] = '\0';
    cgi->bufsize   = 0;
  }
  cgi->pbuff   = cgi->buffer;
  cgi->pbufend = &cgi->buffer[size + 1];
}

/*********************************************************************/

static int cgi_create(Cgi *pcgi,void *data)
{
  struct cgi *cgi;
  
  assert(pcgi != NULL);
  
  cgi = calloc(1,sizeof(struct cgi));
  cgi->data = data;
  *pcgi     = cgi;
  
  ListInit(&cgi->vars);
  return(0);
}

/******************************************************************/

static int cgi_new_get(const Cgi cgi)
{
  char   *query_string;
  size_t  qs;
  
  assert(cgi != NULL);
  
  query_string = getenv("QUERY_STRING");
  if (query_string == NULL)
    return (ENOMSG);
    
  qs = strlen(query_string);
  cgicookie_new(cgi,query_string,qs);
  cgi->method = GET;
  return(0);
}

/***************************************************************/

static int cgi_new_post(const Cgi cgi)
{
  size_t  length;
  char   *content_type;
  char   *content_length;
  
  assert(cgi != NULL);
  
  content_type   = getenv("CONTENT_TYPE");
  content_length = getenv("CONTENT_LENGTH");
  
  if ((content_type == NULL) || (content_length == NULL))
    return ENOMSG;
    
  if (strncmp(content_type,"application/x-www-form-urlencoded",33) != 0)
    return(ENOMSG);
    
  errno  = 0;
  length = strtoul(content_length,NULL,10);
  if ((length == LONG_MAX) && (errno == ERANGE))
    return(ERANGE);
    
  cgi->buffer = malloc(length+2);
  memset(cgi->buffer,'\0',length+2);
  cgi->bufsize          = length+1;
  cgi->buffer[length+1] = '&';
  
  if (fread(cgi->buffer,1,length,stdin) < length)
  {
    if (feof(stdin))
      return errno;
    else
      return ENODATA;
  }
  
  cgi->pbuff   = cgi->buffer;
  cgi->pbufend = &cgi->buffer[cgi->bufsize+1];
  cgi->method  = POST;
  return(0);
}

/**********************************************************************/

static int cgi_new_head(const Cgi cgi)
{
  assert(cgi != NULL);
  
  cgi->method = HEAD;
  return(0);
}

/***************************************************************/

static int cgi_new_put(const Cgi cgi)
{
  char   *content_length;
  size_t  length;
  
  content_length = getenv("CONTENT_LENGTH");
  
  if (content_length == NULL)
    return ENOMSG;
    
  errno  = 0;
  length = strtoul(content_length,NULL,10);
  
  if ((length == LONG_MAX) && (errno == ERANGE))
    return ERANGE;
    
  cgi->bufsize  = length;
  cgi->datatype = getenv("CONTENT_TYPE");
  cgi->method   = PUT;
  
  return 0;
}

/*************************************************************/

Cgi CgiNew(void *data)
{
  char *request_method;
  Cgi   cgi;
  int   rc;
  
  request_method = getenv("REQUEST_METHOD");
  
  if (request_method == NULL)
    return NULL;
    
  if (cgi_create(&cgi,data) != 0)
    return NULL;
    
  if (strcmp(request_method,"GET") == 0)
    rc = cgi_new_get(cgi);
  else if (strcmp(request_method,"POST") == 0)
    rc = cgi_new_post(cgi);
  else if (strcmp(request_method,"HEAD") == 0)
    rc = cgi_new_head(cgi);
  else if (strcmp(request_method,"PUT") == 0)
    rc = cgi_new_put(cgi);
  else
  {
    free(cgi);
    return NULL;
  }
  
  if (rc != 0)
  {
    free(cgi);
    return NULL;
  }
  
  return cgi;
}

/*******************************************************************/
