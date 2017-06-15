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

#include <assert.h>

#include "../cgi.h"

int CgiListRequired(Cgi const cgi,struct dstring *table,size_t size)
{
  int cnt = 0;
  
  assert(cgi   != NULL);
  assert(table != NULL);
  assert(size  >  0);
  
  while(size)
  {
    if ((table->s2 = CgiListGetValue(cgi,table->s1)) == NULL)
      return(cnt);
    cnt++;
    table++;
    size--;
  }
  return(0);
}
