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

#include <assert.h>

#include "pair.h"
#include "rfc822.h"

size_t RFC822HeadersWrite(FILE *out,const List *list)
{
  struct pair *ppair;
  size_t       size;
  
  assert(out  != NULL);
  assert(list != NULL);
  
  for
  (
    size = 0 , ppair = (struct pair *)ListGetHead((List *)list);
    NodeValid(&ppair->node);
    ppair = (struct pair *)NodeNext(&ppair->node)
  )
  {
    size += RFC822HeaderWrite(out,ppair->name,ppair->value);
  }
  return(size);
}
