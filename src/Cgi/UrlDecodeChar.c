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

#include <ctype.h>
#include <assert.h>

#include "../util.h"

char UrlDecodeChar(char **psrc)
{
  char *src;
  char  c;
  
  assert(psrc  != NULL);
  assert(*psrc != NULL);
  
  src = *psrc;
  c   = *src++;
  if (c == '+')
    c = ' ';
  else if (c == '%')
  {
    if (!isxdigit(*src))   return '\0';
    if (!isxdigit(*src+1)) return '\0';
    c    = ctohex(*src) * 16 + ctohex(*(src+1));
    src += 2;
  }
  *psrc = src;
  return(c);
}
