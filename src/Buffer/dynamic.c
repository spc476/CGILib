
/**********************************************************************
*
* Copyright 2001 by Sean Conner.  All Rights Reserved.
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
************************************************************************/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "../types.h"
#include "../memory.h"
#include "../errors.h"
#include "../buffer.h"
#include "../util.h"
#include "../ddt.h"

#define DYNBUFSIZE	16384
#define PREFBLOCKSIZE	 4096

/************************************************************************/

struct dyn_node
{
  Node   node;
  size_t size;
  size_t id;
  char   buffer[DYNBUFSIZE];
};

struct dyn_buffer
{
  struct buffer    buf;
  unsigned long    seek;
  unsigned long    size;
  size_t           nid;
  size_t           idx;
  List             pools;
  struct dyn_node *curr;
};

/************************************************************************/

static struct dyn_node	*dyn_expand	(struct dyn_buffer *);
static int		 dyn_ioreq	(struct buffer     *const,int,va_list);
static int		 dyn_free	(struct dyn_buffer *);
static int		 dyn_read	(struct dyn_buffer *,void *,size_t *);
static int		 dyn_write	(struct dyn_buffer *,void *,size_t *);
static int		 dyn_seek	(struct dyn_buffer *,long *,int);

#ifdef DDT
  static int		 dynchk_node	(struct dyn_node *);
  static int		 dynchk_buffer	(struct dyn_buffer *);
#endif

/************************************************************************/

int DynamicBuffer(Buffer *pbuf)
{
  struct dyn_buffer *buf;
  
  ddt(pbuf != NULL);
  
  buf             = MemAlloc(sizeof(struct dyn_buffer));
  buf->buf.size   = sizeof(struct dyn_buffer);
  buf->buf.tag    = DYNAMICBUFFER;
  buf->buf.ioreq  = dyn_ioreq;
  buf->nid        = 0;
  buf->idx        = 0;
  buf->seek       = 0;
  buf->size       = 0;
  ListInit(&buf->pools);
  dyn_expand(buf);
  *pbuf           = (Buffer)buf;
  
  ddt(dynchk_buffer(buf));
  ddt(dynchk_node(buf->curr));
  
  return(ERR_OKAY);
}

/*********************************************************************/

static struct dyn_node *dyn_expand(struct dyn_buffer *pbuf)
{
  struct dyn_node *pdn;

  ddt(dynchk_buffer(pbuf));  
  
  pdn        = MemAlloc(sizeof(struct dyn_node));
  pdn->size  = DYNBUFSIZE;
  pdn->id    = pbuf->nid++;
  pbuf->curr = pdn;
  pbuf->idx  = 0;
  ListAddTail(&pbuf->pools,&pdn->node);
  D(ddtlog(ddtstream,"","expanding dynamic buffer");)
  
  ddt(dynchk_buffer(pbuf));
  ddt(dynchk_node(pbuf->curr));
  
  return(pdn);
}

/**********************************************************************/

static int dyn_ioreq(struct buffer *const pbuf,int cmd,va_list alist)
{
  struct dyn_buffer *pdb = (struct dyn_buffer *)pbuf;
  unsigned long     *pul;
  void              *pd;
  size_t            *ps;
  long              *pl;
  int                wh;
  int                rc;

  ddt(dynchk_buffer(pdb));
  ddt(dynchk_node(pdb->curr));  
  
  switch(cmd)
  {
    case C_BLOCKSIZE:
         pul = va_arg(alist,unsigned long *);
	 ddt(pul != NULL);
	 *pul = PREFBLOCKSIZE;
	 rc   = ERR_OKAY;
	 break;
    case C_SIZE:
         pul = va_arg(alist,unsigned long *);
         ddt(pul != NULL);
         *pul = pdb->size;
         rc   = ERR_OKAY;
         break;
    case C_READ:
         pd = va_arg(alist,void *);
         ps = va_arg(alist,size_t *);
         rc = dyn_read(pdb,pd,ps);
         break;
    case C_WRITE:
         pd = va_arg(alist,void *);
         ps = va_arg(alist,size_t *);
         rc = dyn_write(pdb,pd,ps);
         break;
    case C_SEEK:
         pl = va_arg(alist,long *);
         wh = va_arg(alist,int);
         rc = dyn_seek(pdb,pl,wh);
         break;
    case C_FREE:
         rc = dyn_free(pdb);
         break;
    case C_FLUSH:
         rc = ERR_OKAY;
         break;
    case CL_EOF:
         rc = 0;
         break;
    case C_IOCTL:
    default:
         ddt(0);
         rc = ERR_NOTIMP;
         break;
  }
  
#ifdef DDT
  if (cmd != C_FREE)
  {
    ddt(dynchk_buffer(pdb));
    ddt(dynchk_node(pdb->curr));
  }
#endif
  
  return(rc);
}

