/************************************************************************
*
* Copyright 2007 by Sean Conner.  All Rights Reserved.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
* Comments, questions and criticisms can be sent to: sean@conman.org
*
*************************************************************************/

#include "../ddt.h"
#include "../memory.h"
#include "../types.h"

#define BUNDLE_BUMP	8

/*******************************************************/

struct bundlemod
{
  size_t  idx;
  size_t  cur;
  size_t  max;
  Stream *data;
};

/********************************************************/

static int	b_read		(struct stream *,struct streamvector *);
static int	b_close		(struct stream *,struct streamvector *);

/********************************************************/

Stream (BundleStreamRead)(void)
{
  struct bundlemod *bmod;
  Stream            s;

  bmod               = MemAlloc(sizeof(struct bundlemod));
  bmod->idx          = 0;
  bmod->cur          = 0;
  bmod->max          = 0;
  bmod->data         = NULL;
  
  s                  = StreamNewRead();
  s->calls.user      = bmod;
  s->calls.readwrite = b_read;
  s->calls.close     = b_close;
  
  return(s);
}

/*********************************************************/

void (BundleReadAdd)(const Stream b,const Stream s)
{
  struct bundlemod *mod;
  
  ddt(b       != NULL);
  ddt(b->type == MODE_READ);
  ddt(s       != NULL);
  ddt(s->type == MODE_READ);
  
  mod = b->calls.user;
  if (mod->idx == mod->max)
  {
    mod->max += BUNDLE_BUMP;
    mod->data = MemResize(mod->data,mod->max * sizeof(struct bundlemod));
  }
  
  mod->data[mod->idx++] = s;
  b->eof                = FALSE;
}

/*********************************************************/

static int b_read(struct stream *s,struct streamvector *sv)
{
  struct bundlemod *mod;
  int               c;
  
  ddt(s       != NULL);
  ddt(s->type == MODE_READ);
  ddt(sv      != NULL);
  
  if (s->eof) return(EOF);
  
  mod = sv->user;
  
  while(1)
  {
    if (StreamEOF(mod->data[mod->cur]))
    {
      mod->cur++;
      if (mod->cur == mod->idx)
      {
        s->eof = TRUE;
        return(IEOF);
      }
      continue;
    }
    
    c = StreamRead(mod->data[mod->cur]);
    if (c == IEOF) 
      continue;

    return(c);
  }
  ddt(0);
  return(EOF);
}

/************************************************************/

static int b_close(struct stream *s,struct streamvector *sv)
{
  struct bundlemod *mod;
  
  ddt(s       != NULL);
  ddt(s->type == MODE_READ);
  ddt(sv      != NULL);
  
  mod = sv->user;
  
  if (mod->data) MemFree(mod->data);
  MemFree(mod);
  return(0);
}

/************************************************************/

