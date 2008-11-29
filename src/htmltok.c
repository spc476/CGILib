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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "types.h"
#include "nodelist.h"
#include "cgi.h"
#include "memory.h"
#include "stream.h"
#include "errors.h"
#include "util.h"
#include "ddt.h"
#include "htmltok.h"

#define S_TAGIGNORE	-2
#define S_EOF		-1
#define S_STRING	 0
#define S_TAG		 1
#define S_COMMENT	 2

/************************************************************************/

static int	 ht_nexteof		(HtmlToken);
static int	 ht_nextstr		(HtmlToken);
static int	 ht_nexttag		(HtmlToken);
static int	 ht_nextcom		(HtmlToken);
static void	 ht_makepair		(HtmlToken,char *,char *);
static void	 entify_char		(char *,size_t,char *s,char e,const char *);

/*************************************************************************/

int (HtmlParseNew)(HtmlToken *phtoken,Stream input)
{
  HtmlToken pht;
  
  ddt(phtoken != NULL);
  ddt(input   != NULL);
  
  pht         = MemAlloc(sizeof(struct htmltoken));
  pht->token  = T_STRING;
  pht->value  = NULL;	/* dup_string(""); */ /* am i perpetuating a hack? */
  pht->state  = S_STRING;
  pht->input  = input;
  pht->acc    = StringStreamWrite();
  ListInit(&pht->pairs);
  *phtoken    = pht;
  return(ERR_OKAY);
}

/***********************************************************************/

int (HtmlParseClone)(HtmlToken *pnew,HtmlToken token)
{
  /* XXX do I really need this call? */
  HtmlToken    pht;
  struct pair *pair;
  struct pair *pairp;

  
  ddt(pnew != NULL);
  ddt(token != NULL);
  
  pht         = MemAlloc(sizeof(struct htmltoken));
  pht->token  = token->token;
  pht->state  = token->state;
  pht->value  = dup_string(token->value);
  pht->input  = token->input;
  pht->acc    = StringStreamWrite();
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
  
  *pnew = pht;
  return(ERR_OKAY);
}

/**********************************************************************/

int (HtmlParseNext)(HtmlToken token)
{
  ddt(token != NULL);

  if(token->value) 
  {
    D(ddtlog(ddtstream,"$","about to free %a",token->value);)
    MemFree(token->value);
  }
  
  token->value = NULL;
  PairListFree(&token->pairs);
  StreamFlush(token->acc);
  
  switch(token->state)
  {
    case S_EOF:		return(ht_nexteof(token));
    case S_STRING:	return(ht_nextstr(token));
    case S_TAG:		return(ht_nexttag(token));
    case S_COMMENT:	return(ht_nextcom(token));
    default:
         ddt(0);	/* this shouldn't happen */
  }
  ddt(0);		/* and this shouldn't happen either */
  return(S_EOF);	/* shut up -Wall -pedantic -ansi options to gcc */
}

/********************************************************************/

char *(HtmlParseValue)(HtmlToken token)
{
  ddt(token != NULL);
  return(token->value);
}

/*******************************************************************/

int (HtmlParseToken)(HtmlToken token)
{
  ddt(token != NULL);
  return(token->token);
}

/*******************************************************************/

struct pair *(HtmlParseFirstOption)(HtmlToken token)
{
  ddt(token != NULL);
  return(PairListFirst(&token->pairs));
}

/****************************************************************/

void (HtmlParseAddPair)(HtmlToken token,struct pair *p)
{
  ddt(token != NULL);
  ddt(p     != NULL);
  
  ListAddTail(&token->pairs,&p->node);
}

/******************************************************************/

struct pair *(HtmlParseGetPair)(HtmlToken token,const char *name)
{
  ddt(token != NULL);
  ddt(name  != NULL);
  return(PairListGetPair(&token->pairs,name));
}

/********************************************************************/

char *(HtmlParseGetValue)(HtmlToken token,char *name)
{
  ddt(token != NULL);
  ddt(name  != NULL);
  
  return(PairListGetValue(&token->pairs,name));
}

/*******************************************************************/

void (HtmlParsePrintTag)(HtmlToken token,Stream out)
{
  struct pair *pp;
  
  LineSFormat(out,"$","<%a",HtmlParseValue(token));
  
  for
  (
    pp = HtmlParseFirstOption(token);
    NodeValid(&pp->node);
    pp = (struct pair *)NodeNext(&pp->node)
  )
  {
    char *sq;
    char *dq;
    char  q;
    char *value;
    char  tvalue[BUFSIZ];
    
    value = pp->value;
    sq    = strchr(value,'\'');
    dq    = strchr(value,'"');
    
    if (dq == NULL)
      q = '"';
    else if ((sq == NULL) && (dq != NULL))
      q = '\'';
    else
    {
      entify_char(tvalue,BUFSIZ,value,'"',"&quot;");
      value = tvalue;
      q     = '"';
    }
    
    if (!emptynull_string(pp->name))
      LineSFormat(out,"$"," %a",pp->name);
    if (!emptynull_string(pp->value))
      LineSFormat(out,"c $","=%a%b%a",q,pp->value);
    else
      LineSFormat(out,"c","=%a%a",q);
  }
  
  StreamWrite(out,'>');
}

