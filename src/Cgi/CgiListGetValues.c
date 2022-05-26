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

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../nodelist.h"
#include "../pair.h"
#include "../cgi.h"

size_t CgiListGetValues(Cgi const cgi,char ***darray,char const *name)
{
  size_t        size  = 0;
  size_t        idx   = 0;
  char        **store = NULL;
  struct pair  *pair;
  
  assert(darray != NULL);
  assert(name   != NULL);
  
  pair = CgiListGetPair(cgi,name);
  if (pair == NULL)
  {
    *darray = NULL;
    return(0);
  }
  
  while(NodeValid(&pair->node))
  {
    if (strcmp(pair->name,name) == 0)
    {
      if (idx == size)
      {
        char **newstore = realloc(store,size+256);
        if (newstore == NULL)
          break;
        store = newstore;
        size  += 256;
      }
      store[idx++] = pair->value;
    }
    pair = (struct pair *)NodeNext(&pair->node);
  }
  *darray = store;
  return(idx);
}
