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

#include "../util.h"
#include "../pair.h"
#include "../cgi.h"
#include "../stream.h"
#include "../nodelist.h"
#include "../errors.h"
#include "../types.h"
#include "../ddt.h"
#include "../memory.h"
#include "../http.h"

/**************************************************************************/

static int		 cgi_create	(Cgi *,void *);
static int		 cgi_new_get	(const Cgi);
static int		 cgi_new_post	(const Cgi);
static int		 cgi_new_head	(const Cgi);
static int		 cgi_new_put	(const Cgi);
static void		 cgicookie_new	(const Cgi,const char *,size_t);

/*************************************************************************/

int (CgiNew)(Cgi *pcgi,void *data)
{
  char *request_method;
  int   rc;
  
  ddt(pcgi != NULL);
  
  if ((rc = cgi_create(pcgi,data)) != ERR_OKAY)
    return(rc);
  
  request_method = CgiEnvGet(*pcgi,"REQUEST_METHOD");
   
  if (strcmp(request_method,"GET") == 0)
    rc = cgi_new_get(*pcgi);
  else if (strcmp(request_method,"POST") == 0)
    rc = cgi_new_post(*pcgi);
  else if (strcmp(request_method,"HEAD") == 0)
    rc = cgi_new_head(*pcgi);
  else if (strcmp(request_method,"PUT") == 0)
    rc = cgi_new_put(*pcgi);
  else
  {
    MemFree(*pcgi);
    *pcgi = NULL;
    return(ERR_ERR);
  }

  return(rc);
}

/*******************************************************************/

#ifdef STDCGI
#  ifdef __unix__
     static int cgi_create(Cgi *pcgi,void *data)
     {
       struct cgi *cgi;
       
       ddt(pcgi != NULL);
       
       cgi       = MemAlloc(sizeof(struct cgi));
       cgi->data = data;
       *pcgi     = cgi;
       
       ListInit(&cgi->vars);
       return(ERR_OKAY);
     }
#  endif
#endif

/******************************************************************/

static int cgi_new_get(const Cgi cgi)
{
  char   *query_string;
  size_t  qs;

  ddt(cgi != NULL);
  
  query_string = CgiEnvGet(cgi,"QUERY_STRING");
  qs           = strlen(query_string);
  cgicookie_new(cgi,query_string,qs);
  cgi->method = GET;
  return(ERR_OKAY);
}

/***************************************************************/

static void cgicookie_new(const Cgi cgi,const char *data,size_t size)
{
  ddt(cgi  != NULL);
  ddt(data != NULL);
  
  if (size)
  {
    cgi->buffer = MemAlloc(size + 2);
    memcpy(cgi->buffer,data,size);
    cgi->buffer[size]     = '&';
    cgi->buffer[size + 1] = '\0';
    cgi->bufsize          = size + 1;
  }
  else
  {
    cgi->buffer    = MemAlloc(1);
    cgi->buffer[0] = '\0';
    cgi->bufsize   = 0;
  }
  cgi->pbuff   = cgi->buffer;
  cgi->pbufend = &cgi->buffer[size + 1];
}

/*********************************************************************/

static int cgi_new_post(const Cgi cgi)
{
  size_t  length;
  char   *content_type;
  char   *content_length;
  size_t  i;
  
  ddt(cgi != NULL);
  
  content_type   = CgiEnvGet(cgi,"CONTENT_TYPE");
  content_length = CgiEnvGet(cgi,"CONTENT_LENGTH");
  
  if (strcmp(content_type,"application/x-www-form-urlencoded") != 0)
    return(ERR_ERR);
    
  errno  = 0;
  length = strtoul(content_length,NULL,10);
  if ((length == LONG_MAX) && (errno == ERANGE))
    return(ERR_ERR);
    
  cgi->buffer = MemAlloc(length+2);
  memset(cgi->buffer,'\0',length+2);
  cgi->bufsize          = length+1;
  cgi->buffer[length+1] = '&';

  /* XXX --- need a better way of doing this */

  for ( i = 0 ; i < length ; i++)
    cgi->buffer[i] = StreamRead(StdinStream);
    
  cgi->pbuff   = cgi->buffer;
  cgi->pbufend = &cgi->buffer[cgi->bufsize+1];
  cgi->method  = POST;
  return(ERR_OKAY);
}

/**********************************************************************/

static int cgi_new_head(const Cgi cgi)
{
  ddt(cgi != NULL);
  
  cgi->method = HEAD;
  return(ERR_OKAY);
}

/***************************************************************/

static int cgi_new_put(const Cgi cgi)
{
  size_t  length;
  char   *content_length;
  size_t  i;
  
  ddt(cgi != NULL);

  content_length = CgiEnvGet(cgi,"CONTENT_LENGTH");
  
  errno  = 0;
  length = strtoul(content_length,NULL,10);
  if ((length == LONG_MAX) && (errno == ERANGE))
    return(ERR_ERR);
  
  cgi->buffer = MemAlloc(length);
  memset(cgi->buffer,'\0',length);
  cgi->bufsize = length;

  for (i = 0 ; i < length ; i++)
    cgi->buffer[i] = StreamRead(StdinStream);
    
  cgi->pbuff   = cgi->buffer;
  cgi->pbufend = &cgi->buffer[cgi->bufsize];
  cgi->method  = PUT;
  return(ERR_OKAY);
}

/*************************************************************/
    
