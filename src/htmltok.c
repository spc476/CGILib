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
#include "errors.h"
#include "util.h"
#include "ddt.h"
#include "htmltok.h"

#define S_TAGIGNORE	-2
#define S_EOF		-1
#define S_STRING	 0
#define S_TAG		 1
#define S_COMMENT	 2

#define IEOF		-1

#define BIGENOUGH	(1024 * 1024)

#if 0
#ifdef DDT
#  define MemAlloc(s)		MemDDTAlloc((s),__FILE__,__LINE__)
#endif
#endif

#ifdef SCREAM
#  define ht_append(c)	   { *m_pbuff++ = (c) ; *m_pbuff = '\0'   ; m_bufsiz++; }
#  define ht_resetbuffer() { m_pbuff = m_buffer ; m_buffer[0] = '\0'; m_bufsiz = 0; }
#endif

/************************************************************************/

static int	 ht_nexteof		(HtmlToken);
static int	 ht_nextstr		(HtmlToken);
static int	 ht_nexttag		(HtmlToken);
static int	 ht_nextcom		(HtmlToken);
static int	 ht_readchar		(Buffer);
static char	*ht_makestring		(void);
static void	 ht_makepair		(HtmlToken,char *,char *);

#ifndef SCREAM
  static void	 ht_append		(int);
  static void	 ht_resetbuffer		(void);
#endif

/*************************************************************************/

static char  	 *m_pbuff;
static size_t 	  m_bufsiz;
static char   	  m_buffer[BIGENOUGH];	/* big enough */

/*************************************************************************/

int (HtmlParseNew)(HtmlToken *phtoken,Buffer buffer)
{
  HtmlToken pht;
  
  ddt(phtoken != NULL);
  ddt(buffer  != NULL);
  
  pht         = MemAlloc(sizeof(struct htmltoken));
  pht->state  = S_STRING;
  pht->buffer = buffer;
  pht->value  = dup_string(""); /* am i perpetuating a hack? */
  ListInit(&pht->pairs);
  *phtoken    = pht;
  return(ERR_OKAY);
}

/***********************************************************************/