/*******************************************************************/

static int dyn_free(struct dyn_buffer *pbuf)
{
  struct dyn_node *pdn;

  ddt(dynchk_buffer(pbuf));
  ddt(dynchk_node(pbuf->curr));  
  
  while(1)
  {
    pdn = (struct dyn_node *)ListRemHead(&pbuf->pools);
    if (!NodeValid(&pdn->node)) break;
    MemFree(pdn,sizeof(struct dyn_node));
  }
  MemFree(pbuf,sizeof(struct dyn_buffer));
  return(ERR_OKAY);
}

/*******************************************************************/

static int dyn_read(struct dyn_buffer *pbuf,void *pd,size_t *ps)
{
  char            *data;
  size_t           size;
  size_t           tsize;
  size_t           amt;
  struct dyn_node *pdn;
  
  ddt(dynchk_buffer(pbuf));
  ddt(dynchk_node(pbuf->curr));
  ddt(pd   != NULL);
  ddt(ps   != NULL);
  ddt(*ps  != 0);
    
  data = pd;
  pdn  = pbuf->curr;
  size = tsize = (*ps < pbuf->size - pbuf->seek) 
  			? *ps 
			: pbuf->size - pbuf->seek;

  while(size)
  {
    if (pbuf->seek >= pbuf->size)	/* end of our dyn_buffer? */
      break;
    
    if (pbuf->idx == pdn->size)		/* end of a node?  go to next */
    {
      pdn        = (struct dyn_node *)NodeNext(&pdn->node);
      pbuf->curr = pdn;
      pbuf->idx  = 0;
      ddt(NodeValid(&pdn->node));
      D(ddtlog(ddtstream,"L","going to next block %a",(unsigned long)pdn->id);)
    }
      
    if (size < (pdn->size - pbuf->idx))	/* we can do this in one swoop */
      amt = size;
    else
      amt = pdn->size - pbuf->idx;
    
    memcpy(data,&pdn->buffer[pbuf->idx],amt);
    pbuf->idx   += amt;
    pbuf->seek  += amt;
    size        -= amt;
  }
  *ps = tsize - size;
  
  ddt(dynchk_buffer(pbuf));
  ddt(dynchk_node(pbuf->curr));
  
  return(ERR_OKAY);
}

/*********************************************************************/

static int dyn_write(struct dyn_buffer *pbuf,void *pd,size_t *ps)
{
  char            *data;
  size_t           size;
  size_t           tsize;
  size_t           amt;
  struct dyn_node *pdn;
  
  ddt(dynchk_buffer(pbuf));
  ddt(dynchk_node(pbuf->curr));
  ddt(pd   != NULL);
  ddt(ps   != NULL);
  ddt(*ps  != 0);
  
  data = pd;
  size = tsize = *ps;
  pdn  = pbuf->curr;

  while(size)
  {
    ddt(pbuf->idx <= pdn->size);
    if (pbuf->idx >= pdn->size)		/* end of a node?  go to next */
      pdn = dyn_expand(pbuf);
      
    if (size < (pdn->size - pbuf->idx))	/* we can do this in one swoop */
      amt = size;
    else
      amt = pdn->size - pbuf->idx;
    
    memcpy(&pdn->buffer[pbuf->idx],data,amt);
    pbuf->idx   += amt;
    pbuf->seek  += amt;
    size        -= amt;
    if (pbuf->seek > pbuf->size)
      pbuf->size = pbuf->seek;
  }
  *ps = tsize - size;
  
  ddt(dynchk_buffer(pbuf));
  ddt(dynchk_node(pbuf->curr));
  
  return(ERR_OKAY);
}

