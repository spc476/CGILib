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

#define _GNU_SOURCE 1

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "../errors.h"
#include "../url.h"

/**********************************************************************/

static int		 http_new	(URL,const char *);
static int		 http_normal	(URL *,URL);
static int		 http_compare	(URL,URL);
static char             *http_makestring(URL);
static int		 http_free	(URL);
#ifdef DDT
  static int		 HTTP_check	(struct urlhttp *);
#endif

/***********************************************************************/

const struct urlvector httpvec = 
{
  http_new,
  http_normal,
  http_compare,
  http_makestring,
  http_free
};

/********************************************************************/

char *(UrlHttpRequest)(struct urlhttp *url)
{
  FILE   *out;
  char   *req;
  size_t  size;
  
  assert(url != NULL);
  
  out = open_memstream(&req,&size);  
  fputs(url->file,out);

  if (url->params[0])
    fprintf(out,";%s",url->params);
  
  if (url->query[0])
    fprintf(out,"?%s",url->query);
  
  fclose(out);
  return (req);
}

/************************************************************************/

char *(UrlHttpHost)(struct urlhttp *url)
{
  return strdup(url->host);
}

/************************************************************************/

char *(UrlHttpHostPort)(struct urlhttp *url)
{
  if (url->port == 80)
    return strdup(url->host);
  else
  {
    char tmp[BUFSIZ];
    
    sprintf(tmp,"%s:%d",url->host,url->port);
    return strdup(tmp);
  }
}

/************************************************************************/

static int http_new(URL url,const char *surl)
{
  struct urlhttp *hurl;
  long            lport;
  char            tmpbuf[BUFSIZ];
  size_t          tmpsz;
  
  assert(url  != NULL);
  assert(surl != NULL);
  
  hurl = &url->http;
  
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
  ;-----------------------------------------------------------------*/

  /*------------------------------------------------
  ; host portion, if any
  ;-------------------------------------------------*/

  tmpsz      = UrlGetHost(tmpbuf,BUFSIZ,&surl) + 1;  
  hurl->host = malloc(tmpsz);
  strcpy(hurl->host,tmpbuf);
  
  /*-------------------------------------------------------
  ; optional port number
  ;------------------------------------------------------*/
  
  tmpsz = UrlGetPort(tmpbuf,BUFSIZ,&surl);
  if (tmpsz)
  {
    errno = 0;
    lport = strtol(tmpbuf,NULL,10);
    if ((errno == ERANGE) && ((lport == LONG_MAX) || (lport == LONG_MIN)))
      return(ERR_ERR);
    if ((lport < 0) || (lport > PORTMAX))
      return(ERR_ERR);
    hurl->port = lport;
  }
  else
    hurl->port = 80;
    
  /*------------------------------------------------------------------
  ; parse the file part
  ;-----------------------------------------------------------------*/
  
  tmpsz = UrlGetFile(tmpbuf,BUFSIZ,&surl) + 1;
  if (tmpsz-1)
  {
    hurl->file = malloc(tmpsz);
    strcpy(hurl->file,tmpbuf);
  }
  else
    hurl->file = strdup("/");

  /*------------------------------------------------------
  ; params, query and fragment parts
  ;-------------------------------------------------------*/
  
    if (*surl == ';')	/* param part */
    {
      surl++;
      tmpsz = UrlGetFile(tmpbuf,BUFSIZ,&surl) + 1;
      if (tmpsz - 1)
      {
        hurl->params = malloc(tmpsz);
        strcpy(hurl->params,tmpbuf);
      }
      else
        hurl->params = strdup("");
    }
    else
      hurl->params = strdup("");
  
    if (*surl == '?')	/* query part */
    {
      surl++;
      tmpsz = UrlGetFile(tmpbuf,BUFSIZ,&surl) + 1;
      if (tmpsz - 1)
      {
        hurl->query = malloc(tmpsz);
        strcpy(hurl->query,tmpbuf);
      }
      else
        hurl->query = strdup("");
    }
    else
      hurl->query = strdup("");
  
  /*---------------------------------------------------------------
  ; fragment part (of file) if any
  ;----------------------------------------------------------------*/
  
  if (*surl == '#')
  {
    surl++;
    tmpsz          = strlen(surl) + 1;
    hurl->fragment = malloc(tmpsz);
    strcpy(hurl->fragment,surl);
  }
  else
    hurl->fragment = strdup("");
 
    
  return(ERR_OKAY);
}

