/***********************************************************************
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
************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <ctype.h>

#if defined(__GNUC__) && defined(__unix__)
#  include "libgen.h"
#else
#  define dirname(s)	""
#endif

#include "types.h"
#include "url.h"
#include "memory.h"
#include "ddt.h"
#include "util.h"
#include "http.h"
#include "errors.h"
#include "stream.h"
#include "rfc822.h"

/*************************************************************************/

static void	 adjust_cookies			(const HTTP,const char *[]);
static int	 http_checkstatus		(const HTTP,int);

/*************************************************************************/

static const char    *const modes[]    = { "GET" , "POST" , "HEAD" };

/**********************************************************************/

int (HttpConnect)(HTTP *phttp,int mode,URLHTTP url,const char *pcheaders[])
{
  HTTP         http;
  int          rc;
  int          ldup = FALSE;	/* MS WS bug workaround */
  size_t       i;
  char        *buffer;
  char         turl[BUFSIZ];
  char        *pt;
  struct pair *ppair;
  
  ddt(phttp     != NULL);
  ddt(mode      >= GET);
  ddt(mode      <= HEAD);
  ddt(url       != NULL);
  ddt(pcheaders != NULL);
  
  UrlMakeString((URL)url,turl,BUFSIZ);
  http          = MemAlloc(sizeof(struct http));
  http->mstatus = 0;
  rc            = UrlDup((URL *)&http->url,(URL)url);
  if (rc != ERR_OKAY)
  {
    MemFree(http);
    return(rc);
  }
  
  *phttp = http;
  rc     = TCPStream(http->bufio,url->host,url->port);
  if (rc != ERR_OKAY) 
  {
    /*-----------------------------------------
    ; We may have a bug around here ... turl seems 
    ; to be trashed!  We're also not cleaning up
    ; after ourselves.
    ;-----------------------------------------*/
    return(rc);
  }

  D(ddtlog(ddtstream,"$","CONNECTED TO: %a",turl);)  
  ListInit(&http->sheaders);

  /*---------------------------------------------
  ; go back to HTTP/1.0 for now, until I figure out how
  ; to handle 100s ... (let's try 1.1, then pause for 100s)
  ; but first, since we changed how the internal workings of URLHTTPs
  ; work, we need to to hack in some changes.
  ;------------------------------------------------*/
  
  {
    char *req  = UrlHttpRequest(url);
    char *host = UrlHttpHostPort(url);
    
    LineSFormat(http->bufio[1],"$ $","%a %b HTTP/1.0\r\n",modes[mode],req);
    LineSFormat(http->bufio[1],"$",  "Host: %a\r\n",host);
    
    MemFree(host);
    MemFree(req);
  }
  
  for (i = 0 ; pcheaders[i] != NULL ; i++)
    LineSFormat(http->bufio[1],"$","%a\r\n",pcheaders[i]);

  LineS(http->bufio[1],"\r\n");

  while(1)
  {
    StreamFlush(http->bufio[1]);
    buffer = LineSRead(http->bufio[0]);
    
    /*--------------------------------------------------------
    ; HTTP 0.9 does not sent back any header information, nor 
    ; does it send back a status code!
    ;--------------------------------------------------------*/
    
    if (strncmp(buffer,"HTTP",4) != 0)
    {
      for (i = strlen(buffer)+1 ; i ; i--)
        StreamUnRead(http->bufio[0],buffer[i]);
      http->version = HTTPv09;
      http->status  = HTTP_OKAY;
      MemFree(buffer);
      return(ERR_OKAY);
    }
    else
    {
      char *pv;
    
      pv = &buffer[5];	/* point just past HTTP/ */
      if (strncmp(pv,"1.0",3) == 0)
        http->version = HTTPv10;
      else if (strncmp(pv,"1.1",3) == 0)
        http->version = HTTPv11;
      else
        http->version = HTTPv10;
      
      for (pv += 4 ; isspace(*pv) ; pv++) /* skip to status value */
        ;
      rc = http_checkstatus(http,atoi(pv));
      if (rc != ERR_OKAY)
        return(rc);
    } 

    /*------------------------------------------------------------------
    ; there's a Microsoft Web Server running a particular CGI program that
    ; for some reason returns an exact duplicate of the headers.  Of course
    ; this screws up this part of the program, but that's what bug fixes
    ; are for.  
    ;
    ; Ah standards.  There are so many to choose from.  You can even make
    ; up your own.
    ;--------------------------------------------------------------------*/

    while(1)
    {
      ddt(buffer != NULL);
      MemFree(buffer);
      buffer = RFC822LineRead(http->bufio[0]);
      if (buffer == NULL) break;
      if (empty_string(buffer))
        break;
      
      if (strncmp(buffer,"HTTP/",5) == 0)  /* MS WS bug workaround */
        ldup = TRUE;
    
      if (ldup == FALSE)
      {
        pt           = buffer;
        ppair        = PairNew(&pt,':','\0');
        ppair->name  = trim_space(ppair->name);
        ppair->value = trim_space(ppair->value);
        up_string(ppair->name);
        ListAddTail(&http->sheaders,&ppair->node);
      }
    }
    
    ddt(buffer != NULL);
    MemFree(buffer);
    
    if (http->status >= HTTP_OKAY) 
      break;
  }
  
  return(ERR_OKAY);
}

