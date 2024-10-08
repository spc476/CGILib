/***************************************************************************
*
* Copyright 2001,2013 by Sean Conner.
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

#ifndef I_712950D0_03A0_5611_BE73_AAD22FCF7C53
#define I_712950D0_03A0_5611_BE73_AAD22FCF7C53

#ifndef __GNUC__
#  define __attribute__(x)
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "nodelist.h"
#include "pair.h"

/*******************************************************************/

enum
{
  OTHER,
  HEAD,
  GET,
  POST,
  PUT,
  DELETE,
};

typedef enum http__e
{
        /*-----------------------------------
        ; Informational Codes
        ;----------------------------------*/
        
  HTTP_CONTINUE          = 100, /* 1.1  */
  HTTP_SWITCHPROTO       = 101, /* 1.1  */
  HTTP_PROCESSING        = 102, /* RFC-2518 */
  HTTP_CHECKPOINT        = 103, /* U */
  HTTP_EARLYHINTS        = 103, /* draft RFC */
  
        /*--------------------------------------
        ; Successful Codes
        ;--------------------------------------*/
        
  HTTP_OKAY              = 200, /* 1.0  */
  HTTP_CREATED           = 201, /* 1.0  */
  HTTP_ACCEPTED          = 202, /* 1.0  */
  HTTP_NONAUTH           = 203, /* 1.1  */
  HTTP_NOCONTENT         = 204, /* 1.0  */
  HTTP_RESETCONTENT      = 205, /* 1.1  */
  HTTP_PARTIALCONTENT    = 206, /* RFC-7233 */
  HTTP_MULTISTATUS       = 207, /* RFC-4918 */
  HTTP_ALREADYREPORTED   = 208, /* RFC-5842 */
  HTTP_IMUSED            = 226, /* RFC-3229 */
  
        /*----------------------------------------
        ; Redirection Codes
        ;----------------------------------------*/
        
  HTTP_300               = 300,
  HTTP_MULTICHOICE       = 300, /* 1.1  */
  HTTP_MOVEPERM          = 301, /* 1.0  */
  HTTP_MOVETEMP          = 302, /* 1.0  */
  HTTP_SEEOTHER          = 303, /* 1.1  */
  HTTP_NOTMODIFIED       = 304, /* 1.0  */
  HTTP_USEPROXY          = 305, /* 1.1  */
  HTTP_SWITCHPROXY       = 306, /* 1.1  */
  HTTP_MOVETEMP_M        = 307, /* 1.1  */
  HTTP_MOVEPERM_M        = 308, /* RFC-7538 */
  
        /*-----------------------------------------
        ; Client Error
        ;----------------------------------------*/
        
  HTTP_BADREQ            = 400, /* 1.0  */
  HTTP_UNAUTHORIZED      = 401, /* 1.0  */
  HTTP_PAYMENTREQ        = 402, /* 1.1  */
  HTTP_FORBIDDEN         = 403, /* 1.0  */
  HTTP_NOTFOUND          = 404, /* 1.0  */
  HTTP_METHODNOTALLOWED  = 405, /* 1.1  */
  HTTP_NOTACCEPTABLE     = 406, /* 1.1  */
  HTTP_PROXYAUTHREQ      = 407, /* 1.1  */
  HTTP_TIMEOUT           = 408, /* 1.1  */
  HTTP_CONFLICT          = 409, /* 1.1  */
  HTTP_GONE              = 410, /* 1.1  */
  HTTP_LENGTHREQ         = 411, /* 1.1  */
  HTTP_PRECONDITION      = 412, /* 1.1  */
  HTTP_TOOLARGE          = 413, /* 1.1  */
  HTTP_URITOOLONG        = 414, /* 1.1  */
  HTTP_MEDIATYPE         = 415, /* 1.1  */
  HTTP_RANGEERROR        = 416, /* 1.1  */
  HTTP_EXPECTATION       = 417, /* 1.1  */
  HTTP_IM_A_TEA_POT      = 418, /* RFC-2324 */
  HTTP_METHODFAILURE     = 420, /* U */
  HTTP_MISDIRECT         = 421, /* RFC-7540 */
  HTTP_UNPROCESSENTITY   = 422, /* RFC-4918 */
  HTTP_LOCKED            = 423, /* RFC-4918 */
  HTTP_FAILEDDEP         = 424, /* RFC-4918 */
  HTTP_UPGRADE           = 426,
  HTTP_PRECONDITION2     = 428, /* RFC-6585 */
  HTTP_TOOMANYREQUESTS   = 429, /* RFC-6585 */
  HTTP_REQHDR2BIG        = 431, /* RFC-6585 */
  HTTP_LOGINTIMEOUT      = 440, /* IIS */
  HTTP_NORESPONSE        = 444, /* nginx */
  HTTP_RETRYWITH         = 449, /* IIS */
  HTTP_MSBLOCK           = 450, /* U */
  HTTP_LEGALCENSOR       = 451, /* RFC-7725 */
  HTTP_SSLCERTERR        = 495, /* nginx */
  HTTP_SSLCERTREQ        = 496, /* nginx */
  HTTP_HTTP2HTTPS        = 497, /* nginx */
  HTTP_INVALIDTOKEN      = 498, /* U */
  HTTP_TOKENREQ          = 499, /* U */
  
        /*-----------------------------------------
        ; Server Errors
        ;-----------------------------------------*/
        
  HTTP_ISERVERERR        = 500, /* 1.0  */
  HTTP_NOTIMP            = 501, /* 1.0  */
  HTTP_BADGATEWAY        = 502, /* 1.0  */
  HTTP_NOSERVICE         = 503, /* 1.0  */
  HTTP_GATEWAYTIMEOUT    = 504, /* 1.1  */
  HTTP_HTTPVERSION       = 505, /* 1.1  */
  HTTP_VARIANTALSO       = 506, /* RFC-2295 */
  HTTP_ENOSPC            = 507, /* RFC-4918 */
  HTTP_LOOP              = 508, /* RFC-5842 */
  HTTP_EXCEEDBW          = 509, /* U */
  HTTP_NOTEXTENDED       = 510, /* RFC-2774 */
  HTTP_NETAUTHREQ        = 511, /* RFC-6585 */
  HTTP_UNKNOWN           = 520, /* CloudFlare */
  HTTP_BLACKHAWKDOWN     = 521, /* CloudFlare */
  HTTP_CONNECTIONTIMEOUT = 522, /* CloudFlare */
  HTTP_ORIGINUNREACHABLE = 523, /* CloudFlare */
  HTTP_TIMEOUT500        = 524, /* CloudFlare */
  HTTP_SSLHANDSHAKE      = 525, /* CloudFlare */
  HTTP_SSLINVALID        = 526, /* CloudFlare */
  HTTP_RAILGUN           = 527, /* CloudFlare */
  HTTP_FROZEN            = 530, /* U */
  
} http__e;

