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

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "../nodelist.h"
#include "../htmltok.h"

HtmlToken HtmlParseClone(HtmlToken token)
{
  HtmlToken    pht;
  struct pair *pair;
  struct pair *pairp;
  
  assert(token != NULL);
  
  /*----------------------------------
  ; do I really need this call?
  ;-----------------------------------*/
  
  pht         = malloc(sizeof(struct htmltoken));
  pht->token  = token->token;
  pht->state  = token->state;
  pht->value  = strdup(token->value);
  pht->input  = token->input;
  pht->data   = NULL;
  pht->max    = 0;
  pht->idx    = 0;
  
  ListInit(&pht->pairs);
  
  for (
        pair = PairListFirst(&token->pairs) ;
        NodeValid(&pair->node) ;
        pair = (struct pair *)NodeNext(&pair->node)
      )
  {
    pairp = PairClone(pair);
    ListAddTail(&pht->pairs,&pairp->node);
  }
  
  return(pht);
}
