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

#ifdef __GNUC__
#  define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "../nodelist.h"
#include "../htmltok.h"
#include "../util.h"

/************************************************************************/

static inline HToken ht_nexteof(HtmlToken token)
{
  assert(token != NULL);
  
  token->token = T_EOF;
  return T_EOF;
}

/**********************************************************************/

static inline HToken ht_nextcom(HtmlToken token)
{
  assert(token != NULL);
  
  token->state = S_EOF;
  return T_EOF;
}

/*********************************************************************/

static inline void ht_makepair(HtmlToken token,char *restrict name,char *restrict value)
{
  struct pair *psp;
  
  assert(token != NULL);
  assert(name  != NULL);
  assert(value != NULL);
  
  psp = PairCreate(name,value);
  ListAddTail(&token->pairs,&psp->node);
}

/******************************************************************/

static void ht_acc(HtmlToken token,int c)
{
  assert(token != NULL);
  assert(c     != EOF);
  
  if (token->idx == token->max)
  {
    token->max += 64;
    token->data = realloc(token->data,token->max);
  }
  
  token->data[token->idx++] = c;
}

/*********************************************************************/

static char *ht_accdup(HtmlToken token)
{
  char *text;
  
  assert(token != NULL);
  
  text = malloc(token->idx + 1);
  memcpy(text,token->data,token->idx);
  text[token->idx] = '\0';
  token->idx = 0;
  
  return text;
}

/**********************************************************************/

static HToken ht_nextstr(HtmlToken token)
{
  int st = S_EOF;
  
  assert(token        != NULL);
  assert(token->input != NULL);
  
  while(!feof(token->input))
  {
    int c = fgetc(token->input);
    if (c == EOF) break;
    if (c == '<')
    {
      st = S_TAG;
      break;
    }
    else
      ht_acc(token,c);
  }
  
  token->value = ht_accdup(token);
  token->token = T_STRING;
  token->state = st;
  return T_STRING;
}

/********************************************************************/