/************************************************************************/

int (HttpOpen)(HTTP *phttp,int mode,URLHTTP url,const char *pcheaders[])
{
  int      retry = 0;
  int      rc;
  int      status;
  int      mstatus;		/* moved? */
  char    *tloc = NULL;
  URLHTTP  nurl = NULL;
  HTTP     http = NULL;
  
  ddt(phttp     != NULL);
  ddt(mode      >= GET);
  ddt(mode      <= HEAD);
  ddt(url       != NULL);
  ddt(pcheaders != NULL);
  
  mstatus = 0;
  while(retry < RETRIES)	/* as per RFC2068 */
  {
    rc = HttpConnect(phttp,mode,url,pcheaders);
    if (rc != ERR_OKAY)
      return(rc);
    status = HttpStatus(*phttp);
    if (
         (status == HTTP_MOVEPERM) 
	 || (status == HTTP_MOVETEMP)
	 || (status == HTTP_CREATED)
       )
    {
      http = *phttp;
      D(ddtlog(ddtstream,"i","redirect: %a",status);)
#ifdef DDT
      tloc = PairListGetValue(&http->sheaders,"SET-COOKIE");
      if (tloc)
        ddtlog(ddtstream,"$","forcing a cookie: [%a]",tloc);
#endif
      adjust_cookies(http,pcheaders);
      tloc = PairListGetValue(&http->sheaders,"LOCATION");
      if (tloc == NULL) 
        return(ERR_ERR);
      D(ddtlog(ddtstream,"$","redirecting to: [%a]",tloc);)
      if (nurl != NULL) UrlFree((URL *)&nurl);
      rc = UrlNew((URL *)&nurl,tloc);
      if (rc != ERR_OKAY)
      {
        /*-----------------------------------------------------------
	; Some servers return a relative URI, not an absolute URI.  So
	; again, code around server bugs ... (and some don't even use
	; a full path!  @#%@#$@# Microsoft!
	;------------------------------------------------------------*/

        if (rc != ERR_OKAY)
	{
	  char tmpbuf[BUFSIZ * 12];

	  url = (*phttp)->url;
	  if (*tloc == '/')
	  {
	    char *t = url->file;
	    url->file = tloc;
	    UrlMakeString((URL)url,tmpbuf,sizeof(tmpbuf));
	    url->file = t;
	  }
	  else
	  {
	    char fms[BUFSIZ];
	    char fms2[BUFSIZ];
	    char *t;
	    
	    /*----------------------------------------------------------
	    ; a complete hack here.  Sigh.  I hate this.  I really do.
	    ; not only is using dirname() here a hack, but in typical Unix
	    ; braindeath, it not only returns a static pointer, BUT it
	    ; *may* modify the string given it.  Grrrrrrrrr ... 
	    ;-----------------------------------------------------------*/
	    
	    D(ddtlog(ddtstream,"$ $","file: %a query: %b",url->file,url->query);)
	    strcpy(fms,url->file);
	    dirname(fms);
	    sprintf(fms2,"%s/%s",fms,tloc);
	    t = url->file;
	    url->file = fms2;
	    UrlMakeString((URL)url,tmpbuf,sizeof(tmpbuf));
	    url->file = t;
	  }
	  D(ddtlog(ddtstream,"$","new location: [%a]",tmpbuf);)
	  rc = UrlNew((URL *)&nurl,tmpbuf);
	  if (rc != ERR_OKAY)
	    return(rc);
	}
	else
	  return(rc); 
      }
      rc = HttpClose(phttp);
      if (rc != ERR_OKAY) return(rc);
      url = nurl;
      retry++;
      mstatus = status;
    }
    else
    {
      (*phttp)->mstatus = mstatus;
      return(ERR_OKAY);
    }
  }
  return(ERR_ERR);
} 

