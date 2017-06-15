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

#ifndef I_436B219C_7E42_53BF_B0C4_134445F2071A
#define I_436B219C_7E42_53BF_B0C4_134445F2071A

#include <stdio.h>

#include "nodelist.h"
#include "pair.h"

/************************************************************************/

enum
{
  S_EOF,
  S_STRING,
  S_TAG,
  S_COMMENT,
  S_TAGIGNORE
};

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
  char   *value;
  List    pairs;
  int     state;
  FILE   *input;
  List    children;
  char   *data;
  size_t  max;
  size_t  idx;
} *HtmlToken;

/************************************************************************/

extern HtmlToken HtmlParseNew      (FILE *);
extern HtmlToken HtmlParseClone    (HtmlToken);
extern int       HtmlParseNext     (HtmlToken);
extern void      HtmlParsePrintTag (HtmlToken,FILE *);
extern int       HtmlParseFree     (HtmlToken);

/**********************************************************************/

static inline char *HtmlParseValue(HtmlToken token)
{
  assert(token != NULL);
  return token->value;
}

/*----------------------------------------------------------------*/

static inline HToken HtmlParseToken(HtmlToken token)
{
  assert(token != NULL);
  return(token->token);
}

/*------------------------------------------------------------------*/

static inline struct pair *HtmlParseFirstOption(HtmlToken token)
{
  assert(token != NULL);
  return PairListFirst(&token->pairs);
}

/*-----------------------------------------------------------------*/

static inline void HtmlParseAddPair(HtmlToken token,struct pair *p)
{
  assert(token != NULL);
  assert(p     != NULL);
  
  ListAddTail(&token->pairs,&p->node);
}

/*------------------------------------------------------------------*/

static inline struct pair *HtmlParseGetPair(HtmlToken token,char const *name)
{
  assert(token != NULL);
  assert(name  != NULL);
  return(PairListGetPair(&token->pairs,name));
}

/*-------------------------------------------------------------------*/

static inline char *HtmlParseGetValue(HtmlToken token,char *name)
{
  assert(token != NULL);
  assert(name  != NULL);
  
  return(PairListGetValue(&token->pairs,name));
}

/*------------------------------------------------------------------*/

#endif

