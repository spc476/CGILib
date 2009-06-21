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

#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

char		*up_string		(char *);
char		*down_string		(char *);
bool		 empty_string		(const char *);
char		*remove_char		(char *,int (*)(int));
char		*trim_lspace		(char *);
char		*trim_tspace		(char *);
int		 ctohex			(char);
char		 hextoc			(int);
void		 dump_memory		(FILE *,const void *,size_t,size_t);

/****************************************************************/

static inline bool emptynull_string(const char *s)
{
  return ((s == NULL) || empty_string(s));
}

/*--------------------------------------------------------------*/

static inline char *remove_ctrl(char *s)
{
  assert(s != NULL);
  return remove_char(s,(iscntrl));
}

/*--------------------------------------------------------------*/

static inline char *trim_space(char *s)
{
  assert(s != NULL);
  return trim_tspace(trim_lspace(s));
}

/*--------------------------------------------------------------*/

#endif

