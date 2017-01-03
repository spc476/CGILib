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
#include <stdlib.h>
#include <assert.h>

char *RFC822LineRead(FILE *in)
{
  char   *line = NULL;
  size_t  max  = 0;
  size_t  idx  = 0;
  int     c;
  
  assert(in != NULL);
  
  while((c = fgetc(in)) != EOF)
  {
    if (c == '\n')
    {
      int c1 = fgetc(in);
      if ((c1 == EOF) || (c1 == '\n') || !isspace(c1))
        ungetc(c1,in);
      else
        c = ' ';
    }
    
    if (idx == max)
    {
      char *n;
      
      max += 80;
      n    = realloc(line,max);
      if (n == NULL)
      {
        free(line);
        return NULL;
      }
      line = n;
    }
    line[idx++] = c;
    line[idx]   = '\0';
    
    if (c == '\n') break;
  }
  
  if ((line != NULL) && (*line == '\n'))
  {
    free(line);
    line = NULL;
  }
  
  return line;
}