/**********************************************************************/

static void adjust_cookies(const HTTP conn,const char *headers[])
{
  char        *neocookie;
  char         tmpcookie[BUFSIZ];
  int          i;
  char        *setcookie;
  char        *p;
  size_t       sz;
  
  ddt(conn    != NULL);
  ddt(headers != NULL);
  
  for (i = 0 ; headers[i] != NULL ; i++)
  {
    if (
         (strncmp(headers[i],"Cookie:",7) == 0)
	 || (strncmp(headers[i],"X-Cookie:",9) == 0)
       )
    {
      /*------------------------------------------------------
      ; we have a cookie header, so we are sending/receiving
      ; cookies---quick hack---just replace the header with
      ; the new cookie.  The actual spec takes into account
      ; the various levels and domains and crap like that---I
      ; just want my current project to work.
      ; 
      ; I hate hacking up stuff, but full cookie support
      ; would require a whole rethink of how I do things
      ; around here.  Sigh.
      ;-------------------------------------------------------*/
      
      setcookie = PairListGetValue(&conn->sheaders,"SET-COOKIE");
      if ((setcookie == NULL) || (empty_string(setcookie)))
        return;		/* nothing to adjust */
      
      /*remove_ctrl(headers[i]);*/
      p = strchr(setcookie,';');
      if (p == NULL)
        neocookie = concat_strings("Cookie: ",setcookie,"\r\n",(char *)NULL);
      else
      {
        sz = p - setcookie;
        memcpy(tmpcookie,setcookie,sz);
        tmpcookie[sz] = '\0';
	neocookie = concat_strings("Cookie: ",tmpcookie,"\r\n",(char *)NULL);
      }
   
      D(ddtlog(ddtstream,"$","new cookie: [%a]",neocookie);)   
      headers[i] = neocookie;
      return;
    }
  }
}

/***********************************************************************/

struct pair *(HttpServerHeaders)(const HTTP http)
{
  ddt(http != NULL);
  return((struct pair *)ListGetHead(&http->sheaders));
}

/**********************************************************************/

char *(HttpGetServerHeader)(const HTTP http,const char *name)
{
  ddt(http != NULL);
  ddt(name != NULL);
  return(PairListGetValue(&http->sheaders,name));
}

/***********************************************************************/

Stream (HttpStreamRead)(const HTTP http)
{
  ddt(http != NULL);
  return(http->bufio[0]);
}

/***********************************************************************/

Stream (HttpStreamWrite)(const HTTP http)
{
  ddt(http != NULL);
  return(http->bufio[1]);
}

/**********************************************************************/

URLHTTP (HttpUrl)(const HTTP http)
{
  ddt(http != NULL);
  return(http->url);
}

