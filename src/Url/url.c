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
#include <ctype.h>

#define URL_C
#include "../url.h"
#include "../memory.h"
#include "../errors.h"
#include "../util.h"
#include "../ddt.h"

/**********************************************************************/

extern struct urlvector httpvec;
extern struct urlvector filevec;

static const struct urlrelation
{
  char		   *const proto;
  int		    type;
  struct urlvector *const puv;
  size_t	    size;
} protos[] =
{
  { "http"      , URL_HTTP      , &httpvec      , sizeof(struct urlhttp)     },
  { "file"      , URL_FILE      , &filevec      , sizeof(struct urlfile)     },
#if 0
  { "https"	, URL_HTTPS	, &httpsvec	, sizeof(struct urlhttps)    },
  { "ftp"       , URL_FTP       , &ftpvec       , sizeof(struct urlftp)      },
  { "gopher"    , URL_GOPHER    , &gophervec    , sizeof(struct urlgopher)   },
  { "mailto"    , URL_MAILTO    , &mailtovec    , sizeof(struct urlmailto)   },
  { "news"      , URL_NEWS      , &newsvec      , sizeof(struct urlnews)     },
  { "nntp"      , URL_NNTP      , &nntpvec      , sizeof(struct urlnntp)     },
  { "telnet"    , URL_TELNET    , &telnetvec    , sizeof(struct urltelnet)   },
  { "wais"      , URL_WAIS      , &waisvec      , sizeof(struct urlwais)     },
  { "prospero"  , URL_PROSPERO  , &prosperovec  , sizeof(struct urlprospero) },
#endif
};

#define PROTOCOLS	(sizeof(protos) / sizeof(struct urlrelation))

/********************************************************************/

size_t UrlGetProto(char *d,size_t sd,const char **ppurl)
{
  const char *s;
  char	     *rd;
  char	      c;

  ddt(d      != NULL);
  ddt(sd     >	0);
  ddt(ppurl  != NULL);
  ddt(*ppurl != NULL);

  rd = d;
  s  = *ppurl;
#ifdef DDT
  {
    size_t sz = strlen(s) - 2;
    if ((s[sz] < ' ') || (s[sz] > 0x7e))
      ddt(0);
  }
#endif
  D(ddtlog(ddtstream,"$","b: [%a] ",s);)
  while(((c = *s++) != 0) && (--sd))
  {
    if (c == ':') break;
    *d++ = tolower(c);
  }
  *d	 = '\0';
  *ppurl = s;
  D(ddtlog(ddtstream,"$ $","a: [%a] p: [%b]",s,rd);)
  return((size_t)(d - rd));
}

/***********************************************************************/

size_t UrlGetHost(char *d,size_t sd,const char **ppurl)
{
  const char *s;
  char       *rd;
  char	      c;

  ddt(d      != NULL);
  ddt(sd     >	0);
  ddt(ppurl  != NULL);
  ddt(*ppurl != NULL);

  rd = d;
  s  = *ppurl;

  if ((*s == '/') && (*(s+1) == '/'))
  {
    s += 2;	/* point past // in URL */
    while(((c = *s) != 0) && (--sd))
    {
      if ((c == ':') || (c == '/')) break;
      *d++ = tolower(c);
      s++;
    }
  }
  if (*(d-1) == '.') d--;       /* remove any trailing '.'      */
  *d	 = '\0';
  *ppurl = s;
  return((size_t)(d - rd));
}

/***********************************************************************/

size_t UrlGetPort(char *d,size_t sd,const char **ppurl)
{
  const char *s;
  char       *rd;
  char	      c;

  ddt(d      != NULL);
  ddt(sd     >	0);
  ddt(ppurl  != NULL);
  ddt(*ppurl != NULL);

  rd = d;
  s  = *ppurl;

  if (*s == ':')
  {
    s++;
    while(((c = *s) != 0) && (--sd))
    {
      if (!isdigit(*s)) break;
      *d++ = *s++;
    }
  }
  *d	 = '\0';
  *ppurl = s;
  return((size_t)(d - rd));
}

/***********************************************************************/

size_t UrlGetFile(char *d,size_t sd,const char **ppurl)
{
  const char *s;
  char       *rd;
  char	      c;

  ddt(d      != NULL);
  ddt(sd     >	0);
  ddt(ppurl  != NULL);
  ddt(*ppurl != NULL);

  rd = d;
  s  = *ppurl;

  while(((c = *s) != 0) && (--sd))
  {
    if (strchr(";?#",c)) break;
    *d++ = c;
    s++;
  }
  *d	 = '\0';
  *ppurl = s;
  return((size_t)(d - rd));
}

