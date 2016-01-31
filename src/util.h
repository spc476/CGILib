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

#ifndef I_E9E199CD_82EF_511B_941A_FD918E03F4A1
#define I_E9E199CD_82EF_511B_941A_FD918E03F4A1

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

extern char		*up_string		(char *);
extern char		*down_string		(char *);
extern bool		 empty_string		(const char *);
extern char		*remove_char		(char *,int (*)(int));
extern char		*trim_lspace		(char *);
extern char		*trim_tspace		(char *);
extern int		 ctohex			(char);
extern char		 hextoc			(int);
extern void		 dump_memory		(FILE *,const void *,size_t,size_t);

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