/***********************************************************************/

int (HtmlParseFree)(HtmlToken *ptoken)
{
  HtmlToken token;
  
  ddt(ptoken  != NULL);
  ddt(*ptoken != NULL);

  token = *ptoken;
  
  if (token->value != NULL)
  {
    ddt(token->value != NULL);
    MemFree(token->value);
  }

  StreamFree(token->acc);
  PairListFree(&token->pairs);
  MemFree(token);
  *ptoken = NULL;
  return(ERR_OKAY);
}  

/***********************************************************************/
  
static int ht_nexteof(HtmlToken token)
{
  ddt(token != NULL);
  
  token->token = T_EOF;
  return(T_EOF);
}

/*******************************************************************/

static int ht_nextstr(HtmlToken token)
{
  int c;
  int st = S_EOF;
    
  ddt(token != NULL);

  /* while ((c = StreamRead(token->input)) != IEOF) */
  while(!StreamEOF(token->input))
  {
    c = StreamRead(token->input);
    if (c == IEOF) break;
    if (c == '<')
    {
      st = S_TAG;
      break;
    }
    else
      StreamWrite(token->acc,c);
  }

  token->value = StringFromStream(token->acc);
  token->token = T_STRING;
  token->state = st;
  return(T_STRING);
}

/********************************************************************/