static HToken ht_nexttag(HtmlToken token)
{
  int   c;
  char *t;
  char *tt;
  int   level;
  
  assert(token != NULL);
  
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
        
htnt_alpha:     c = fgetc(token->input);
                if (c == EOF)   goto htnt_error;
                if (isspace(c)) goto htnt_alpha;
                if (c == '>' )  goto htnt_alpha10;      /* done - return tag */
                if (c == '!' )  goto htnt_comment;
                goto htnt_gottag;
                
htnt_alpha10:   token->value = up_string(ht_accdup(token));
                goto htnt_done;
                
        /*-----------------------------------------------
        ; STAGE BETA - read the tag and save
        ;----------------------------------------------*/
        
htnt_gottag:    ht_acc(token,c);
                c = fgetc(token->input);
                if (c == EOF)   goto htnt_error;
                if (c == '=' )  goto htnt_error;
                if (c == '>' )  goto htnt_alpha10;      /* done - return tag */
                if (!isspace(c)) goto htnt_gottag;
                
                token->value = up_string(ht_accdup(token));
                
        /*-------------------------------------------------
        ; STAGE GAMMA - skip space to options
        ;------------------------------------------------*/
        
htnt_bopts:     c = fgetc(token->input);
                if (c == EOF)   goto htnt_error;
                if (isspace(c)) goto htnt_bopts;
                if (c == '>')   goto htnt_done;
                if (c == '=')   goto htnt_error;
                
        /*-----------------------------------------------
        ; STAGE DELTA - read in option
        ;-----------------------------------------------*/
        
htnt_gotopt:    ht_acc(token,c);
                c = fgetc(token->input);
                if (c == EOF)   goto htnt_error;
                if (isspace(c)) goto htnt_boptval;
                if (c == '>')   goto htnt_gotoptd;
                if (c == '=')   goto htnt_voptval;
                goto htnt_gotopt;
                
htnt_gotoptd:   t = up_string(ht_accdup(token));
                ht_makepair(token,t,strdup(""));
                free(t);
                goto htnt_done;
                
        /*------------------------------------------------
        ; STAGE EPSILON - between an option and possibly its
        ; value - or another option
        ;-------------------------------------------------*/
        
htnt_boptval:   c = fgetc(token->input);
                if (c == EOF)   goto htnt_error;
                if (isspace(c)) goto htnt_boptval;
                if (c == '>')   goto htnt_gotoptd;
                if (c == '=')   goto htnt_voptval;
                
                t = up_string(ht_accdup(token));
                ht_makepair(token,t,strdup(""));
                free(t); /* XXX - clang found possible memory leak here */
                goto htnt_gotopt;
                
        /*-------------------------------------------------
        ; STAGE ZETA4[1] - skip space between '=' and value.
        ; Also determine if the value is quoted or not.
        ;
        ; [1]   - local rock radio station in Lower Sheol
        ;--------------------------------------------------*/
        
htnt_voptval:   t = up_string(ht_accdup(token));

htnt_voptval10: c = fgetc(token->input);
                if (c == EOF)   goto htnt_error;
                if (isspace(c)) goto htnt_voptval10;
                if (c == '>')   goto htnt_error;
                if (c == '\'')  goto htnt_valsq;
                if (c == '"')   goto htnt_valdq;
                
        /*-------------------------------------------------
        ; STAGE ETA - read in non-quoted value
        ;-------------------------------------------------*/
        
htnt_nqval:     ht_acc(token,c);
                c = fgetc(token->input);
                if (c == EOF)   goto htnt_error;
                if (c == '>')   goto htnt_nqvald;
                if (!isspace(c)) goto htnt_nqval;
                tt = ht_accdup(token);
                ht_makepair(token,t,tt);
                free(tt);
                free(t);
                goto htnt_bopts;
                
htnt_nqvald:    ht_makepair(token,t,ht_accdup(token));
                free(t); /* XXX clang found possible memory leak here */
                goto htnt_done;
                
        /*------------------------------------------------
        ; STAGE THETA - read in single quoted value
        ;------------------------------------------------*/
        
htnt_valsq10:   ht_acc(token,c);
htnt_valsq:     c = fgetc(token->input);
                if (c == EOF)   goto htnt_error;
                if (iscntrl(c)) c = ' ';
                if (c != '\'')  goto htnt_valsq10;
                tt = ht_accdup(token);
                ht_makepair(token,t,tt);
                free(tt);
                free(t);
                goto htnt_bopts;
                
        /*------------------------------------------------
        ; STAGE IOTA - read in a double quoted value
        ;------------------------------------------------*/
        
htnt_valdq10:   ht_acc(token,c);
htnt_valdq:     c = fgetc(token->input);
                if (c == EOF)   goto htnt_error;
                if (iscntrl(c)) c = ' ';
                if (c != '"')   goto htnt_valdq10;
                tt = ht_accdup(token);
                ht_makepair(token,t,tt);
                free(tt);
                free(t);
                goto htnt_bopts;
                
        /*-------------------------------------------------
        ; STAGE KAPPA - read in a comment (kludgy for now)
        ;------------------------------------------------*/
        
htnt_comment:   level = 1;
htnt_comm10:    c = fgetc(token->input);
                if (c == EOF)   goto htnt_error;
                if (c != '<')   goto htnt_comm20;
                level++;
                goto htnt_comm30;
htnt_comm20:    if (c != '>')   goto htnt_comm30;
                if (level == 1) goto htnt_commdone;
                level--;
htnt_comm30:    ht_acc(token,c);
                goto htnt_comm10;
                
htnt_commdone:  token->value = ht_accdup(token);
                token->token = T_COMMENT;
                token->state = S_STRING;
                return T_COMMENT;
                
        /*--------------------------------------------------
        ; STAGE OMEGA - done - return
        ;--------------------------------------------------*/
        
htnt_done:      token->token = T_TAG;
                token->state = S_STRING;
                return T_TAG;
                
        /*-----------------------------------------------
        ; Abandon All Hope Ye Who Enter Here
        ;-----------------------------------------------*/
        
htnt_error:     token->token = T_EOF;
                token->state = S_EOF;
                return T_EOF;
}

/**********************************************************************/

int HtmlParseNext(HtmlToken token)
{
  assert(token != NULL);
  
  if(token->value)
    free(token->value);
    
  token->value = NULL;
  PairListFree(&token->pairs);
  token->idx = 0;
  
  switch(token->state)
  {
    case T_EOF:         return(ht_nexteof(token));
    case T_STRING:      return(ht_nextstr(token));
    case T_TAG:         return(ht_nexttag(token));
    case T_COMMENT:     return(ht_nextcom(token));
    default:
         assert(0);     /* this shouldn't happen */
  }
  assert(0);            /* and this shouldn't happen either */
  return(T_EOF);        /* shut up -Wall -pedantic -ansi options to gcc */
}

/********************************************************************/
