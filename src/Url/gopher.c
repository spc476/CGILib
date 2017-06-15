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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "../url.h"

/**********************************************************************/

static int    gopher_new        (url__t *restrict,char const *restrict);
static int    gopher_compare    (url__t const *const restrict,url__t const *const restrict);
static size_t gopher_makestring (url__t const *const restrict,char *restrict,size_t);
static void   gopher_free       (url__t *);

/***********************************************************************/

struct urlvector const g_gophervec =
{
  gopher_new,
  gopher_compare,
  gopher_makestring,
  gopher_free
};

/********************************************************************/

static int gopher_new(url__t *restrict url,char const *surl)
{
  urlgopher__t *hurl = &url->gopher;
  char         *next;
  char          tmpbuf[BUFSIZ];
  size_t        tmpsz;
  size_t        len;
  
  assert(url  != NULL);
  assert(surl != NULL);
  
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
    hurl->port = 70;
    
  if (*surl == '\0')
  {
    hurl->type     = GOPHER_DIR;
    hurl->selector = strdup("");
    hurl->search   = strdup("");
    hurl->plus     = strdup("");
    return 0;
  }
  else if (*surl != '/')
  {
    free(hurl->host);
    return EINVAL;
  }
  
  surl++;
  
  /*------------------------------------------------------------------
  ; parse the type
  ;-----------------------------------------------------------------*/
  
  hurl->type = *surl++;
  
  /*------------------------------------
  ; Path (actually, the selector)
  ;-------------------------------------*/
  
  next = strstr(surl,"%09");
  if (next == NULL)
  {
    hurl->selector = strdup(surl);
    hurl->search   = strdup("");
    hurl->plus     = strdup("");
    return 0;
  }
  
  /*-----------------------------------------------------------------------
  ; We found the '%09', copy everything up to it in selector, and bump past it.
  ;------------------------------------------------------------------------*/
  
  len            = (size_t)(next - surl) + 1;
  hurl->selector = malloc(len);
  if (hurl->selector == NULL)
  {
    free(hurl->host);
    return ENOMEM;
  }
  memcpy(hurl->selector,surl,len);
  surl += 3;
  
  /*------------------------------------------
  ; if we're up against Sark, return an error.
  ;-------------------------------------------*/
  
  if (*surl == '\0')
  {
    free(hurl->selector);
    free(hurl->host);
    return EINVAL;
  }
  
  /*--------------------------------
  ; handle the search portion
  ;---------------------------------*/
  
  next = strstr(surl,"%09");
  if (next == NULL)
  {
    hurl->search = strdup(surl);
    hurl->plus   = strdup("");
    return 0;
  }
  
  /*------------------------------
  ; we have a plus selector!
  ;-------------------------------*/
  
  len = (size_t)(next - surl) + 1;
  hurl->search = malloc(len);
  if (hurl->search == NULL)
  {
    free(hurl->selector);
    free(hurl->host);
    return ENOMEM;
  }
  memcpy(hurl->search,surl,len);
  surl += 3;
  
  if (*surl == '\0')
  {
    free(hurl->search);
    free(hurl->selector);
    free(hurl->host);
    return EINVAL;
  }
  
  hurl->plus = strdup(surl);
  return 0;
}

/**********************************************************************/

static int gopher_compare(
        url__t const *const restrict durl,
        url__t const *const restrict surl
)
{
  int rc;
  
  assert(durl         != NULL);
  assert(durl->scheme == URL_GOPHER);
  assert(surl         != NULL);
  assert(surl->scheme <  URL_max);
  
  rc = durl->scheme - surl->scheme;
  if (rc != 0) return rc;
  rc = strcmp(durl->gopher.host,surl->gopher.host);
  if (rc != 0) return rc;
  rc = durl->gopher.port - surl->gopher.port;
  if (rc != 0) return rc;
  rc = durl->gopher.type - surl->gopher.type;
  if (rc != 0) return rc;
  rc = strcmp(durl->gopher.selector,surl->gopher.selector);
  if (rc != 0) return rc;
  rc = strcmp(durl->gopher.search,surl->gopher.search);
  if (rc != 0) return rc;
  return strcmp(durl->gopher.plus,surl->gopher.plus);
}

/**********************************************************************/

static size_t gopher_makestring(
        url__t const *const restrict url,
        char         *restrict       d,
        size_t                       sd
)
{
  char port[7];
  
  assert(url                  != NULL);
  assert(url->scheme          == URL_GOPHER);
  assert(url->gopher.host     != NULL);
  assert(url->gopher.port     >= 0);
  assert(url->gopher.port     <= PORTMAX);
  assert(url->gopher.type     != '\0');
  assert(url->gopher.selector != NULL);
  assert(url->gopher.search   != NULL);
  assert(url->gopher.plus     != NULL);
  assert(d                    != NULL);
  assert(sd                   >  0);
  
  if (url->gopher.port == 70)
    port[0] = '\0';
  else
    snprintf(port,sizeof(port),":%d",url->gopher.port);
    
  return snprintf(
        d,
        sd, /*     h N  t p > s > + */
        "gopher://%s%s/%c%s%s%s%s%s",
        url->gopher.host,
        port,
        url->gopher.type,
        url->gopher.selector,
        (*url->gopher.search != '\0') ? "%09" : "",
        url->gopher.search,
        (*url->gopher.plus   != '\0') ? "%09" : "",
        url->gopher.plus
  );
}

/***********************************************************************/

static void gopher_free(url__t *url)
{
  assert(url != NULL);
  assert(url->scheme == URL_GOPHER);
  
  free(url->gopher.host);
  free(url->gopher.selector);
  free(url->gopher.search);
  free(url->gopher.plus);
  free(url);
}

/*************************************************************************/