/********************************************************************/

int UrlNew(URL *ppurl,const char *url)
{
  size_t  i;
  const char *turl = url;
  char	  tmpbuf[BUFSIZ];
  URL	  purl;
  int	  rc;

  ddt(ppurl != NULL);
  ddt(url  != NULL);

  UrlGetProto(tmpbuf,BUFSIZ,&turl);

  for (i = 0 ; i < PROTOCOLS ; i++)
  {
    if (strcmp(tmpbuf,protos[i].proto) == 0)
    {
      purl	     = MemAlloc(protos[i].size);
      memset(purl,0,protos[i].size);
      purl->size     = protos[i].size;
      purl->vector   = protos[i].puv;
      purl->type     = protos[i].type;
      purl->protocol = dup_string(tmpbuf);
      rc	     = (*purl->vector->new)(purl,turl);
      if (rc == ERR_OKAY)
	*ppurl	     = purl;
      else
      {
	MemFree(purl->protocol);
	MemFree(purl);
	*ppurl	     = NULL;
      }
      return(rc);
    }
  }
  return(ERR_ERR);
}

/*******************************************************************/

int UrlType(URL url)
{
  ddt(url != NULL);

  return(url->type);
}

/*******************************************************************/

int UrlNormal(URL *nurl,URL url)
{
  int rc;

  ddt(nurl != NULL);
  ddt(url  != NULL);

  *nurl = MemAlloc(url->size);
  rc	= (*url->vector->normal)(*nurl,url);
  if (rc != ERR_OKAY)
  {
    MemFree(*nurl);
    *nurl = NULL;
  }
  return(rc);
}

/**********************************************************************/

int UrlCompare(URL durl,URL surl)
{
  ddt(durl != NULL);
  ddt(surl != NULL);

  return((*surl->vector->compare)(durl,surl));
}

/************************************************************************/

int UrlMakeString(URL url,char *d,size_t sd)
{
  ddt(url != NULL);
  ddt(d   != NULL);
  ddt(sd  >  0);

  return((*url->vector->makestring)(url,d,sd));
}

/************************************************************************/

int UrlDup(URL *purl,URL url)
{
  char tmpbuf[BUFSIZ];
  int  rc;

  rc = UrlMakeString(url,tmpbuf,BUFSIZ);
  if (rc != ERR_OKAY)
    return(rc);
  rc = UrlNew(purl,tmpbuf);
  if (rc != ERR_OKAY)
    return(rc);
  return(ERR_OKAY);
}

/************************************************************************/

int UrlFree(URL *purl)
{
  URL url;
  int rc;

  ddt(purl != NULL);
  ddt(*purl != NULL);

  url = *purl;
  rc = (*url->vector->free)(url);
  if (rc == ERR_OKAY)
  {
    MemFree(url);
    *purl = NULL;
  }
  return(rc);
}

/***********************************************************************/

char *UrlEncodeString(const char *src)
{
  size_t  nsize;
  char	 *dest;
  char	 *p;

  ddt(src != NULL);

  nsize = strlen(src) * 3 + 1;
  dest	= MemAlloc(nsize);

  for ( p = dest ; *src ; src++)
  {
    p = UrlEncodeChar(p,*src);
  }

  *p = 0;
  p  = dup_string(dest);
  MemFree(dest);
  return(p);
}

/*********************************************************************/

char *UrlEncodeChar(char *dest,char c)
{
  div_t sdiv;

  ddt(dest != NULL);

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
  /**dest   = 0;*/
  return(dest);
}

/*************************************************************************/

char *UrlDecodeString(char *src)
{
  char *r;
  char *t;

  ddt(src != NULL);

  /*for ( r = src ; *src ; src++)*/
  for ( r = src , t = src ; *src ; t++)
  {
    *t = UrlDecodeChar(&src);
  }
  *t = '\0';
  return(r);
}

/***********************************************************************/

char UrlDecodeChar(char **psrc)
{
  char *src;
  int	c;

  ddt(psrc  != NULL);
  ddt(*psrc != NULL);

  src = *psrc;
  c   = *src++;
  if (c == '+')
    c = ' ';
  else if (c == '%')
  {
    ddt(isxdigit(*src));
    ddt(isxdigit(*(src+1)));
    c	 = ctohex(*src) * 16 + ctohex(*(src+1));
    src += 2;
  }
  *psrc = src;
  return(c);
}

/**************************************************************************/

