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
#include <limits.h>
#include <string.h>
#include <errno.h>

#include "../memory.h"
#include "../errors.h"
#include "../util.h"
#include "../url.h"
#include "../ddt.h"

/**********************************************************************/

static int		 http_new	(URL,const char *);
static int		 http_normal	(URL,URL);
static int		 http_compare	(URL,URL);
static int		 http_makestring(URL,char *,size_t);
static int		 http_free	(URL);
static int		 http_nourl	(URLHTTP,int);
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

char *(UrlHttpRequest)(URLHTTP url)
{
  char *req;
  
  ddt(url != NULL);
  
  req = dup_string(url->file);
  if (url->params[0])
    req = concat_strings(req,";",url->params,(char *)NULL);
  
  if (url->query[0])
    req = concat_strings(req,"?",url->query,(char *)NULL);
  
  return(req);
}

/************************************************************************/

char *(UrlHttpHost)(URLHTTP url)
{
  return(dup_string(url->host));
}

/************************************************************************/

char *(UrlHttpHostPort)(URLHTTP url)
{
  if (url->port == 80)
    return(dup_string(url->host));
  else
  {
    char tmp[BUFSIZ];
    
    sprintf(tmp,":%d",url->port);
    return(concat_strings(url->host,tmp,(char *)NULL));
  }
}

/************************************************************************/

static int http_new(URL url,const char *surl)
{
  URLHTTP hurl = (URLHTTP)url;
  long    lport;
  char    tmpbuf[BUFSIZ];
  size_t  tmpsz;
  
  ddt(url  != NULL);
  ddt(surl != NULL);
  
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
  hurl->host = MemAlloc(tmpsz);
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
      return(http_nourl(hurl,ErrorPush(CgiErr,URLNEW,URLERR_PORTNUM,"$",tmpbuf)));
    if ((lport < 0) || (lport > PORTMAX))
      return(http_nourl(hurl,ErrorPush(CgiErr,URLNEW,URLERR_PORTNUM,"$",tmpbuf)));
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
    hurl->file = MemAlloc(tmpsz);
    strcpy(hurl->file,tmpbuf);
  }
  else
    hurl->file = dup_string("/");

  /*------------------------------------------------------
  ; params, query and fragment parts
  ;-------------------------------------------------------*/
  
    if (*surl == ';')	/* param part */
    {
      surl++;
      tmpsz = UrlGetFile(tmpbuf,BUFSIZ,&surl) + 1;
      if (tmpsz - 1)
      {
        hurl->params = MemAlloc(tmpsz);
        strcpy(hurl->params,tmpbuf);
      }
      else
        hurl->params = dup_string("");
    }
    else
      hurl->params = dup_string("");
  
    if (*surl == '?')	/* query part */
    {
      surl++;
      tmpsz = UrlGetFile(tmpbuf,BUFSIZ,&surl) + 1;
      if (tmpsz - 1)
      {
        hurl->query = MemAlloc(tmpsz);
        strcpy(hurl->query,tmpbuf);
      }
      else
        hurl->query = dup_string("");
    }
    else
      hurl->query = dup_string("");
  
  /*---------------------------------------------------------------
  ; fragment part (of file) if any
  ;----------------------------------------------------------------*/
  
  if (*surl == '#')
  {
    surl++;
    tmpsz          = strlen(surl) + 1;
    hurl->fragment = MemAlloc(tmpsz);
    strcpy(hurl->fragment,surl);
  }
  else
    hurl->fragment = dup_string("");
 
    
  return(ERR_OKAY);
}

/**********************************************************************/

static int http_normal(URL durl,URL surl)
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
  
  return(ErrorPush(CgiErr,URLNORMAL,URLERR_NOTIMP,""));
}

/**********************************************************************/

