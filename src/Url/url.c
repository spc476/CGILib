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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

#include "../url.h"

/**********************************************************************/

extern struct urlvector g_httpvec;
extern struct urlvector g_filevec;
extern struct urlvector g_gophervec;

struct urlrelation const g_protos[] =
{
  { "file"      , URL_FILE      , &g_filevec      , sizeof(urlfile__t)   },
  { "http"      , URL_HTTP      , &g_httpvec      , sizeof(urlhttp__t)   },
  { "gopher"    , URL_GOPHER    , &g_gophervec    , sizeof(urlgopher__t) },
  { "https"     , URL_HTTPS     , &g_httpvec      , sizeof(urlhttp__t)   },
};

#define PROTOCOLS       (sizeof(g_protos) / sizeof(struct urlrelation))

/********************************************************************/

size_t UrlGetProto(char *d,size_t sd,char const **ppurl)
{
  char const *s;
  char       *rd;
  char        c;
  
  assert(d      != NULL);
  assert(sd     >     0);
  assert(ppurl  != NULL);
  assert(*ppurl != NULL);
  
  rd = d;
  s  = *ppurl;
  
  while(((c = *s++) != 0) && (--sd))
  {
    if (c == ':') break;
    *d++ = tolower(c);
  }
  *d     = '\0';
  *ppurl = s;
  return((size_t)(d - rd));
}

/***********************************************************************/

size_t UrlGetHost(char *d,size_t sd,char const **ppurl)
{
  char const *s;
  char       *rd;
  
  assert(d      != NULL);
  assert(sd     >  0);
  assert(ppurl  != NULL);
  assert(*ppurl != NULL);
  
  rd = d;
  s  = *ppurl;
  
  if ((*s == '/') && (*(s+1) == '/'))
  {
    int c;
    
    s += 2;     /* point past // in URL */
    while(((c = *s) != 0) && (--sd))
    {
      if ((c == ':') || (c == '/')) break;
      *d++ = tolower(c);
      s++;
    }
  }
  if (*(d-1) == '.') d--;       /* remove any trailing '.'      */
  *d     = '\0';
  *ppurl = s;
  return((size_t)(d - rd));
}

/***********************************************************************/

size_t UrlGetPort(char *d,size_t sd,char const **ppurl)
{
  char const *s;
  char       *rd;
  
  assert(d      != NULL);
  assert(sd     >  0);
  assert(ppurl  != NULL);
  assert(*ppurl != NULL);
  
  rd = d;
  s  = *ppurl;
  
  if (*s == ':')
  {
    s++;
    while (*s && --sd)
    {
      if (!isdigit(*s)) break;
      *d++ = *s++;
    }
  }
  *d     = '\0';
  *ppurl = s;
  return((size_t)(d - rd));
}

/***********************************************************************/

size_t UrlGetFile(char *d,size_t sd,char const **ppurl)
{
  char const *s;
  char       *rd;
  char        c;
  
  assert(d      != NULL);
  assert(sd     >       0);
  assert(ppurl  != NULL);
  assert(*ppurl != NULL);
  
  rd = d;
  s  = *ppurl;
  
  while(((c = *s) != 0) && (--sd))
  {
    if (strchr("?#",c)) break;
    *d++ = c;
    s++;
  }
  *d     = '\0';
  *ppurl = s;
  return((size_t)(d - rd));
}

/********************************************************************/

url__t *UrlNew(char const *url)
{
  size_t      i;
  char const *turl = url;
  char        tmpbuf[BUFSIZ];
  url__t     *purl;
  
  assert(url != NULL);
  
  UrlGetProto(tmpbuf,BUFSIZ,&turl);
  
  for (i = 0 ; i < PROTOCOLS ; i++)
  {
    if (strcmp(tmpbuf,g_protos[i].proto) == 0)
    {
      purl         = calloc(1,g_protos[i].size);
      purl->scheme = g_protos[i].scheme;
      
      if ((*g_protos[i].puv->new)(purl,turl) == 0)
        return purl;
        
      free(purl);
      return NULL;
    }
  }
  
  return NULL;
}

/**********************************************************************/

