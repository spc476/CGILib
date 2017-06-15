/***************************************************************************
*
* Copyright 2011,2013 by Sean Conner.
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
#include <assert.h>

#include "bisearch.h"

/*************************************************************************/

bisearch__t bisearch(
        void   const *const restrict key,
        void   const *const restrict base,
        size_t const                 nelem,
        size_t const                 size,
        int (*compare)(void const *restrict,void const *restrict)
)
{
  size_t first = 0;
  size_t len   = nelem;
  
  assert(key  != NULL);
  assert(size >  0);
  
  while(len > 0)
  {
    assert(base != NULL);
    
    size_t      half   = len / 2;
    size_t      middle = first + half;
    char const *pivot  = (char const *)base + (middle * size);
    int         q      = (*compare)(key,pivot);
    
    if (q > 0)
    {
      first = middle + 1;
      len   = len - half - 1;
    }
    else if (q == 0)
      return (bisearch__t){ .datum = (void *)pivot , .idx = middle };
    else
      len = half;
  }
  
  return (bisearch__t){ .datum = NULL , .idx = first };
}

/**************************************************************************/