/***********************************************************************/

int (HttpStatus)(const HTTP http)
{
  ddt(http != NULL);
  return(http->status);
}

/***********************************************************************/

int (HttpRedirected)(const HTTP http)
{
  ddt(http != NULL);
  return(http->mstatus);
}

/**********************************************************************/

int (HttpClose)(HTTP *phttp)
{
  HTTP http;
  int  rc;
  
  ddt(phttp  != NULL);
  ddt(*phttp != NULL);
  http = *phttp;
  rc   = UrlFree((URL *)&http->url);
  if (rc != ERR_OKAY) return(rc);
  rc   = StreamFree(http->bufio[0]);
  if (rc != ERR_OKAY) return(rc);
  rc   = StreamFree(http->bufio[1]);
  if (rc != ERR_OKAY) return(rc);
  PairListFree(&http->sheaders);
  MemFree(http);
  *phttp = NULL;
  return(ERR_OKAY);
}
 
/************************************************************************/

static const int c100[] =
{
  HTTP_100MAX,			/* highest status number for group 	*/
  HTTPv11,			/* rest are versions status is 		*/
  HTTPv11,			/* documented for			*/
};

static const int c200[] =
{
  HTTP_200MAX,
  HTTPv10,
  HTTPv10,
  HTTPv10,
  HTTPv11,
  HTTPv10,
  HTTPv11,
  HTTPv11,
};

static const int c300[] =
{
  HTTP_300MAX,
  HTTPv11,
  HTTPv10,
  HTTPv10,
  HTTPv11,
  HTTPv10,
  HTTPv11,
};

static const int c400[] =
{
  HTTP_400MAX,
  HTTPv10,
  HTTPv10,
  HTTPv11,
  HTTPv10,
  HTTPv10,
  HTTPv11,
  HTTPv11,
  HTTPv11,
  HTTPv11,
  HTTPv11,
  HTTPv11,
  HTTPv11,
  HTTPv11,
  HTTPv11,
  HTTPv11,
  HTTPv11,
};

static const int c500[] =
{
  HTTP_500MAX,
  HTTPv10,
  HTTPv10,
  HTTPv10,
  HTTPv10,
  HTTPv11,
  HTTPv11,
};

static const int *const tabs[] = { c100 , c200 , c300 , c400 , c500 };
  
/*--------------------------------------------------------------------*/  
  
static int http_checkstatus(const HTTP http,int status)
{
  const int *pd;
  div_t      l_stat; 
  
  l_stat = div(status,100);
  if ((l_stat.quot < 1) || (l_stat.quot > 5))
    return(HTTPERR_STATUS);
  pd = tabs[l_stat.quot - 1];
  if (status > pd[0])
    http->status = (l_stat.quot) * 100;
  else
  {
    if (http->version < pd[l_stat.rem + 1])
      http->status = (l_stat.quot) * 100;
    else
      http->status = status;
  }
  return(ERR_OKAY);
}

/***********************************************************************/

int (HttpVersion)(const char *protostring)
{
  ddt(protostring != NULL);
  
  if (*protostring == 0)
    return(HTTPv09);
  else if (strcmp(protostring,"HTTP/1.0") == 0)
    return(HTTPv10);
  else if (strcmp(protostring,"HTTP/1.1") == 0)
    return(HTTPv11);
  else
    return(HTTPv10);
}

/*********************************************************************/

int (HttpRetStatus)(int status,int version)
{
  const int *pd;
  div_t      l_stat;
  
  ddt(status >= 100);
  ddt(status <= 599);
  ddt((version == HTTPv09) || (version == HTTPv10) || (version == HTTPv11));
  
  l_stat = div(status,100);
  pd   = tabs[l_stat.quot - 1];
  if (status > pd[0])
    return(l_stat.quot * 100);
  else
  {
    if (version < pd[l_stat.rem + 1])
      return(l_stat.quot * 100);
    else
      return(status);
  }
  ddt(0);
}

/**************************************************************/

