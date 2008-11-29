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

#ifndef URL_H
#define URL_H

#include <limits.h>

#include "types.h"
#include "conf.h"
#include "errors.h"
#include "stream.h"

/************************************************************************/

#if (INT_MAX < 65535L)
#  define PORTMAX	INT_MAX
#else
#  define PORTMAX	65535
#endif

#define URL_HTTP		0
#define URL_FILE		1
#define URL_FTP 		2
#define URL_GOPHER		3
#define URL_MAILTO		4
#define URL_NEWS		5
#define URL_NNTP		6
#define URL_TELNET		7
#define URL_WAIS		8
#define URL_PROSPERO		9

#define GS_HOST 		0
#define GS_PORT 		1
#define GS_USER 		2
#define GS_PASS 		3

#define CON_READ		0
#define CON_WRITE		1
#define CON_RW			2

/***********************************************************************/

typedef struct urlbase
{
  size_t	      size;
  struct urlvector *vector;
  char	     *protocol;
  int		      type;
} *URL;

typedef struct urlhttp
{
  size_t	      size;
  struct urlvector *vector;
  char	     *protocol;
  int		      type;
  char	     *host;
  int		      port;
  char	     *file;
  char             *params;
  char             *query;
  char	     *fragment;
} *URLHTTP;

typedef struct urlfile
{
  size_t	      size;
  struct urlvector *vector;
  char	     *protocol;
  int		      type;
  char	     *file;
} *URLFILE;

typedef struct urlftp
{
  size_t	      size;
  struct urlvector *vector;
  char	     *protocol;
  int		      type;
  char	     *host;
  int		      port;
  char	     *user;
  char	     *password;
  char	     *file;
  int		      xmittype;
} *URLFTP;

typedef struct urlgopher
{
  size_t	      size;
  struct urlvector *vector;
  char	     *protocol;
  int		      type;
  char	     *host;
  int		      port;
  char	     *select;
  char	     *search;
  char	     *gpsearch;
} *URLGOPHER;

typedef struct urlmailto
{
  size_t	      size;
  struct urlvector *vector;
  char	     *protocol;
  int		      type;
  char	     *email;
} *URLMAILTO;

typedef struct urlnews
{
  size_t	      size;
  struct urlvector *vector;
  char	     *protocol;
  int		      type;
  char	     *newsgroup;
} *URLNEWS;

typedef struct urlnntp
{
  size_t	      size;
  struct urlvector *vector;
  char	     *protocol;
  int		      type;
  char	     *host;
  int		      port;
  char	     *newsgroup;
  unsigned long     article;
} *URLNNTP;

typedef struct urltelnet
{
  size_t	      size;
  struct urlvector *vector;
  char	     *protocol;
  int		      type;
  char	     *host;
  int		      port;
  char	     *user;
  char	     *password;
} *URLTELNET;

typedef struct urlwais
{
  size_t	      size;
  struct urlvector *vector;
  char	     *protocol;
  int		      type;
  char	     *host;
  int		      port;
  char	     *database;
  char	     *search;
  char	     *waistype;
  char	     *waispath;
} *URLWAIS;

typedef struct urlprospero
{
  size_t            size;
  struct urlvector *vector;
  char             *protocol;
  int		      type;
  char	     *host;
  int		      port;
  char             *hsoname;
  char             *field;
  char	     *value;
} *URLPROSPERO;

struct urlvector
{
  int (*new)		(URL,const char *);
  int (*normal) 	(URL,URL);
  int (*compare)	(URL,URL);
  int (*makestring)	(URL,char *,size_t);
  int (*free)		(URL);
  int (*getinfo)	(URL,int,void *,size_t);
  int (*setinfo)	(URL,int,void *,size_t);
  int (*connect)	(URL,Stream *,int);
};

/********************************************************************/

size_t	 UrlGetProto		(char *,size_t,const char **);
size_t	 UrlGetHost		(char *,size_t,const char **);
size_t	 UrlGetPort		(char *,size_t,const char **);
size_t	 UrlGetFile		(char *,size_t,const char **);

int		 UrlNew 		(URL *,const char *);
int		 UrlType		(URL);
int		 UrlNormal		(URL *,URL);
int		 UrlCompare		(URL,URL);
int		 UrlMakeString		(URL,char *,size_t);
int		 UrlDup 		(URL *,URL);
int		 UrlGetInfo		(URL,int,void *,size_t);
int		 UrlSetInfo		(URL,int,void *,size_t);
int		 UrlOpen		(URL,Stream *,int);
int		 UrlFree		(URL *);

char		*(UrlHttpRequest)	(URLHTTP);
char		*(UrlHttpHost)		(URLHTTP);
char		*(UrlHttpHostPort)	(URLHTTP);
  
char		*UrlEncodeChar		(char *,char);
char		 UrlDecodeChar		(char **);
char		*UrlEncodeString	(const char *);
char		*UrlDecodeString	(char *);

#endif

/**********************************************************************/

