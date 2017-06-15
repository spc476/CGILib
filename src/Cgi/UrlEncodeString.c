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

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../cgi.h"

char *UrlEncodeString(char const *src)
{
  size_t  nsize;
  char   *dest;
  char   *p;
  
  assert(src != NULL);
  
  nsize = strlen(src) * 3 + 1;
  dest  = malloc(nsize);
  
  for ( p = dest ; *src ; src++)
    p = UrlEncodeChar(p,*src);
    
  *p = 0;
  
  return dest;
}
