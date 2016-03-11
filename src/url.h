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

#ifndef I_B51450F6_CCD6_5434_B2EA_1999BA310107
#define I_B51450F6_CCD6_5434_B2EA_1999BA310107

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
  URL_GOPHER,
  URL_max
} url_type__t;

typedef enum
{
  GOPHER_FILE     = '0',
  GOPHER_DIR      = '1',
  GOPHER_CSO      = '2',
  GOPHER_ERROR    = '3',
  GOPHER_BINHEX   = '4',
  GOPHER_ZIP      = '5',
  GOPHER_UUENCODE = '6',
  GOPHER_SEARCH   = '7',
  GOPHER_TELNET   = '8',
  GOPHER_BINARY   = '9',
  GOPHER_IMAGE    = 'I',
  GOPHER_3270     = 'T',
  GOPHER_GIF      = 'g',
  GOPHER_HTML     = 'h',
  GOPHER_INFO     = 'i',
} gopher_type__e;

typedef struct urlhttp
{
  url_type__t  scheme;
  char        *host;
  int          port;
  char        *path;
  char        *query;
  char        *fragment;
} urlhttp__t;

typedef struct urlgopher
{
  url_type__t     scheme;
  char           *host;
  int             port;
  gopher_type__e  type;
  char           *selector;
  char           *search;
  char           *plus;
} urlgopher__t;

typedef struct urlfile
{
  url_type__t  scheme;
  char        *path;
} urlfile__t;

typedef union url
{
  url_type__t  scheme;
  urlhttp__t   http;
  urlfile__t   file;
  urlgopher__t gopher;
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

extern size_t	 UrlGetProto		(char *,size_t,const char **);
extern size_t	 UrlGetHost		(char *,size_t,const char **);
extern size_t	 UrlGetPort		(char *,size_t,const char **);
extern size_t	 UrlGetFile		(char *,size_t,const char **);

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