void (CgiGetRawData)(const Cgi cgi,char **pdest,size_t *psize)
{
  ddt(cgi   != NULL);
  ddt(pdest != NULL);
  ddt(psize != NULL);
  
  *pdest = cgi->buffer;
  *psize = cgi->bufsize;
}

/************************************************************************/

void (CgiOutHtml)(const Cgi cgi)
{
  static const char msg[] = "Content-type: text/html\n\n";
  
  ddt(cgi != NULL);

  LineS(StdoutStream,msg);
}

/***********************************************************************/

void (CgiOutText)(const Cgi cgi)
{
  static const char msg[] = "Content-type: text/plain\n\n";

  ddt(cgi != NULL);

  LineS(StdoutStream,msg);
}

/************************************************************************/

void (CgiOutShtml)(const Cgi cgi)
{
  static const char msg[] = "Content-type: text/x-server-parsed-html\n\n";
  
  ddt(cgi != NULL);

  LineS(StdoutStream,msg);  
}

/**************************************************************************/

void (CgiOutLocation)(const Cgi cgi,const char *location)
{
  ddt(cgi      != NULL);
  ddt(location != NULL);

  LineSFormat(StdoutStream,"$","Location: %a\n\n",location);  
}

/*******************************************************************/

int (CgiMethod)(const Cgi cgi)
{
  ddt(cgi != NULL);
  return(cgi->method);
}

/********************************************************************/

#ifdef STDCGI
#  ifdef __unix__
     char *(CgiEnvGet)(const Cgi cgi,const char *name)
       {
         ddt(cgi  != NULL);
         ddt(name != NULL);

         return(spc_getenv(name));
       }
#  endif
#endif

/*********************************************************************/

struct pair *(CgiNextValue)(const Cgi cgi)
{
  struct pair *psp;
  char        *s;
  char        *d;
  
  ddt(cgi != NULL);
  
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
  
  ddt(cgi != NULL);
  
  while((psp = CgiNextValue(cgi)) != NULL)
    ListAddTail(&cgi->vars,&psp->node);
}

/*************************************************************************/

struct pair *(CgiListFirst)(const Cgi cgi)
{
  ddt(cgi != NULL);
  return(PairListFirst(&cgi->vars));
}

/*************************************************************************/

struct pair *(CgiListGetPair)(const Cgi cgi,const char *name)
{
  ddt(cgi  != NULL);
  ddt(name != NULL);
  return(PairListGetPair(&cgi->vars,name));
}

/***********************************************************************/
      
char *(CgiListGetValue)(const Cgi cgi,const char *name)
{
  ddt(cgi  != NULL);
  ddt(name != NULL);
  return(PairListGetValue(&cgi->vars,name));
}

/*********************************************************************/

size_t (CgiListGetValues)(const Cgi cgi,char ***darray,const char *name)
{
  size_t        size  = 0;
  size_t        idx   = 0;
  char        **store = NULL;
  struct pair  *pair;

  ddt(darray != NULL);
  ddt(name   != NULL);

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
        store = MemResize(store,size+256);
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
  
  ddt(cgi   != NULL);
  ddt(table != NULL);
  ddt(size  >  0);
  
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

int (CgiFree)(Cgi *pcgi)
{
  Cgi cgi;
  
  ddt(pcgi  != NULL);
  ddt(*pcgi != NULL);
  
  cgi = *pcgi;
  MemFree(cgi->buffer);
  PairListFree(&cgi->vars);
  MemFree(cgi);
  *pcgi = NULL;
  return(ERR_OKAY);
}

/***********************************************************************/

int (CookieNew)(Cookie *pmonster,const Cgi cgi)
{
  Cookie  monster;
  char   *http_cookie;
  size_t  cs;
  
  ddt(pmonster != NULL);
  ddt(cgi      != NULL);
  
  monster     = MemAlloc(sizeof(struct cgi));
  http_cookie = CgiEnvGet(cgi,"HTTP_COOKIE");
  cs          = strlen(http_cookie);
  *pmonster   = monster;
  ListInit(&monster->vars);
  cgicookie_new((Cgi)monster,http_cookie,cs);
  return(ERR_OKAY);
}

/***********************************************************************/

struct pair *(CookieNextValue)(const Cookie monster)
{
  ddt(monster != NULL);  
  return(CgiNextValue((Cgi)monster));
}

/*************************************************************************/

void (CookieListMake)(const Cookie monster)
{
  ddt(monster != NULL);
  CgiListMake((Cgi)monster);
}

/**********************************************************************/

struct pair *(CookieListFirst)(const Cookie monster)
{
  ddt(monster != NULL);
  return(PairListFirst(&monster->vars));
}

/*************************************************************************/

struct pair *(CookieListGetPair)(const Cookie monster,const char *name)
{
  ddt(monster != NULL);
  ddt(name    != NULL);
  return(PairListGetPair(&monster->vars,name));
}

/***********************************************************************/

char *(CookieListGetValue)(const Cookie monster,const char *name)
{
  ddt(monster != NULL);
  ddt(name    != NULL);
  return(PairListGetValue(&monster->vars,name));
}

/*********************************************************************/

int (CookieFree)(Cookie *pmonster)
{
  Cookie monster;
  
  ddt(pmonster  != NULL);
  ddt(*pmonster != NULL);
  
  monster = *pmonster;
  PairListFree(&monster->vars);
  MemFree(monster->buffer);
  MemFree(monster);
  *pmonster = NULL;
  return(ERR_OKAY);
}

/***********************************************************************/

