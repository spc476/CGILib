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

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "util.h"

/***************************************************************************/

char *up_string(char *s)
{
  char *r = s;
  
  assert(s != NULL);
  
  for ( ; *s ; s++)
    *s = toupper(*s);
  return(r);
}

/***************************************************************************/

char *down_string(char *s)
{
  char *r = s;
  
  assert(s != NULL);
  
  for ( ; *s ; s++)
    *s = tolower(*s);
  return(r);
}

/*************************************************************************/

bool empty_string(const char *s)
{
  assert(s != NULL);
  
  for ( ; *s ; s++)
    if (!isspace(*s)) return false;

  return true;
}

/*************************************************************************/

char *remove_char(char *s,int (*tstchar)(int))
{
  char *ret = s;
  char *d   = s;
  
  assert(s != NULL);
  assert(tstchar);		/* can function pointers be compared to NULL? */
  
  for ( ; *s ; s++)
    if (!(*tstchar)(*s)) *d++ = *s;

  *d = '\0';
  return(ret);
}

/************************************************************************/

char *trim_lspace(char *s)
{
  char *p;

  assert(s != NULL);  

  for ( p = s ; (*p) && (isspace(*p)) ; p++)
    ;
  memmove(s,p,strlen(p) + 1);
  return s;
}

/*************************************************************************/

char *trim_tspace(char *s)
{
  char *p;
  
  assert(s != NULL);
  
  for (p = s + strlen(s) - 1 ; (p > s) && (isspace(*p)) ; p--)
    ;
  *(p+1) = '\0';
  return s;
}

/**************************************************************************/

int ctohex(char c)
{
  assert(isxdigit(c));
 
  c = toupper(c); 
  c -= '0';
  if (c > 9) c -= 7;
  assert(c >= 0);	/* warning on AIX - apparently chars are unsigned */
  assert(c <  16);
  return c;
}

/***************************************************************************/

char hextoc(int i)
{
  char c;
  
  assert(i >= 0);
  assert(i <  16);
  
  c = i+'0';
  if (c > '9') c += 7;
  assert(isxdigit(c));
  return c;
}

/**************************************************************************/
