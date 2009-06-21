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
#include <ctype.h>
#include <assert.h>

#include "../url.h"
#include "../errors.h"
#include "../util.h"

/**********************************************************************/

extern const struct urlvector httpvec;
extern const struct urlvector filevec;

static const struct urlrelation
{
  char		         *const proto;
  UrlType	                type;
  const struct urlvector *const puv;
  size_t	                size;
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

  assert(d      != NULL);
  assert(sd     >	0);
  assert(ppurl  != NULL);
  assert(*ppurl != NULL);

  rd = d;
  s  = *ppurl;
#ifdef DDT
  {
    size_t sz = strlen(s) - 2;
    if ((s[sz] < ' ') || (s[sz] > 0x7e))
      assert(0);
  }
#endif
  while(((c = *s++) != 0) && (--sd))
  {
    if (c == ':') break;
    *d++ = tolower(c);
  }
  *d	 = '\0';
  *ppurl = s;
  return((size_t)(d - rd));
}

/***********************************************************************/

size_t UrlGetHost(char *d,size_t sd,const char **ppurl)
{
  const char *s;
  char       *rd;
  char	      c;

  assert(d      != NULL);
  assert(sd     >	0);
  assert(ppurl  != NULL);
  assert(*ppurl != NULL);

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

  assert(d      != NULL);
  assert(sd     >	0);
  assert(ppurl  != NULL);
  assert(*ppurl != NULL);

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

  assert(d      != NULL);
  assert(sd     >	0);
  assert(ppurl  != NULL);
  assert(*ppurl != NULL);

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

URL UrlNew(const char *url)
{
  size_t      i;
  const char *turl = url;
  char	      tmpbuf[BUFSIZ];
  URL	      purl;
  int	      rc;

  assert(url != NULL);

  UrlGetProto(tmpbuf,BUFSIZ,&turl);

  for (i = 0 ; i < PROTOCOLS ; i++)
  {
    if (strcmp(tmpbuf,protos[i].proto) == 0)
    {
      purl = malloc(protos[i].size);
      memset(purl,0,protos[i].size);
      purl->url.type     = protos[i].type;
      purl->url.size     = protos[i].size;
      purl->url.vector   = protos[i].puv;
      purl->url.protocol = strdup(tmpbuf);
      
      rc = (*purl->url.vector->new)(purl,turl);
      if (rc == ERR_OKAY)
        return purl;
      else
      {
	free(purl->url.protocol);
	free(purl);
	return NULL;
      }
    }
  }
  return NULL;
}

/*******************************************************************/

UrlType UrlGetType(URL url)
{
  assert(url != NULL);

  return(url->type);
}

/*******************************************************************/

URL UrlNormal(URL url)
{
  URL new;

  assert(url  != NULL);

  new = malloc(url->url.size);
  if ((*url->url.vector->normal)(&new,url) != ERR_OKAY)
  {
    free(new);
    new = NULL;
  }
  return new;
}

/**********************************************************************/

int UrlCompare(URL durl,URL surl)
{
  assert(durl != NULL);
  assert(surl != NULL);

  return((*surl->url.vector->compare)(durl,surl));
}

/************************************************************************/

char *UrlMakeString(URL url)
{
  assert(url != NULL);

  return (*url->url.vector->makestring)(url);
}

/************************************************************************/

URL UrlDup(URL url)
{
  URL   dup;
  char *tmp;

  tmp = UrlMakeString(url);
  dup = UrlNew(tmp);
  free(tmp);
  return dup;
}

/************************************************************************/

int UrlFree(URL url)
{
  int rc;

  assert(url != NULL);

  rc = (*url->url.vector->free)(url);
  if (rc == ERR_OKAY)
    free(url);

  return(rc);
}

/***********************************************************************/

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