/**********************************************************************/

static int http_normal(URL *durl,URL surl)
{
  /*--------------------------------------------------------------------
  ; the normalization of a URL.  In this case, http:.  
  ;
  ; Some comments:
  ;
  ; some comments about normalizing the DNS name is located:
  ;  	http://info.webcrawler.com/mailing-lists/robots/0736.html
  ;	http://info.webcrawler.com/mailing-lists/robots/0089.html
  ; 
  ; Some comments about spidering:
  ;
  ; 	http://www.w3.org/pub/WWW/Search/9605-Indexing-Workshop/
  ;		ReportOutcomes/Spidering.txt
  ;
  ; Some comments on robots:
  ;	http://www.kollar.com/robots.html
  ;
  ;
  ; anyway ... 
  ;
  ;--------------------------------------------------------------------*/
  
  return(ERR_NOTIMP);
}

/**********************************************************************/

static int http_compare(URL durlb,URL surlb)
{
  struct urlhttp *durl;
  struct urlhttp *surl;
  int             rc;

  durl = &durlb->http;
  surl = &surlb->http;

#ifdef DDT  
  assert(HTTP_check(durl));
  assert(HTTP_check(surl));
#endif

  rc = strcmp(durl->protocol,surl->protocol);
  if (rc != 0) return(rc);
  rc = strcmp(durl->host,surl->host);
  if (rc != 0) return(rc);
  if (durl->port != surl->port) return(1);
  rc = strcmp(durl->file,surl->file);
  if (rc != 0) return(rc);
  rc = strcmp(durl->params,surl->params);
  if (rc != 0) return(rc);
  rc = strcmp(durl->query,surl->query);
  if (rc != 0) return(rc);
  return(strcmp(durl->fragment,surl->fragment));
}

/**********************************************************************/

static char *http_makestring(URL url)
{
  FILE   *out;
  char   *res;
  size_t  size;
  
  out = open_memstream(&res,&size);
  
  fprintf(out,"http://%s",url->http.host);
  if (url->http.port != 80)
    fprintf(out,":%d",url->http.port);
  
  fprintf(out,"%s",url->http.file);
  
  if (url->http.params[0])
    fprintf(out,";%s",url->http.params);
  
  if (url->http.query[0])
    fprintf(out,"?%s",url->http.query);
  
  if (url->http.fragment[0])
    fprintf(out,"#%s",url->http.fragment);
  
  fclose(out);
  return res;
}

/**************************************************************************/

static int http_free(URL url)
{
  struct urlhttp *purl;

  purl = &url->http;

#ifdef DDT
  assert(HTTP_check(&purl->http));
#endif

  free(purl->protocol);
  free(purl->host);
  free(purl->file);
  free(purl->params);
  free(purl->query);
  free(purl->fragment);
  free(url);

  return(ERR_OKAY);
}

/*************************************************************************/

#ifdef DDT
  static int HTTP_check(struct urlhttp *purl)
  {
    if (purl                          == NULL) 	  return(0);
    if (purl->protocol                == NULL) 	  return(0);
    if (strcmp(purl->protocol,"http") != 0)	  return(0);
    if (purl->host                    == NULL) 	  return(0);
    if (purl->port                    <  0)    	  return(0);
    if (purl->port                    >  PORTMAX) return(0);
    if (purl->file                    == NULL)	  return(0);
    if (purl->params                  == NULL)    return(0);
    if (purl->query                   == NULL)	  return(0);
    if (purl->fragment                == NULL) 	  return(0);
    return(1);
  }
#endif

/************************************************************************/

