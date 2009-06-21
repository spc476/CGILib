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
void		 (CgiGetRawData)	(const Cgi,char **,size_t *);
void		 (CgiListMake)		(const Cgi);
void		 (CgiOutText)		(const Cgi);
void		 (CgiOutHtml)		(const Cgi);
void		 (CgiOutShtml)		(const Cgi);
void		 (CgiOutLocation)	(const Cgi,const char *);
int		 (CgiMethod)		(const Cgi);
struct pair	*(CgiNextValue)		(const Cgi);
struct pair	*(CgiListFirst)		(const Cgi);
struct pair	*(CgiListGetPair)	(const Cgi,const char *);
char		*(CgiListGetValue)	(const Cgi,const char *);
size_t	 	 (CgiListGetValues)	(const Cgi,char ***,const char *);	/* added */
int		 (CgiListRequired)	(const Cgi,struct dstring *,size_t);
int		 (CgiFree)		(Cgi);

char		*(UrlEncodeString)	(const char *);
char		*(UrlEncodeChar)	(char *,char);
char		*(UrlDecodeString)	(char *);
char		 (UrlDecodeChar)	(char **);

#endif

/*******************************************************************/

