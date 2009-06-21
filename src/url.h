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

#define PORTMAX	65535u

/***********************************************************************/

typedef enum urltype
{
  URL_UNKNOWN,
  URL_HTTP,
  URL_FILE,
  URL_FTP,
  URL_GOPHER,
  URL_MAILTO,
  URL_NEWS,
  URL_NNTP,
  URL_TELNET,
  URL_WAIS,
  URL_PROSPERO
} UrlType;
  
struct urlbase
{
  UrlType                 type;
  size_t	          size;
  const struct urlvector *vector;
  char	                 *protocol;
};

struct urlhttp
{
  UrlType           type;
  size_t	    size;
  const struct urlvector *vector;
  char	           *protocol;
  char	           *host;
  int		    port;
  char	           *file;
  char             *params;
  char             *query;
  char	           *fragment;
};

struct urlfile
{
  UrlType                 type;
  size_t	          size;
  const struct urlvector *vector;
  char	                 *protocol;
  char	                 *file;
};

struct urlftp
{
  UrlType           type;
  size_t	    size;
  const struct urlvector *vector;
  char	           *protocol;
  char	           *host;
  int		    port;
  char	           *user;
  char	           *password;
  char	           *file;
  int		    xmittype;
};

struct urlgopher
{
  UrlType           type;
  size_t	    size;
  const struct urlvector *vector;
  char	           *protocol;
  char	           *host;
  int		    port;
  char	           *select;
  char	           *search;
  char	           *gpsearch;
};

struct urlmailto
{
  UrlType           type;
  size_t	    size;
  const struct urlvector *vector;
  char	           *protocol;
  char	           *email;
};

struct urlnews
{
  UrlType           type;
  size_t	    size;
  const struct urlvector *vector;
  char	           *protocol;
  char	           *newsgroup;
};

struct urlnntp
{
  UrlType           type;
  size_t	    size;
  const struct urlvector *vector;
  char	           *protocol;
  char	           *host;
  int		    port;
  char	           *newsgroup;
  unsigned long     article;
};

struct urltelnet
{
  UrlType           type;
  size_t	    size;
  const struct urlvector *vector;
  char	           *protocol;
  char	           *host;
  int		    port;
  char	           *user;
  char	           *password;
};

struct urlwais
{
  UrlType           type;
  size_t	    size;
  const struct urlvector *vector;
  char	           *protocol;
  char	           *host;
  int		    port;
  char	           *database;
  char	           *search;
  char	           *waistype;
  char	           *waispath;
};

struct urlprospero
{
  UrlType           type;
  size_t            size;
  const struct urlvector *vector;
  char             *protocol;
  char	           *host;
  int		    port;
  char             *hsoname;
  char             *field;
  char	           *value;
};

typedef union url
{
  UrlType            type;
  struct urlbase     url;
  struct urlhttp     http;
  struct urlfile     file;
  struct urlftp      ftp;
  struct urlgopher   gopher;
  struct urlmailto   mailto;
  struct urlnews     news;
  struct urlnntp     nntp;
  struct urltelnet   telnet;
  struct urlwais     wais;
  struct urlprospero prospero;
} *URL;
  
struct urlvector
{
  int   (*new)		(URL,const char *);
  int   (*normal) 	(URL *,URL);
  int   (*compare)	(URL,URL);
  char *(*makestring)	(URL);
  int   (*free)		(URL);
  int   (*getinfo)	(URL,int,void *,size_t);
  int   (*setinfo)	(URL,int,void *,size_t);
  int   (*connect)	(URL,FILE *,int);
};

/********************************************************************/

size_t	 UrlGetProto		(char *,size_t,const char **);
size_t	 UrlGetHost		(char *,size_t,const char **);
size_t	 UrlGetPort		(char *,size_t,const char **);
size_t	 UrlGetFile		(char *,size_t,const char **);

URL		 UrlNew 		(const char *);
UrlType		 UrlGetType		(URL);
URL		 UrlNormal		(URL);
int		 UrlCompare		(URL,URL);
char		*UrlMakeString		(URL);
URL		 UrlDup 		(URL);
int		 UrlGetInfo		(URL,int,void *,size_t);
int		 UrlSetInfo		(URL,int,void *,size_t);
int		 UrlOpen		(URL,FILE *,int);
int		 UrlFree		(URL);

char		*(UrlHttpRequest)	(struct urlhttp *);
char		*(UrlHttpHost)		(struct urlhttp *);
char		*(UrlHttpHostPort)	(struct urlhttp *);
  
char		*UrlEncodeChar		(char *,char);
char		 UrlDecodeChar		(char **);
char		*UrlEncodeString	(const char *);
char		*UrlDecodeString	(char *);

#endif

/**********************************************************************/

