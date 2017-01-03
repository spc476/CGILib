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
#include <assert.h>

#include "../nodelist.h"

void NodeRemove(Node *const pn)
{
  Node *pns;
  Node *pnp;
  
  assert(pn          != NULL);
  assert(pn->ln_Succ != NULL);
  assert(pn->ln_Pred != NULL);
  
  pns = pn->ln_Succ;
  pnp = pn->ln_Pred;
  
  pns->ln_Pred = pnp;
  pnp->ln_Succ = pns;
}