typedef struct cgi
{
  char    *query;          /* if NULL, no simple text query */
  char    *content_type;   /* valid for PUT, POST */
  size_t   content_length; /* valid for PUT, POST */
  List     qvars;          /* if not empty, name/value query */
  List     pvars;          /* valid on POST */
  int      method;
  http__e  status;
} *Cgi;

struct dstring
{
  char const *s1;
  char const *s2;
};

/************************************************************************/

extern Cgi     CgiNew          (void);
extern size_t  CgiGetValues    (Cgi,char ***,char const *);      /* added */
extern size_t  CgiRequired     (Cgi,struct dstring *,size_t);
extern int     CgiFree         (Cgi);

extern char   *UrlEncodeString (char const *);
extern char   *UrlEncodeChar   (char *,char);
extern char   *UrlDecodeString (char *);
extern char    UrlDecodeChar   (char **);

extern bool    HttpNotModified (time_t);
extern char   *HttpTimeStamp   (char *,size_t,time_t);

/********************************************************************/

static inline int CgiMethod(Cgi cgi)
{
  assert(cgi != NULL);
  return cgi->method;
}

/*------------------------------------------------------------------*/

static inline http__e CgiStatus(Cgi cgi)
{
  assert(cgi != NULL);
  return cgi->status;
}

/*------------------------------------------------------------------*/

static inline char const *CgiContentType(Cgi cgi)
{
  assert(cgi               != NULL);
  assert(cgi->content_type != NULL);
  return cgi->content_type;
}

/*-------------------------------------------------------------------*/

static inline size_t CgiContentLength(Cgi cgi)
{
  assert(cgi != NULL);
  return cgi->content_length;
}

/*-------------------------------------------------------------------*/

static inline struct pair *CgiGetPair(Cgi cgi,char const *name)
{
  assert(cgi  != NULL);
  assert(name != NULL);
  return PairListGetPair(&cgi->pvars,name);
}

/*-------------------------------------------------------------------*/

static inline char *CgiGetValue(Cgi cgi,char const *name)
{
  assert(cgi  != NULL);
  assert(name != NULL);
  return PairListGetValue(&cgi->pvars,name);
}

/*-------------------------------------------------------------------*/

static inline struct pair *CgiGetQPair(Cgi cgi,char const *name)
{
  assert(cgi  != NULL);
  assert(name != NULL);
  return PairListGetPair(&cgi->qvars,name);
}

/*-------------------------------------------------------------------*/

static inline char *CgiGetQValue(Cgi cgi,char const *name)
{
  assert(cgi  != NULL);
  assert(name != NULL);
  return PairListGetValue(&cgi->qvars,name);
}

/***********************************************************************/

#endif
