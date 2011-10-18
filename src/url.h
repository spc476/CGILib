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

#ifndef URL_H
#define URL_H

#include <limits.h>
#include <assert.h>

/************************************************************************/

#if (INT_MAX < 65535L)
#  define PORTMAX	INT_MAX
#else
#  define PORTMAX	65535
#endif

/********************************************************************/

typedef enum url_type
{
  URL_FILE,
  URL_HTTP,
  URL_max
} url_type__t;

typedef struct urlhttp
{
  url_type__t  scheme;
  char        *host;
  int          port;
  char        *path;
  char        *query;
  char        *fragment;
} urlhttp__t;

typedef struct urlfile
{
  url_type__t  scheme;
  char        *path;
} urlfile__t;

typedef union url
{
  url_type__t scheme;
  urlhttp__t  http;
  urlfile__t  file;
} url__t;

struct urlvector
{
  int    (*new)		(url__t *restrict,const char *);
  int    (*compare)	(const url__t *const restrict,const url__t *const restrict);
  size_t (*makestring)	(const url__t *const restrict,char *restrict,size_t);
  void   (*free)	(url__t *);
};

struct urlrelation
{
  const char *const             proto;
  const url_type__t             scheme;
  const struct urlvector *const puv;
  const size_t                  size;
};

/********************************************************************/

extern const struct urlrelation g_protos[];

size_t	 UrlGetProto		(char *,size_t,const char **);
size_t	 UrlGetHost		(char *,size_t,const char **);
size_t	 UrlGetPort		(char *,size_t,const char **);
size_t	 UrlGetFile		(char *,size_t,const char **);

url__t	*UrlNew		(const char *);

/*------------------------------------------------------------------*/

static inline int UrlCompare(
	const url__t *const restrict durl,
	const url__t *const restrict surl
)
{
  assert(durl         != NULL);
  assert(durl->scheme <  URL_max);
  assert(surl         != NULL);
  assert(surl->scheme <  URL_max);
  
  return (*g_protos[durl->scheme].puv->compare)(durl,surl);
}

/*------------------------------------------------------------------*/

static inline size_t UrlMakeString(
	const url__t *const restrict url,
	char         *restrict       d,
	size_t                       sd
)
{
  assert(url         != NULL);
  assert(url->scheme <  URL_max);
  assert(d           != NULL);
  assert(sd          >  0);

  return (*g_protos[url->scheme].puv->makestring)(url,d,sd);
}

/*------------------------------------------------------------------*/

static inline void UrlFree(url__t *url)
{
  assert(url         != NULL);
  assert(url->scheme <  URL_max);
  
  (*g_protos[url->scheme].puv->free)(url);
}

/********************************************************************/

#endif

