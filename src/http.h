
/***********************************************************************
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
************************************************************************/

#ifndef HTTP_H
#define HTTP_H

#include <limits.h>
#include "types.h"
#include "url.h"
#include "cgi.h"
#include "buffer.h"
#include "nodelist.h"
#include "errors.h"

#if 1
#  define RETRIES		5		/* as per RFC2068 */
#else
#  define RETRIES		15		/* as per Microsoft braindeath? */
#endif

#define HTTPv09		0x0090
#define HTTPv10		0x0100
#define HTTPv11		0x0110
#define HTTPvXX		INT_MAX

#define GET		0
#define POST		1
#define HEAD		2
#define PUT		3

#if 0
#  define DELETE	4
#  define TRACE		5
#endif

typedef struct http
{
  URLHTTP        url;
  List           sheaders;
  Buffer         buffer;
  Buffer         libuff;
  Buffer         lobuff;
  int            version;
  int            status;
  int            mstatus;
} *HTTP;

/***********************************************************************/

int		 (HttpConnect)		(HTTP *,int,URLHTTP,const char *[]);
int		 (HttpOpen)		(HTTP *,int,URLHTTP,const char *[]);
struct pair	*(HttpServerHeaders)	(const HTTP);
char		*(HttpGetServerHeader)	(const HTTP,const char *);
Buffer		 (HttpBuffer)		(const HTTP);
Buffer		 (HttpLineBuffer)	(const HTTP);
URLHTTP		 (HttpUrl)		(const HTTP);
int		 (HttpStatus)		(const HTTP);
int		 (HttpRedirected)	(const HTTP);
int		 (HttpClose)		(HTTP *);
int		 (HttpVersion)		(const char *);
int		 (HttpRetStatus)	(int,int);

#define HTTPCONNECT		(ERR_HTTP + 0)
#define HTTPOPEN		(ERR_HTTP + 1)
#define HTTPSERVERHEADERS	(ERR_HTTP + 2)
#define HTTPBUFFER		(ERR_HTTP + 3)
#define HTTPSTATUS		(ERR_HTTP + 4)
#define HTTPCLOSE		(ERR_HTTP + 5)
#define HTTPVERSION		(ERR_HTTP + 6)
#define HTTPRETSTATUS		(ERR_HTTP + 7)

#define HTTPERR_CONNECT 	(ERR_HTTP + 1)
#define HTTPERR_VERSION 	(ERR_HTTP + 2)
#define HTTPERR_NOSTATUS	(ERR_HTTP + 3)
#define HTTPERR_STATUS		(ERR_HTTP + 4)
#define HTTPERR_NOHEADER	(ERR_HTTP + 5)

/*************************************************************************/

	/*-----------------------------------
	; Informational Codes
	;----------------------------------*/

#define HTTP_CONTINUE		100		/* 1.1	*/
#define HTTP_SWITCHPROTO	101		/* 1.1	*/
#define HTTP_100MAX		101

	/*--------------------------------------
	; Successful Codes
	;--------------------------------------*/
	
#define HTTP_OKAY		200		/* 1.0	*/
#define HTTP_CREATED		201		/* 1.0	*/
#define HTTP_ACCEPTED		202		/* 1.0	*/
#define HTTP_NONAUTH		203		/* 1.1	*/
#define HTTP_NOCONTENT		204		/* 1.0	*/
#define HTTP_RESETCONTENT	205		/* 1.1	*/
#define HTTP_PARTIALCONTENT	206		/* 1.1	*/
#define HTTP_200MAX		206

	/*----------------------------------------
	; Redirection Codes
	;----------------------------------------*/

#define HTTP_300		300
#define HTTP_MULTICHOICE	300		/* 1.1	*/
#define HTTP_MOVEPERM		301		/* 1.0	*/
#define HTTP_MOVETEMP		302		/* 1.0	*/
#define HTTP_SEEOTHER		303		/* 1.1	*/
#define HTTP_NOTMODIFIED	304		/* 1.0	*/
#define HTTP_USEPROXY		305		/* 1.1	*/
#define HTTP_300MAX		305

	/*-----------------------------------------
	; Client Error
	;----------------------------------------*/
	
#define HTTP_BADREQ		400		/* 1.0	*/
#define HTTP_UNAUTHORIZED	401		/* 1.0	*/
#define HTTP_PAYMENTREQ		402		/* 1.1	*/
#define HTTP_FORBIDDEN		403		/* 1.0	*/
#define HTTP_NOTFOUND		404		/* 1.0	*/
#define HTTP_METHODNOTALLOWED	405		/* 1.1	*/
#define HTTP_NOTACCEPTABLE	406		/* 1.1	*/
#define HTTP_PROXYAUTHREQ	407		/* 1.1	*/
#define HTTP_TIMEOUT		408		/* 1.1	*/
#define HTTP_CONFLICT		409		/* 1.1	*/
#define HTTP_GONE		410		/* 1.1	*/
#define HTTP_LENGTHREQ		411		/* 1.1	*/
#define HTTP_PRECONDITION	412		/* 1.1	*/
#define HTTP_TOOLARGE		413		/* 1.1	*/
#define HTTP_URITOOLONG		414		/* 1.1	*/
#define HTTP_MEDIATYPE		415		/* 1.1	*/
#define HTTP_RANGEERROR		416		/* 1.1	*/
#define HTTP_EXPECTATION	417		/* 1.1	*/
#define HTTP_400MAX		417

	/*-----------------------------------------
	; Server Errors
	;-----------------------------------------*/
	
#define HTTP_ISERVERERR		500		/* 1.0	*/
#define HTTP_NOTIMP		501		/* 1.0	*/
#define HTTP_BADGATEWAY		502		/* 1.0	*/
#define HTTP_NOSERVICE		503		/* 1.0	*/
#define HTTP_GATEWAYTIMEOUT	504		/* 1.1	*/
#define HTTP_HTTPVERSION	505		/* 1.1	*/
#define HTTP_500MAX		505

#endif

