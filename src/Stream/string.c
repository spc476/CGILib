/************************************************************************
*
* Copyright 2005 by Sean Conner.  All Rights Reserved.
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

#include <string.h>

#include "../ddt.h"
#include "../memory.h"
#include "../nodelist.h"
#include "../types.h"
#include "../stream.h"

#define STRING_BLOCK_SIZE	8192

/*********************************************************************/

struct stringstream
{
  List    blocks;
  List    avail;
  size_t  offset;
};

struct stringblock
{
  Node           node;
  size_t         size;
  unsigned char *data;
};

/***********************************************************************/

static int string_write(struct stream *s,struct streamvector *v,int c)
{
  struct stringstream *ss;
  struct stringblock  *sb;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  ss = v->user;
  sb = (struct stringblock *)ListGetTail(&ss->blocks);
  
  while(1)
  {
    if (sb->size < STRING_BLOCK_SIZE)
    {
      ss->offset++;
      sb->data[sb->size++] = c;
      return(c);
    }
         
    sb = (struct stringblock *)ListRemHead(&ss->avail);
    if (!NodeValid(&sb->node))
    {
      sb       = MemAlloc(sizeof(struct stringblock));
      sb->data = MemAlloc(STRING_BLOCK_SIZE);
    }
    
    sb->size = 0;
    ListAddTail(&ss->blocks,&sb->node);
  }
}

/*************************************************************************/
  
static int string_unread(struct stream *s,struct streamvector *v,int c)
{
  struct stringstream *ss;
  struct stringblock  *sb;
  struct stringblock  *sbn;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);

  ss = v->user;
  sb = (struct stringblock *)ListGetTail(&ss->blocks);
  
  while(1)
  {  
    /*-----------------------------------------------
    ; if any characters to remove, remove them
    ;------------------------------------------------*/
    
    if (sb->size)
    {
      ddt(ss->offset > 0);
      ss->offset--;
      sb->size--;
      sb->data[sb->size] = c;
      return(c);
    }
  
    /*---------------------------------------------------------
    ; otherwise, we need to retrieve a filled block, then start
    ; removing characters from it
    ;-----------------------------------------------------------*/
  
    sbn = (struct stringblock *)NodePrev(&sb->node);
    if (!NodeValid(&sb->node))	/* at end of first block */
      return(IEOF);
    
    NodeRemove(&sb->node);		/* remove this block */
    ListAddTail(&ss->avail,&sb->node);	/* move to available list */
    sb = sbn;				/* look at previous block */
  }
}

/***********************************************************************/

static int string_flush(struct stream *s,struct streamvector *v)
{
  struct stringstream *ss;
  List                *blist;
  List                *alist;
  Node                *bh;
  Node                *bt;
  Node                *at;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  ss = v->user;
  
  blist = &ss->blocks;
  alist = &ss->avail;

  bh = blist->lh_Head;
  bt = blist->lh_TailPred;
  at = alist->lh_TailPred;
  
  at->ln_Succ        = bh;
  bh->ln_Pred        = at;
  bt->ln_Succ        = (Node *)&alist->lh_Tail;
  alist->lh_TailPred = bt;

  ListInit(blist);
  bh = ListRemHead(alist);
  ListAddTail(blist,bh);

  ss->offset = ((struct stringblock *)bh)->size = 0;

  return(0);
}

/************************************************************************/

static int string_close(struct stream *s,struct streamvector *v)
{
  struct stringstream *ss;
  struct stringblock  *sb;
  
  ddt(s       != NULL);
  ddt(v       != NULL);
  ddt(v->user != NULL);
  
  ss = v->user;
  
  for
  (
    sb = (struct stringblock *)ListRemHead(&ss->blocks);
    NodeValid(&sb->node);
    sb = (struct stringblock *)ListRemHead(&ss->blocks)
  )
  {
    MemFree(sb->data);
    MemFree(sb);
  }
  
  for
  (
    sb = (struct stringblock *)ListRemHead(&ss->avail);
    NodeValid(&sb->node);
    sb = (struct stringblock *)ListRemHead(&ss->avail)
  )
  {
    MemFree(sb->data);
    MemFree(sb);
  }
  
  MemFree(ss);
  return(0);
}

/***********************************************************************/
  
Stream (StringStreamWrite)(void)
{
  Stream s;
  struct stringstream *ss;
  struct stringblock  *sb;
  
  s = StreamNewRead();
  s->calls.readwrite   = string_write;
  s->calls.unrw        = string_unread;
  s->calls.refillflush = string_flush;
  s->calls.close       = string_close;
  s->calls.user        = MemAlloc(sizeof(struct stringstream));
  s->data              = NULL;
  s->eof               = FALSE;

  ss         = s->calls.user;
  ss->offset = 0;
  
  ListInit(&ss->blocks);
  ListInit(&ss->avail);
  
  sb       = MemAlloc(sizeof(struct stringblock));
  sb->size = 0;
  sb->data = MemAlloc(STRING_BLOCK_SIZE);

  ListAddTail(&ss->blocks,&sb->node);
  
  return(s);
}

/************************************************************************/

char *(StringFromStream)(const Stream s)
{
  struct stringstream *ss;
  struct stringblock  *sb;
  char                *text;
  char                *p;
  
  ddt(s             != NULL);
  ddt(s->calls.user != NULL);

  ss = s->calls.user;
  p  = text = MemAlloc(ss->offset + 1);
  
  for
  (
    sb = (struct stringblock *)ListGetHead(&ss->blocks) ;
    NodeValid(&sb->node) ;
    sb = (struct stringblock *)NodeNext(&sb->node)
  )
  {
    memcpy(p,sb->data,sb->size);
    p += sb->size;
  }
  
  text[ss->offset] = '\0';
  
  return(text);
}

/*************************************************************************/

