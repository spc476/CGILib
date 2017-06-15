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

#include <stdlib.h>
#include <assert.h>

#include "../nodelist.h"
#include "../pair.h"
#include "../rfc822.h"
#include "../util.h"

void RFC822HeadersRead(FILE *in,List const *list)
{
  char        *line;
  char        *t;
  struct pair *ppair;
  
  assert(in   != NULL);
  assert(list != NULL);
  
  while((line = RFC822LineRead(in)) != NULL)
  {
    t            = line;
    ppair        = PairNew(&t,':','\0');
    ppair->name  = trim_space(ppair->name);
    ppair->value = trim_space(ppair->value);
    up_string(ppair->name);
    ListAddTail((List *)list,&ppair->node);
    free(line);
  }
}
