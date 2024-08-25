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

#ifdef __GNUC__
#  define _GNU_SOURCE
#endif

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../pair.h"

struct pair *PairCreate(char const *restrict name,char const *restrict value)
{
  struct pair *psp;
  
  assert(name  != NULL);
  assert(value != NULL);
  
  psp = malloc(sizeof(struct pair));
  if (psp != NULL)
  {
    psp->node->ln_Succ = NULL;
    psp->node->ln_Pred = NULL;
    psp->tree->left    = NULL;
    psp->tree->right   = NULL;
    psp->tree->height  = 0;
    psp->name          = strdup(name);
    psp->value         = strdup(value);
  }
  return(psp);
}
