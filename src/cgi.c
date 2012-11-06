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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>

#include "util.h"
#include "cgi.h"

/**************************************************************************/

static int		 cgi_create	(Cgi *,void *);
static int		 cgi_new_get	(const Cgi);
static int		 cgi_new_post	(const Cgi);
static int		 cgi_new_head	(const Cgi);
static int		 cgi_new_put	(const Cgi);
static void              cgicookie_new  (const Cgi,const char *,size_t);

/*************************************************************************/

Cgi (CgiNew)(void *data)
{
  char *request_method;
  Cgi   cgi;
  
  request_method = getenv("REQUEST_METHOD");
  
  if (request_method == NULL)
    return NULL;
  
  if (cgi_create(&cgi,data) != 0)
    return NULL;
  
  if (strcmp(request_method,"GET") == 0)
    cgi_new_get(cgi);
  else if (strcmp(request_method,"POST") == 0)
    cgi_new_post(cgi);
  else if (strcmp(request_method,"HEAD") == 0)
    cgi_new_head(cgi);
  else if (strcmp(request_method,"PUT") == 0)
    cgi_new_put(cgi);
  else
  {
    free(cgi);
    return NULL;
  }

  return cgi;
}

/*******************************************************************/

static int cgi_create(Cgi *pcgi,void *data)
{
  struct cgi *cgi;

  assert(pcgi != NULL);
       
  cgi = malloc(sizeof(struct cgi));
  memset(cgi,0,sizeof(struct cgi));
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

  fread(cgi->buffer,1,length,stdin);

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

struct pair *(CgiNextValue)(const Cgi cgi)
{
  struct pair *psp;
  char        *s;
  char        *d;
  
  assert(cgi != NULL);
  
  if (cgi->pbuff >= cgi->pbufend)
    return(NULL);
  if (empty_string(cgi->pbuff))
    return(NULL);

  psp = PairNew(&cgi->pbuff,'=','&');

  for ( d = s = psp->name  ; *s ; *d++ = UrlDecodeChar(&s))
    ;
  *d = '\0';
  for ( d = s = psp->value ; *s ; *d++ = UrlDecodeChar(&s))
    ;
  *d = '\0';

  return(psp);
}

/*************************************************************************/

void (CgiListMake)(const Cgi cgi)
{
  struct pair *psp;
  
  assert(cgi != NULL);
  
  while((psp = CgiNextValue(cgi)) != NULL)
    ListAddTail(&cgi->vars,&psp->node);
}

/*************************************************************************/

size_t (CgiListGetValues)(const Cgi cgi,char ***darray,const char *name)
{
  size_t        size  = 0;
  size_t        idx   = 0;
  char        **store = NULL;
  struct pair  *pair;

  assert(darray != NULL);
  assert(name   != NULL);

  pair = CgiListGetPair(cgi,name);
  if (pair == NULL)
  {
    *darray = NULL;
    return(0);
  }

  while(NodeValid(&pair->node))
  {
    if (strcmp(pair->name,name) == 0)
    {
      if (idx == size)
      {
        store = realloc(store,size+256);
	size  += 256;
      }
      store[idx++] = pair->value;
    }
    pair = (struct pair *)NodeNext(&pair->node);
  }
  *darray = store;
  return(idx);
}

/************************************************************************/

int (CgiListRequired)(const Cgi cgi,struct dstring *table,size_t size)
{
  int cnt = 0;
  
  assert(cgi   != NULL);
  assert(table != NULL);
  assert(size  >  0);
  
  while(size)
  {
    if ((table->s2 = CgiListGetValue(cgi,table->s1)) == NULL)
      return(cnt);
    cnt++;
    table++;
    size--;
  }
  return(0);
}

/**********************************************************************/

int (CgiFree)(Cgi cgi)
{
  assert(cgi  != NULL);
  
  free(cgi->buffer);
  PairListFree(&cgi->vars);
  free(cgi);
  return 0;
}

/***********************************************************************/

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

char *UrlEncodeString(const char *src)
{
  size_t  nsize;
  char	 *dest;
  char	 *p;

  assert(src != NULL);

  nsize = strlen(src) * 3 + 1;
  dest	= malloc(nsize);

  for ( p = dest ; *src ; src++)
    p = UrlEncodeChar(p,*src);

  *p = 0;
  
  return dest;
}

/*********************************************************************/

char *UrlEncodeChar(char *dest,char c)
{
  div_t sdiv;

  assert(dest != NULL);

  if (ispunct(c) || iscntrl(c))
  {
    if ((c != '-') && (c != '@') && (c != '*') && (c != '_'))
    {
      *dest++ = '%';
      sdiv    = div(c,16);
      *dest++ = hextoc(sdiv.quot);
      *dest++ = hextoc(sdiv.rem);
      return(dest);
    }
  }
  else if (c == ' ')
    c = '+';
  *dest++ = c;
  return(dest);
}

/*************************************************************************/

char *UrlDecodeString(char *src)
{
  char *r;
  char *t;

  assert(src != NULL);

  for ( r = src , t = src ; *src ; t++)
    *t = UrlDecodeChar(&src);

  *t = '\0';
  return(r);
}

/***********************************************************************/

char UrlDecodeChar(char **psrc)
{
  char *src;
  int	c;

  assert(psrc  != NULL);
  assert(*psrc != NULL);

  src = *psrc;
  c   = *src++;
  if (c == '+')
    c = ' ';
  else if (c == '%')
  {
    assert(isxdigit(*src));
    assert(isxdigit(*(src+1)));
    c	 = ctohex(*src) * 16 + ctohex(*(src+1));
    src += 2;
  }
  *psrc = src;
  return(c);
}

/**************************************************************************/

