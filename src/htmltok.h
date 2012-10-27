/************************************************************************
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

#ifndef HTMLTOK_H
#define HTMLTOK_H

#include <stdio.h>

#include "nodelist.h"
#include "pair.h"

/************************************************************************/

typedef enum htoken
{
  T_EOF,
  T_STRING,
  T_TAG,
  T_COMMENT,
  T_TAGIGNORE
} HToken;

/*************************************************************************/

typedef struct htmltoken
{
  Node    node;	
  HToken  token;
  char	 *value;
  List    pairs;
  int	  state;
  FILE   *input;
  List    children;
  char   *data;
  size_t  max;
  size_t  idx;
} *HtmlToken;

/************************************************************************/

HtmlToken		 (HtmlParseNew)		(FILE *);
HtmlToken		 (HtmlParseClone)	(HtmlToken);
int			 (HtmlParseNext)	(HtmlToken);
void			 (HtmlParsePrintTag)	(HtmlToken,FILE *);
int			 (HtmlParseFree)	(HtmlToken);

/**********************************************************************/

static inline char *(HtmlParseValue)(HtmlToken token)
{
  assert(token != NULL);
  return token->value;
}

/*----------------------------------------------------------------*/

static inline HToken (HtmlParseToken)(HtmlToken token)
{
  assert(token != NULL);
  return(token->token);
}

/*------------------------------------------------------------------*/

static inline struct pair *(HtmlParseFirstOption)(HtmlToken token)
{
  assert(token != NULL);
  return PairListFirst(&token->pairs);
}

/*-----------------------------------------------------------------*/

static inline void (HtmlParseAddPair)(HtmlToken token,struct pair *p)
{
  assert(token != NULL);
  assert(p     != NULL);
  
  ListAddTail(&token->pairs,&p->node);
}

/*------------------------------------------------------------------*/

static inline struct pair *(HtmlParseGetPair)(HtmlToken token,const char *name)
{
  assert(token != NULL);
  assert(name  != NULL);
  return(PairListGetPair(&token->pairs,name));
}

/*-------------------------------------------------------------------*/

static inline char *(HtmlParseGetValue)(HtmlToken token,char *name)
{
  assert(token != NULL);
  assert(name  != NULL);
  
  return(PairListGetValue(&token->pairs,name));
}

/*------------------------------------------------------------------*/

#endif