/*********************************************************************/

static int dyn_seek(struct dyn_buffer *pbuf,long *pl,int whence)
{
  long             seek;
  unsigned long    useek;
  unsigned long    stseek;	/* this is assigned before use (-Wall ...)*/
  Size             size;
  Size             nseek;
  Size             amt;
  struct dyn_node *pdn;		/* this is assigned before use (-Wall ... )*/

  ddt(dynchk_buffer(pbuf));
  ddt(dynchk_node(pbuf->curr));
  ddt(pl         != NULL);
  ddt(
       (whence == SEEK_START) 
       || (whence == SEEK_CURRENT) 
       || (whence == SEEK_END)
     );

  seek  = *pl;
  useek = labs(seek);
  
  switch(whence)
  {
    case SEEK_START:   stseek = 0;		 break;
    case SEEK_CURRENT: stseek = pbuf->seek; 	 break;
    case SEEK_END:     stseek = pbuf->size - 1;  break;
    default:           ddt(0); return(ERR_GENERAL);
  }
  
  /*--------------------------------------------------------------------
  ; ``For Muofubumo has computed that nothing exists before creation, yet
  ;   much exists beyond now that it is without bound.  We are finite
  ;   before but infinite to come says Muofubumo.  Until the disks fill
  ;   up sings Aeweat.''
  ;			-- From ``Song of Muofubumo'', II.iii
  ;---------------------------------------------------------------------*/
  
  if (seek < 0)
    if (useek > stseek)
      nseek = 0;
    else
      nseek = stseek - useek;
  else
    nseek = stseek + useek;

  if (nseek < pbuf->size)
  {
    pbuf->seek = nseek;
    for (
          pdn = (struct dyn_node *)ListGetHead(&pbuf->pools);
          (NodeValid(&pdn->node)) && (nseek > pdn->size);
          pdn = (struct dyn_node *)NodeNext(&pdn->node)
        )
    {
      nseek -= pdn->size;
    }
    pbuf->curr = pdn;
    pbuf->idx  = nseek;  
  }
  else
  {
    /*-----------------------------------------------------------------
    ; expand our horizons.  We do this by "writing" into the buffer.
    ; see dyn_write() for more information
    ;-----------------------------------------------------------------*/
  
    size = nseek - pbuf->seek;	/* amount to expand by */
    pdn  = pbuf->curr;

    while(size)
    {
      if (pbuf->idx == pdn->size)
        pdn = dyn_expand(pbuf);
      if (size < (pdn->size - pbuf->idx))
        amt = size;
      else
        amt = pdn->size - pbuf->idx;
      pbuf->idx  += amt;
      pbuf->seek += amt;
      size       -= amt;
    }
  }

  ddt(dynchk_buffer(pbuf));
  ddt(dynchk_node(pbuf->curr));
  D(ddtlog(ddtstream,"L","dynamic buffer block %a",(unsigned long)pbuf->curr->id);)

  *pl = pbuf->seek;
  return(ERR_OKAY);
}

/************************************************************************/

#ifdef DDT

  static int dynchk_node(struct dyn_node *pdn)
  {
    if (pdn == NULL)
    {
      ddtlog(ddtstream,"","NULL dyn_node");
      return(0);
    }
    if (pdn->size != DYNBUFSIZE)
    {
      ddtlog(ddtstream,"","corrupt dyn_node size");
      return(0);
    }
    return(1);
  }
  
  /*------------------------------------------------------------------*/
  
  static int dynchk_buffer(struct dyn_buffer *pbuf)
  {
    if (pbuf == NULL)
    {
      ddtlog(ddtstream,"","NULL dyn_buffer");
      return(0);
    }
    if (pbuf->seek > pbuf->size)
    {
      ddtlog(ddtstream,"","dyn_buffer - we've seeked into the unknown");
      return(0);
    }
    if (pbuf->idx > DYNBUFSIZE)
    {
      ddtlog(ddtstream,"","dyn_buffer - we're past a node");
      return(0);
    }
    return(1);
  }

#endif

  
