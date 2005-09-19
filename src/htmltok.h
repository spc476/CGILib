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
#include "stream.h"
#include "errors.h"

/************************************************************************/

#define T_TAGIGNORE	-2
#define T_EOF		-1
#define T_STRING	 0
#define T_TAG		 1
#define T_COMMENT	 2

/*************************************************************************/

typedef struct htmltoken
{
  Node    node;	
  int	  token;
  char	 *value;
  List    pairs;
  int	  state;
  Stream  input;
  Stream  acc;
  List    children;
} *HtmlToken;

/************************************************************************/

int			 (HtmlParseNew)		(HtmlToken *,Stream);
int			 (HtmlParseClone)	(HtmlToken *,HtmlToken);
int			 (HtmlParseNext)	(HtmlToken);
char			*(HtmlParseValue)	(HtmlToken);
int			 (HtmlParseToken)	(HtmlToken);
void			 (HtmlParseAddPair)	(HtmlToken,struct pair *);
struct pair		*(HtmlParseFirstOption)	(HtmlToken);
struct pair		*(HtmlParseGetPair)	(HtmlToken,const char *);
struct pair		*(HtmlParseNextValue)	(HtmlToken);
char			*(HtmlParseGetValue)	(HtmlToken,char *);
void			 (HtmlParsePrintTag)	(HtmlToken,Stream);
int			 (HtmlParseFree)	(HtmlToken *);


#ifdef SCREAM
#  define HtmlParseValue(t)		(t)->value
#  define HtmlParseToken(t)		(t)->token
#  define HtmlParseFirstOption(t)	PairListFirst(&((t)->pairs))
#  define HtmlParseGetPair(t,n)		PairListGetPair(&((t)->pairs),(n))
#  define HtmlParseGetValue(t,n)	PairListGetValue(&((t)->pairs),(n))
#  define HtmlParseAddPair(t,p)		ListAddTail((t)->pairs,&(p)->node)
#endif

/**********************************************************************/

#endif