int (HtmlParseClone)(HtmlToken *pnew,HtmlToken token)
{
  HtmlToken    pht;
  struct pair *pair;
  struct pair *pairp;
  
  ddt(pnew != NULL);
  ddt(token != NULL);
  
  pht = MemAlloc(sizeof(struct htmltoken));
  pht->token = token->token;
  pht->state = token->state;
  pht->value = dup_string(token->value);
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

  ht_resetbuffer();  
  /* gross hack - need to track this down */
  if(token->value) 
  {
    D(ddtlog(ddtstream,"$","about to free %a",token->value);)
    MemFree(token->value,strlen(token->value)+1);
  }
  token->value = NULL;
  PairListFree(&token->pairs);

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

int (HtmlParseAddPair)(HtmlToken token,struct pair *p)
{
  ddt(token != NULL);
  ddt(p     != NULL);
  
  ListAddTail(&token->pairs,&p->node);
  return(ERR_OKAY);
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

int (HtmlParseFree)(HtmlToken *ptoken)
{
  HtmlToken token;
  
  ddt(ptoken  != NULL);
  ddt(*ptoken != NULL);

  token = *ptoken;
  
  if (token->value != NULL)
  {
    ddt(token->value != NULL);
    MemFree(token->value,strlen(token->value)+1);
  }
  PairListFree(&token->pairs);
  MemFree(token,sizeof(struct htmltoken));
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

  while((c = ht_readchar(token->buffer)) != IEOF)
  {
    if (c == '<')
    {
      st = S_TAG;
      break;
    }
    else
      ht_append(c);
  }

  token->value = ht_makestring();
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
 
htnt_alpha:	c = ht_readchar(token->buffer);
		if (isspace(c)) goto htnt_alpha;
		if (c == IEOF) 	goto htnt_error;
		if (c == '>' ) 	goto htnt_alpha10;	/* done - return tag */
		if (c == '!' ) 	goto htnt_comment;
		goto htnt_gottag;

htnt_alpha10:	token->value = up_string(ht_makestring());
		D(ddtlog(ddtstream,"$","just got %a",token->value);)
		goto htnt_done;

	/*-----------------------------------------------
	; STAGE BETA - read the tag and save
	;----------------------------------------------*/
	
htnt_gottag:	ht_append(c);	
		c = ht_readchar(token->buffer);
		if (c == IEOF) 	goto htnt_error;
		if (c == '=' ) 	goto htnt_error;
		if (c == '>' ) 	goto htnt_alpha10;	/* done - return tag */
		if (!isspace(c)) goto htnt_gottag;

		token->value = up_string(ht_makestring());
		D(ddtlog(ddtstream,"$","parsed %a",token->value);)
		
	/*-------------------------------------------------
	; STAGE GAMMA - skip space to options
	;------------------------------------------------*/
	
htnt_bopts:	c = ht_readchar(token->buffer);
		if (isspace(c))	goto htnt_bopts;
		if (c == '>')	goto htnt_done;
		if (c == '=')	goto htnt_error;
		if (c == IEOF)	goto htnt_error;
		
	/*-----------------------------------------------
	; STAGE DELTA - read in option
	;-----------------------------------------------*/
	
htnt_gotopt:	ht_append(c);
		c = ht_readchar(token->buffer);
		if (c == IEOF)	goto htnt_error;
		if (isspace(c))	goto htnt_boptval;
		if (c == '>')	goto htnt_gotoptd;
		if (c == '=')	goto htnt_voptval;
		goto htnt_gotopt;

htnt_gotoptd:	ht_makepair(token,up_string(ht_makestring()),dup_string(""));
		goto htnt_done;

	/*------------------------------------------------
	; STAGE EPSILON - between an option and possibly its
	; value - or another option
	;-------------------------------------------------*/

htnt_boptval:	c = ht_readchar(token->buffer);
		if (isspace(c))	goto htnt_boptval;
		if (c == IEOF)	goto htnt_error;
		if (c == '>')	goto htnt_gotoptd;
		if (c == '=')	goto htnt_voptval;
		
		ht_makepair(token,up_string(ht_makestring()),dup_string(""));
		goto htnt_gotopt;

	/*-------------------------------------------------
	; STAGE ZETA4[1] - skip space between '=' and value.
	; Also determine if the value is quoted or not.
	;
	; [1]	- local rock radio station in Lower Sheol
	;--------------------------------------------------*/
	
htnt_voptval:	t = up_string(ht_makestring());

htnt_voptval10:	c = ht_readchar(token->buffer);
		if (isspace(c))	goto htnt_voptval10;
		if (c == IEOF)	goto htnt_error;
		if (c == '>')	goto htnt_error;
		if (c == '\'')	goto htnt_valsq;
		if (c == '"')	goto htnt_valdq;
		
	/*-------------------------------------------------
	; STAGE ETA - read in non-quoted value
	;-------------------------------------------------*/
	
htnt_nqval:	ht_append(c);
		c = ht_readchar(token->buffer);
		if (c == IEOF)	goto htnt_error;
		if (c == '>')	goto htnt_nqvald;
		if (!isspace(c)) goto htnt_nqval;
		ht_makepair(token,t,ht_makestring());
		goto htnt_bopts;
		
htnt_nqvald:	ht_makepair(token,t,ht_makestring());
		goto htnt_done;
		
	/*------------------------------------------------
	; STAGE THETA - read in single quoted value
	;------------------------------------------------*/

htnt_valsq10:	ht_append(c);
htnt_valsq:	c = ht_readchar(token->buffer);
		if (c == IEOF)	goto htnt_error;
		if (iscntrl(c)) c = ' ';
		if (c != '\'')	goto htnt_valsq10;
		ht_makepair(token,t,ht_makestring());
		goto htnt_bopts;

	/*------------------------------------------------
	; STAGE IOTA - read in a double quoted value
	;------------------------------------------------*/
	
htnt_valdq10:	ht_append(c);
htnt_valdq:	c = ht_readchar(token->buffer);
		if (c == IEOF)	goto htnt_error;
		if (iscntrl(c)) c = ' ';
		if (c != '"')	goto htnt_valdq10;
		ht_makepair(token,t,ht_makestring());
		goto htnt_bopts;
		
	/*-------------------------------------------------
	; STAGE KAPPA - read in a comment (kludgy for now)
	;------------------------------------------------*/
	
htnt_comment:	level = 1;
htnt_comm10:	c = ht_readchar(token->buffer);
		if (c == IEOF)	goto htnt_error;
		if (c != '<')	goto htnt_comm20;
		level++;
		goto htnt_comm30;
htnt_comm20:	if (c != '>')	goto htnt_comm30;
		if (level == 1)	goto htnt_commdone;
		level--;
htnt_comm30:	ht_append(c);
		goto htnt_comm10;
		
htnt_commdone:	token->value = ht_makestring();
		token->token = T_COMMENT;
		token->state = S_STRING;
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

static int ht_readchar(Buffer buffer)
{
  char   c[2];
  int    rc;
  /*size_t size;*/
  
  ddt(buffer != NULL);

  while(!LineEOF(buffer))
  {
    /*size = 2;*/
    rc   = LineReadC(buffer,c);
    /*rc   = BufferRead(buffer,c,&size);*/
    if (rc != ERR_OKAY)
    {
      if (rc != BUFERR_EOF) ErrorPush(CgiErr,HTMLPARSENEXT,rc,"");
      return(IEOF);
    }
    /*if (size == 0) continue;*/

    /*--------------------------------------------------------
    ; it seems that SOME engines (cough!) send out NUL bytes.
    ; I suppose to be cute or something.  Sigh.
    ;
    ; If we read a NUL byte, return end of file.
    ;----------------------------------------------------*/
    return( c[0] ? c[0] : IEOF);
  }
  return(IEOF);
}

/********************************************************************/

#ifndef SCREAM
  static void ht_append(int c)
  {
    *m_pbuff++ = c;
    *m_pbuff   = '\0';
    m_bufsiz++;
  }
#endif

/********************************************************************/

static char *ht_makestring(void)
{
  char *p;

  p = MemAlloc(m_bufsiz+1);
  memcpy(p,m_buffer,m_bufsiz);
  p[m_bufsiz] = 0;
  ht_resetbuffer();
  return(p);
}

/*********************************************************************/

#ifndef SCREAM
  static void ht_resetbuffer(void)
  {
    m_pbuff     = m_buffer;
    m_buffer[0] = '\0';
    /*memset(m_buffer,0,BIGENOUGH);*/
    m_bufsiz    = 0; 
  }
#endif

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