static int ht_nexttag(HtmlToken token)
{
  int   c;
  char *t;
  int   level;
    
  ddt(token != NULL);  
  
  /*-----------------------------------------------------------
  ; but parsing HTML has proven to be a real bitch to program,
  ; especially testing for unexpected end of files and what not, 
  ; so I'm dropping down a level, so to speak.  This will probably
  ; produce horrible code, but at this point, nothing else has
  ; worked reliably, so ... 
  ;------------------------------------------------------------*/  
  
  	/*-------------------------------------------------
  	; STAGE ALPHA - we just read a '<', so skip past 
  	; any white space till we hit the tag
  	;--------------------------------------------------*/
 
htnt_alpha:	c = StreamRead(token->input);
		if (c == IEOF)  goto htnt_error;
		if (isspace(c)) goto htnt_alpha;
		if (c == '>' ) 	goto htnt_alpha10;	/* done - return tag */
		if (c == '!' ) 	goto htnt_comment;
		goto htnt_gottag;

htnt_alpha10:	token->value = up_string(StringFromStream(token->acc));
                StreamFlush(token->acc);
		D(ddtlog(ddtstream,"$","just got %a",token->value);)
		goto htnt_done;

	/*-----------------------------------------------
	; STAGE BETA - read the tag and save
	;----------------------------------------------*/
	
htnt_gottag:	StreamWrite(token->acc,c);
		c = StreamRead(token->input);
		if (c == IEOF) 	goto htnt_error;
		if (c == '=' ) 	goto htnt_error;
		if (c == '>' ) 	goto htnt_alpha10;	/* done - return tag */
		if (!isspace(c)) goto htnt_gottag;

		token->value = up_string(StringFromStream(token->acc));
		StreamFlush(token->acc);
		D(ddtlog(ddtstream,"$","parsed %a",token->value);)
		
	/*-------------------------------------------------
	; STAGE GAMMA - skip space to options
	;------------------------------------------------*/
	
htnt_bopts:	c = StreamRead(token->input);
		if (c == IEOF)	goto htnt_error;
		if (isspace(c))	goto htnt_bopts;
		if (c == '>')	goto htnt_done;
		if (c == '=')	goto htnt_error;
		
	/*-----------------------------------------------
	; STAGE DELTA - read in option
	;-----------------------------------------------*/
	
htnt_gotopt:	StreamWrite(token->acc,c);
		c = StreamRead(token->input);
		if (c == IEOF)	goto htnt_error;
		if (isspace(c))	goto htnt_boptval;
		if (c == '>')	goto htnt_gotoptd;
		if (c == '=')	goto htnt_voptval;
		goto htnt_gotopt;

htnt_gotoptd:	ht_makepair(token,up_string(StringFromStream(token->acc)),dup_string(""));
		StreamFlush(token->acc);
		goto htnt_done;

	/*------------------------------------------------
	; STAGE EPSILON - between an option and possibly its
	; value - or another option
	;-------------------------------------------------*/

htnt_boptval:	c = StreamRead(token->input);
		if (c == IEOF)	goto htnt_error;
		if (isspace(c))	goto htnt_boptval;
		if (c == '>')	goto htnt_gotoptd;
		if (c == '=')	goto htnt_voptval;
		
		ht_makepair(token,up_string(StringFromStream(token->acc)),dup_string(""));
		StreamFlush(token->acc);
		goto htnt_gotopt;

	/*-------------------------------------------------
	; STAGE ZETA4[1] - skip space between '=' and value.
	; Also determine if the value is quoted or not.
	;
	; [1]	- local rock radio station in Lower Sheol
	;--------------------------------------------------*/
	
htnt_voptval:	t = up_string(StringFromStream(token->acc));
		StreamFlush(token->acc);

htnt_voptval10:	c = StreamRead(token->input);
		if (c == IEOF)  goto htnt_error;
		if (isspace(c))	goto htnt_voptval10;
		if (c == '>')	goto htnt_error;
		if (c == '\'')	goto htnt_valsq;
		if (c == '"')	goto htnt_valdq;
		
	/*-------------------------------------------------
	; STAGE ETA - read in non-quoted value
	;-------------------------------------------------*/
	
htnt_nqval:	StreamWrite(token->acc,c);
		c = StreamRead(token->input);
		if (c == IEOF)	goto htnt_error;
		if (c == '>')	goto htnt_nqvald;
		if (!isspace(c)) goto htnt_nqval;
		ht_makepair(token,t,StringFromStream(token->acc));
		StreamFlush(token->acc);
		goto htnt_bopts;
		
htnt_nqvald:	ht_makepair(token,t,StringFromStream(token->acc));
		StreamFlush(token->acc);
		goto htnt_done;
		
	/*------------------------------------------------
	; STAGE THETA - read in single quoted value
	;------------------------------------------------*/

htnt_valsq10:	StreamWrite(token->acc,c);
htnt_valsq:	c = StreamRead(token->input);
		if (c == IEOF)	goto htnt_error;
		if (iscntrl(c)) c = ' ';
		if (c != '\'')	goto htnt_valsq10;
		ht_makepair(token,t,StringFromStream(token->acc));
		StreamFlush(token->acc);
		goto htnt_bopts;

	/*------------------------------------------------
	; STAGE IOTA - read in a double quoted value
	;------------------------------------------------*/
	
htnt_valdq10:	StreamWrite(token->acc,c);
htnt_valdq:	c = StreamRead(token->input);
		if (c == IEOF)	goto htnt_error;
		if (iscntrl(c)) c = ' ';
		if (c != '"')	goto htnt_valdq10;
		ht_makepair(token,t,StringFromStream(token->acc));
		StreamFlush(token->acc);
		goto htnt_bopts;
		
	/*-------------------------------------------------
	; STAGE KAPPA - read in a comment (kludgy for now)
	;------------------------------------------------*/
	
htnt_comment:	level = 1;
htnt_comm10:	c = StreamRead(token->input);
		if (c == IEOF)	goto htnt_error;
		if (c != '<')	goto htnt_comm20;
		level++;
		goto htnt_comm30;
htnt_comm20:	if (c != '>')	goto htnt_comm30;
		if (level == 1)	goto htnt_commdone;
		level--;
htnt_comm30:	StreamWrite(token->acc,c);
		goto htnt_comm10;
		
htnt_commdone:	token->value = StringFromStream(token->acc);
		token->token = T_COMMENT;
		token->state = S_STRING;
		StreamFlush(token->acc);
		return(T_COMMENT);
		
	/*--------------------------------------------------
	; STAGE OMEGA - done - return
	;--------------------------------------------------*/
	
htnt_done:	token->token = T_TAG;
		token->state = S_STRING;
		return(T_TAG);
		
	/*-----------------------------------------------
	; Abandon All Hope Ye Who Enter Here
	;-----------------------------------------------*/
	
htnt_error:	token->token = T_EOF;
		token->state = S_EOF;
		D(ddtlog(ddtstream,"","Received EOF in input");)
		return(T_EOF);
		
}
		
/**********************************************************************/

static int ht_nextcom(HtmlToken token)
{
  ddt(token != NULL);
  
  token->state = S_EOF;
  return(T_EOF);
}

/*********************************************************************/

static void ht_makepair(HtmlToken token,char *name,char *value)
{
  struct pair *psp;
  
  ddt(token != NULL);
  ddt(name  != NULL);
  ddt(value != NULL);
  
  psp = PairCreate(name,value);
  ListAddTail(&token->pairs,&psp->node);
}

/********************************************************************/

static void entify_char(char *d,size_t ds,char *s,char e,const char *entity)
{
  size_t se;

  ddt(d      != NULL);
  ddt(ds     >  0);
  ddt(s      != NULL);
  ddt(e      != '\0');
  ddt(entity != NULL);

  se = strlen(entity);

  for ( ; (*s) && (ds > 0) ; )
  {
    if (*s == e)
    {
      if (ds < se)
      {
        *d = '\0';
	return;
      }

      memcpy(d,entity,se);
      d  += se;
      ds -= se;
      s++;
    }
    else
    {
      *d++ = *s++;
      ds--;
    }
  }
  *d = '\0';
}

/***********************************************************************/
