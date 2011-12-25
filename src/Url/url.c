/************************************************************************
*
* Copyright 2001,2011 by Sean Conner.  All Rights Reserved.
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

#include "../url.h"
#include "../errors.h"

/**********************************************************************/

extern struct urlvector g_httpvec;
extern struct urlvector g_filevec;

const struct urlrelation g_protos[] = 
{
  { "file"      , URL_FILE      , &g_filevec      , sizeof(urlfile__t) },
  { "http"      , URL_HTTP      , &g_httpvec      , sizeof(urlhttp__t) },
};

#define PROTOCOLS	(sizeof(g_protos) / sizeof(struct urlrelation))

/********************************************************************/

size_t UrlGetProto(char *d,size_t sd,const char **ppurl)
{
  const char *s;
  char	     *rd;
  char	      c;

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
  *d	 = '\0';
  *ppurl = s;
  return((size_t)(d - rd));
}

/***********************************************************************/

size_t UrlGetHost(char *d,size_t sd,const char **ppurl)
{
  const char *s;
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
    if (strchr("?#",c)) break;
    *d++ = c;
    s++;
  }
  *d	 = '\0';
  *ppurl = s;
  return((size_t)(d - rd));
}

/********************************************************************/

url__t *UrlNew(const char *url)
{
  size_t      i;
  const char *turl = url;
  char	      tmpbuf[BUFSIZ];
  url__t     *purl;

  assert(url != NULL);

  UrlGetProto(tmpbuf,BUFSIZ,&turl);

  for (i = 0 ; i < PROTOCOLS ; i++)
  {
    if (strcmp(tmpbuf,g_protos[i].proto) == 0)
    {
      purl         = calloc(1,g_protos[i].size);
      purl->scheme = g_protos[i].scheme;
      
      if ((*g_protos[i].puv->new)(purl,turl) == ERR_OKAY)
        return purl;
      
      free(purl);
      return NULL;
    }
  }
  
  return NULL;
}
      
/**********************************************************************/

