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

#ifndef CGI_H
#define CGI_H

#include <stddef.h>
#include <stdio.h>
#include "nodelist.h"
#include "pair.h"

/*******************************************************************/

enum
{
  GET,
  POST,
  HEAD,
  PUT
};

typedef struct cgi
{
  char   *buffer;
  size_t  bufsize;
  char   *pbufend;
  char   *pbuff;
  char   *datatype;
  List    vars;
  void   *data;
  int     method;
} *Cgi;

struct dstring
{
  const char *s1;
  const char *s2;
};

/************************************************************************/

Cgi		 (CgiNew)		(void *);
void		 (CgiListMake)		(const Cgi);
struct pair	*(CgiNextValue)		(const Cgi);
size_t	 	 (CgiListGetValues)	(const Cgi,char ***,const char *);	/* added */
int		 (CgiListRequired)	(const Cgi,struct dstring *,size_t);
int		 (CgiFree)		(Cgi);

char		*(UrlEncodeString)	(const char *);
char		*(UrlEncodeChar)	(char *,char);
char		*(UrlDecodeString)	(char *);
char		 (UrlDecodeChar)	(char **);

/********************************************************************/

static inline void (CgiGetRawData)(const Cgi cgi,char **pdest,size_t *psize)
{
  assert(cgi   != NULL);
  assert(pdest != NULL);
  assert(psize != NULL);
  
  *pdest = cgi->buffer;
  *psize = cgi->bufsize;
}

/*--------------------------------------------------------------------*/

static inline void (CgiOutHtml)(const Cgi cgi)
{
  static const char msg[] = "Content-type: text/html\n\n";
  
  assert(cgi != NULL);
  fputs(msg,stdout);
}

/*----------------------------------------------------------------------*/

static inline void (CgiOutText)(const Cgi cgi)
{
  static const char msg[] = "Content-type: text/plain\n\n";

  assert(cgi != NULL);  
  fputs(msg,stdout);
}

/*--------------------------------------------------------------------*/

static inline void (CgiOutShtml)(const Cgi cgi)
{
  static const char msg[] = "Content-type: text/x-server-parsed-html\n\n";
  
  assert(cgi != NULL);
  fputs(msg,stdout);
}

/*--------------------------------------------------------------------*/

static inline void (CgiOutLocation)(const Cgi cgi,const char *location)
{
  assert(cgi      != NULL);
  assert(location != NULL);
  
  fprintf(stdout,"Location: %s\n\n",location);
}

/*--------------------------------------------------------------------*/

static inline int (CgiMethod)(const Cgi cgi)
{
  assert(cgi != NULL);
  return(cgi->method);
}

/*------------------------------------------------------------------*/

static inline struct pair *(CgiListFirst)(const Cgi cgi)
{
  assert(cgi != NULL);
  return(PairListFirst(&cgi->vars));
}

/*-------------------------------------------------------------------*/

static inline struct pair *(CgiListGetPair)(const Cgi cgi,const char *name)
{
  assert(cgi  != NULL);
  assert(name != NULL);
  return(PairListGetPair(&cgi->vars,name));
}

/*-------------------------------------------------------------------*/

static inline char *(CgiListGetValue)(const Cgi cgi,const char *name)
{
  assert(cgi  != NULL);
  assert(name != NULL);
  return(PairListGetValue(&cgi->vars,name));
}

/***********************************************************************/

#endif