static int http_compare(URL durlb,URL surlb)
{
  URLHTTP durl = (URLHTTP)durlb;
  URLHTTP surl = (URLHTTP)surlb;
  int     rc;

  ddt(HTTP_check(durl));
  ddt(HTTP_check(surl));
  
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

static int http_makestring(URL urlb,char *d,size_t sd)
{
  URLHTTP         url = (URLHTTP)urlb;
  struct urlhttp  turl;
  char            bport    [33];	/* big enough for a decimal number between 0 65536 */
  char            bparams  [BUFSIZ];
  char            bquery   [BUFSIZ];
  char            bfragment[BUFSIZ];
  size_t          sproto;
  size_t          shost;
  size_t          sport;
  size_t          sfile;
  size_t          sparams;
  size_t          squery;
  size_t          sfragment;
    
  ddt(d      != NULL);
  ddt(sd     >  0);
  ddt(HTTP_check(url));

  bport    [0] = '\0';
  bparams  [0] = '\0';
  bquery   [0] = '\0';
  bfragment[0] = '\0';

  turl.protocol = url->protocol;
  sproto        = strlen(turl.protocol);
  
  if (url->host[0])
  {
    turl.host = url->host;
    turl.port = url->port;
    shost     = strlen(turl.host) + 2;	/* host + "//" */
    if (turl.port != 80)
      sport = formatstr(bport,sizeof(bport),"i",":%a",turl.port);
    else
    {
      bport[0] = '\0';
      sport    = 0;
    } 
  }
  else
  {
    turl.host = "";
    turl.port = 0;
    shost     = 0;
    sport     = 0;
  }
  
  ddt(url->file);
  turl.file = url->file;
  sfile     = strlen(turl.file);
  
  if (url->params[0])
  {
    turl.params = url->params;
    sparams     = formatstr(bparams,sizeof(bparams),"$",";%a",turl.params);
  }
  else
  {
    turl.params = "";
    sparams     = 0;
  }
  
  if (url->query[0])
  {
    turl.query = url->query;
    squery     = formatstr(bquery,sizeof(bquery),"$","?%a",turl.query);
  }
  else
  {
    turl.query = "";
    squery     = 0;
  }
  
  if (url->fragment[0])
  {
    turl.fragment = url->fragment;
    sfragment     = formatstr(bfragment,sizeof(bfragment),"$","#%a",turl.fragment);
  }
  else
  {
    turl.fragment = "";
    sfragment     = 0;
  }
  
  if (sd < (sproto + shost + sport + sfile + sparams + squery + sfragment + 1))
  {
    ddt(0);
    return(ErrorPush(CgiErr,URLMAKESTRING,URLERR_BUFFER,""));
  }
  
  formatstr(d,sd,"$ $ $ $ $ $ $","%a://%b%c%d%e%f%g",turl.protocol,turl.host,bport,turl.file,bparams,bquery,bfragment);
  return(ERR_OKAY);
}

/**************************************************************************/

static int http_free(URL url)
{
  URLHTTP purl = (URLHTTP)url;

  ddt(HTTP_check(purl));
  /*ddt(url != (URL)&httpself);*/

  purl->vector = NULL;  
  MemFree(purl->protocol,strlen(purl->protocol) + 1);
  MemFree(purl->host,    strlen(purl->host)     + 1);
  MemFree(purl->file,    strlen(purl->file)     + 1);
  MemFree(purl->params,  strlen(purl->params)   + 1);
  MemFree(purl->query,   strlen(purl->query)    + 1);
  MemFree(purl->fragment,strlen(purl->fragment) + 1);
  purl->port = 0;
  return(ERR_OKAY);
}

/*************************************************************************/

#ifdef DDT
  static int HTTP_check(struct urlhttp *purl)
  {
    if (purl == NULL) 				return(0);
    if (purl->protocol == NULL) 		return(0);
    if (strcmp(purl->protocol,"http") != 0)	return(0);
    if (purl->host     == NULL) 		return(0);
    if (purl->port     <  0)    		return(0);
    if (purl->port     >  PORTMAX) 		return(0);
    if (purl->file     == NULL)			return(0);
    if (purl->params   == NULL)                 return(0);
    if (purl->query    == NULL)			return(0);
    if (purl->fragment == NULL) 		return(0);
    return(1);
  }
#endif

/************************************************************************/

static int http_nourl(URLHTTP url,int error)
{
  URLHTTP purl = (URLHTTP)url;

  ddt(HTTP_check(purl));
  /*ddt(url != (URLHTTP)&httpself);*/

  purl->vector = NULL;  
  if (purl->protocol)	MemFree(purl->protocol,strlen(purl->protocol)+1);
  if (purl->host)	MemFree(purl->host,    strlen(purl->host)+1);
  if (purl->file)	MemFree(purl->file,    strlen(purl->file)+1);
  if (purl->params)     MemFree(purl->params,  strlen(purl->params)+1);
  if (purl->query)      MemFree(purl->query,   strlen(purl->query)+1);
  if (purl->fragment)	MemFree(purl->fragment,strlen(purl->fragment)+1);
  purl->port = 0;
  return(error);
}

/**************************************************************************/

