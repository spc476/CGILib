
/*************************************************************************
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
*************************************************************************/

#include "types.h"
#include "nodelist.h"
#include "memory.h"
#include "clean.h"
#include "ddt.h"

struct cleanitem
{
  Node   node;
  Tag    tag;
  void  *datum;
  int  (*funct)(void *);
};

static List cleanlist;

/********************************************************************/

void CleanInit(void)
{
  ListInit(&cleanlist);
}

/*******************************************************************/ 

void CleanThis(Tag tag,void *datum,int (*funct)(void *))
{
  struct cleanitem *pce;

  ddt(datum != NULL);
  ddt(funct);
  
  pce = MemAlloc(sizeof(struct cleanitem));
  pce->tag   = tag;
  pce->datum = datum;
  pce->funct = funct;
  ListAddHead(&cleanlist,&pce->node);
}

/*********************************************************************/

void CleanUpAll(void)
{
  struct cleanitem *pce;
  
  while(NodeValid((Node *)(pce = (struct cleanitem *)ListRemHead(&cleanlist))))
  {
    (*pce->funct)(pce->datum);
    MemFree(pce,sizeof(struct cleanitem));
  }
}

/**********************************************************************/

void CleanUp(Tag tag)
{
  struct cleanitem *pce;
   
  pce = (struct cleanitem *)ListGetHead(&cleanlist);
  while(NodeValid(&pce->node))
  {
    if (pce->tag == tag)
    {
      NodeRemove(&pce->node);
      (*pce->funct)(pce->datum);
      MemFree(pce,sizeof(struct cleanitem));
      pce = (struct cleanitem *)ListGetHead(&cleanlist);
    }
    else
    {
      pce = (struct cleanitem *)NodeNext(&pce->node);
    }
  }
}

/**********************************************************************/

