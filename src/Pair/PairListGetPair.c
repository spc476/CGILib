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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../nodelist.h"
#include "../pair.h"

struct pair *PairListGetPair(List const *plist,char const *name)
{
  struct pair *psp;
  
  assert(plist != NULL);
  assert(name  != NULL);
  
  psp = (struct pair *)ListGetHead(plist);
  while(NodeValid(&psp->node))
  {
    if (strcmp(psp->name,name) == 0) return(psp);
    psp = (struct pair *)NodeNext(&psp->node);
  }
  return(NULL);
}
