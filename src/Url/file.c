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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

#include "../conf.h"
#include "../memory.h"
#include "../errors.h"
#include "../buffer.h"
#include "../util.h"
#include "../url.h"
#include "../ddt.h"

/**********************************************************************/

static int		 file_new	(URL,const char *);
static int		 file_normal	(URL,URL);
static int		 file_compare	(URL,URL);
static int		 file_makestring(URL,char *,size_t);
static int		 file_free	(URL);
static int		 file_nourl	(URLFILE,int);
#ifdef DDT
  static int		 FILE_check	(struct urlfile *);
#endif

/***********************************************************************/

const struct urlvector filevec = 
{
  file_new,
  file_normal,
  file_compare,
  file_makestring,
  file_free
};

/********************************************************************/

static int file_new(URL url,const char *surl)
{
  URLFILE furl = (URLFILE)url;
  char    tmpbuf[BUFSIZ];
  size_t  tmpsz;
  
  ddt(url  != NULL);
  ddt(surl != NULL);

  /*------------------------------------------------------------
  ; host portion of string - can be either one of 'localhost' or
  ; the host this library is running on, or nothing.  If the host
  ; specified isn't this host, it's an error
  ;------------------------------------------------------------*/
    
  tmpsz = UrlGetHost(tmpbuf,BUFSIZ,&surl);
  if (tmpsz)
  {
    /*if ((strcmp(tmpbuf,"localhost") != 0) && (strcmp(tmpbuf,CGIHOST) != 0))*/
    if (strcmp(tmpbuf,"localhost") != 0)
      return(file_nourl(furl,ErrorPush(CgiErr,URLNEW,URLERR_HOST,"$",tmpbuf)));
  }
  
  /*------------------------------------------------------------
  ; the file portion.  It has to exist, otherwise, it's an error
  ;-------------------------------------------------------------*/
  
  tmpsz = UrlGetFile(tmpbuf,BUFSIZ,&surl);
  if (tmpsz)
    furl->file = dup_string(tmpbuf);
  else
    return(file_nourl(furl,ErrorPush(CgiErr,URLNEW,URLERR_FILE,"$","/")));
    
  return(ERR_OKAY);
}

/**************************************************************************/
    
static int file_normal(URL durl,URL surl)
{
  return(ErrorPush(CgiErr,URLNORMAL,URLERR_NOTIMP,""));
}

/**********************************************************************/

static int file_compare(URL durlb,URL surlb)
{
  URLFILE durl = (URLFILE)durlb;
  URLFILE surl = (URLFILE)surlb;
  int     rc;

  ddt(FILE_check(durl));
  ddt(FILE_check(surl));
  
  rc = strcmp(durl->protocol,surl->protocol);
  if (rc != 0) return(rc);
  return(strcmp(durl->file,surl->file));
}

/**********************************************************************/

static int file_makestring(URL urlb,char *d,size_t sd)
{
  URLFILE         url = (URLFILE)urlb;

  ddt(d      != NULL);
  ddt(sd     >  0);
  ddt(FILE_check(url));

  formatstr(d,sd,"$","%a://localhost%a",url->protocol,url->file);
  return(ERR_OKAY);
}

/**************************************************************************/

static int file_free(URL url)
{
  URLFILE purl = (URLFILE)url;

  ddt(FILE_check(purl));

  purl->vector = NULL;  
  MemFree(purl->protocol,strlen(purl->protocol) + 1);
  MemFree(purl->file,    strlen(purl->file)     + 1);
  return(ERR_OKAY);
}

/*************************************************************************/

#ifdef DDT
  static int FILE_check(struct urlfile *purl)
  {
    if (purl == NULL) 				return(0);
    if (purl->protocol == NULL) 		return(0);
    if (strcmp(purl->protocol,"file") != 0)	return(0);
    if (purl->file     == NULL)			return(0);
    return(1);
  }
#endif

/************************************************************************/

static int file_nourl(URLFILE url,int error)
{
  URLFILE purl = (URLFILE)url;

  ddt(FILE_check(purl));

  purl->vector = NULL;  
  if (purl->protocol)	MemFree(purl->protocol,strlen(purl->protocol)+1);
  if (purl->file)	MemFree(purl->file,    strlen(purl->file)+1);
  return(error);
}

/**************************************************************************/

