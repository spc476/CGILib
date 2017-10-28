/***************************************************************************
*
* Copyright 2001,2011,2013 by Sean Conner.
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
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "../url.h"

/**********************************************************************/

static int    http_new        (url__t *restrict,char const *restrict);
static int    http_compare    (url__t const *const restrict,url__t const *const restrict);
static size_t http_makestring (url__t const *const restrict,char *restrict,size_t);
static void   http_free       (url__t *);

/***********************************************************************/

struct urlvector const g_httpvec =
{
  http_new,
  http_compare,
  http_makestring,
  http_free
};

/********************************************************************/

static int http_new(url__t *restrict url,char const *surl)
{
  urlhttp__t *hurl = &url->http;
  char        tmpbuf[BUFSIZ];
  size_t      tmpsz;
  
  assert(url  != NULL);
  assert(surl != NULL);
  
  /*-----------------------------------------------------------------
  ; at this point, durl->protocol is set, and url points just past the
  ; ':' in the url spec
  ;
  ; durl->vector is also set.
  ;
  ; The spec (RFC-1808) is
  ; http:[//host.domain.top[:port]][/path/file[;parms][?query][#fragment]]
  ;
  ; technically, everything but the scheme may be absent, but for now,
  ; we're hacking it up a bit ...
  ;
  ; we're also ignoring the parms portion for now.
  ;-----------------------------------------------------------------*/
  
  /*------------------------------------------------
  ; host portion, if any
  ;-------------------------------------------------*/
  
  UrlGetHost(tmpbuf,BUFSIZ,&surl);
  hurl->host = strdup(tmpbuf);
  
  /*-------------------------------------------------------
  ; optional port number
  ;------------------------------------------------------*/
  
  tmpsz = UrlGetPort(tmpbuf,BUFSIZ,&surl);
  if (tmpsz)
  {
    long lport;
    
    errno = 0;
    lport = strtol(tmpbuf,NULL,10);
    if ((errno == ERANGE) && ((lport == LONG_MAX) || (lport == LONG_MIN)))
      return(ERANGE);
    if ((lport < 0) || (lport > PORTMAX))
      return(EDOM);
    hurl->port = lport;
  }
  else
    hurl->port = 80;
    
  /*------------------------------------------------------------------
  ; parse the file part
  ;-----------------------------------------------------------------*/
  
  tmpsz = UrlGetFile(tmpbuf,BUFSIZ,&surl);
  if (tmpsz)
    hurl->path = strdup(tmpbuf);
  else
    hurl->path = strdup("/");
    
  /*------------------------------------------------------
  ; params, query and fragment parts
  ;-------------------------------------------------------*/
  
  if (*surl == '?')     /* query part */
  {
    surl++;
    tmpsz = UrlGetFile(tmpbuf,BUFSIZ,&surl);
    if (tmpsz)
      hurl->query = strdup(tmpbuf);
    else
      hurl->query = strdup("");
  }
  else
    hurl->query = strdup("");
    
  /*---------------------------------------------------------------
  ; fragment part (of file) if any
  ;----------------------------------------------------------------*/
  
  if (*surl == '#')
    hurl->fragment = strdup(++surl);
  else
    hurl->fragment = strdup("");
    
  return(0);
}

/**********************************************************************/

static int http_compare(
        url__t const *const restrict durl,
        url__t const *const restrict surl
)
{
  int rc;
  
  assert(durl         != NULL);
  assert(durl->scheme == URL_HTTP);
  assert(surl         != NULL);
  assert(surl->scheme <  URL_max);
  
  rc = durl->scheme - surl->scheme;
  if (rc != 0) return rc;
  rc = strcmp(durl->http.host,surl->http.host);
  if (rc != 0) return rc;
  rc = durl->http.port - surl->http.port;
  if (rc != 0) return rc;
  rc = strcmp(durl->http.path,surl->http.path);
  if (rc != 0) return rc;
  rc = strcmp(durl->http.query,surl->http.query);
  if (rc != 0) return rc;
  return strcmp(durl->http.fragment,surl->http.fragment);
}

/**********************************************************************/

static size_t http_makestring(
        url__t const *const restrict url,
        char         *restrict       d,
        size_t                       sd
)
{
  char port[7];
  
  assert(url                != NULL);
  assert(url->scheme        == URL_HTTP);
  assert(url->http.host     != NULL);
  assert(url->http.port     >= 0);
  assert(url->http.port     <= PORTMAX);
  assert(url->http.path     != NULL);
  assert(url->http.query    != NULL);
  assert(url->http.fragment != NULL);
  assert(d                  != NULL);
  assert(sd                 >  0);
  
  if (url->http.port == 80)
    port[0] = '\0';
  else
    snprintf(port,sizeof(port),":%d",url->http.port);
    
  return snprintf(
        d,
        sd, /*   h N p ? q # f */
        "http://%s%s%s%s%s%s%s",
        url->http.host,
        port,
        url->http.path,
        (*url->http.query != '\0')    ? "?" : "",
        url->http.query,
        (*url->http.fragment != '\0') ? "#" : "",
        url->http.fragment
  );
}

/***********************************************************************/

static void http_free(url__t *url)
{
  assert(url != NULL);
  assert(url->scheme == URL_HTTP);
  
  free(url->http.host);
  free(url->http.path);
  free(url->http.query);
  free(url->http.fragment);
  free(url);
}

/*************************************************************************/

