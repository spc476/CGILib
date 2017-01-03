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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../nodelist.h"
#include "../htmltok.h"
#include "../util.h"

void HtmlParsePrintTag(HtmlToken token,FILE *out)
{
  struct pair *pp;
  
  fprintf(out,"<%s",HtmlParseValue(token));
  
  for
  (
    pp = HtmlParseFirstOption(token);
    NodeValid(&pp->node);
    pp = (struct pair *)NodeNext(&pp->node)
  )
  {
    if (!emptynull_string(pp->name))
    {
      fprintf(out," %s=",pp->name);
      if (emptynull_string(pp->value))
      {
        if (strcmp(pp->name,"ALT") != 0)
          fprintf(out,"\"%s\"",pp->name);
        else
          fprintf(out,"\"\"");
      }
      else
        fprintf(out,"\"%s\"",pp->value);
    }
  }
  
  fputc('>',out);
}
