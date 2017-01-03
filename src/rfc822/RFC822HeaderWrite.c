/***************************************************************************
*
* Copyright 2009,2013 by Sean Conner.
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
#include <stdio.h>
#include <string.h>
#include <assert.h>

size_t RFC822HeaderWrite(FILE *out,const char *restrict name,const char *restrict value)
{
  int        (*conv)(int);
  int          size;
  char         n[strlen(name) + 1];
  const char  *s;
  char        *d;
  
  assert(out   != NULL);
  assert(name  != NULL);
  assert(value != NULL);
  
  for (conv = (toupper) , s = name , d = n ; ; s++ , d++)
  {
    *d   = (*conv)(*s);
    conv = isalpha(*d) ? (tolower) : (toupper);
    if (*d == '\0') break;
  }
  
  size = fprintf(out,"%s: %s\n",n,value);
  if (size < 0)
    size = 0;
  return(size);
}
