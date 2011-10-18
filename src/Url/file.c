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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../url.h"
#include "../errors.h"

/**********************************************************************/

static int	file_new	(url__t *,const char *);
static int	file_compare	(const url__t *const restrict,const url__t *const restrict);
static size_t	file_makestring	(const url__t *const restrict,char *restrict,size_t);
static void	file_free	(url__t *);

/***********************************************************************/

const struct urlvector g_filevec = 
{
  file_new,
  file_compare,
  file_makestring,
  file_free
};

/********************************************************************/

static int file_new(url__t *restrict url,const char *surl)
{
  char   tmpbuf[BUFSIZ];
  size_t tmpsz;
  
  assert(url  != NULL);
  assert(surl != NULL);

  /*------------------------------------------------------------
  ; host portion of string - can be either one of 'localhost' or
  ; the host this library is running on, or nothing.  If the host
  ; specified isn't this host, it's an error
  ;------------------------------------------------------------*/
    
  tmpsz = UrlGetHost(tmpbuf,BUFSIZ,&surl);
  if (tmpsz)
  {
    if (strcmp(tmpbuf,"localhost") != 0)
      return(ERR_ERR);
  }
  
  /*------------------------------------------------------------
  ; the file portion.  It has to exist, otherwise, it's an error
  ;-------------------------------------------------------------*/
  
  tmpsz = UrlGetFile(tmpbuf,BUFSIZ,&surl);
  if (tmpsz)
    url->file.path = strdup(tmpbuf);
  else
    return(ERR_ERR);
    
  return(ERR_OKAY);
}

/**************************************************************************/

static int file_compare(
	const url__t *const restrict durl,
	const url__t *const restrict surl
)
{
  int rc;

  assert(durl != NULL);
  assert(durl->scheme == URL_FILE);
  assert(surl         != NULL);
  assert(surl->scheme <  URL_max);

  rc = durl->scheme - surl->scheme;
  if (rc != 0) return rc;
  return strcmp(durl->file.path,surl->file.path);
}

/**********************************************************************/

static size_t file_makestring(
	const url__t *const restrict url,
	char         *restrict       d,
	size_t                       sd
)
{
  assert(url            != NULL);
  assert(url->scheme    == URL_FILE);
  assert(url->file.path != NULL);
  
  return snprintf(d,sd,"file://%s",url->file.path);
}

/**************************************************************************/

static void file_free(url__t *url)
{
  assert(url         != NULL);
  assert(url->scheme == URL_FILE);
  
  free(url->file.path);
  free(url);
}

/*************************************************************************/

