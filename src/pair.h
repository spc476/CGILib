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

#ifndef PAIR_H_INCLUDE
#define PAIR_H_INCLUDE
#  include <stddef.h>
#  include "types.h"
#  include "nodelist.h"
#  include "errors.h"
#  if 0
#    include "stream.h"
#  endif
#endif

/**********************************************************************/

#ifndef PAIR_H_DEFINE
#define PAIR_H_DEFINE

#  define PAIRNEW		(ERR_PAIR + 0)
#  define PAIRCREATE		(ERR_PAIR + 1)
#  define PAIRFREE		(ERR_PAIR + 2)
#  define PAIRLISTADD		(ERR_PAIR + 3)
#  define PAIRLISTFIRST		(ERR_PAIR + 4)
#  define PAIRLISTGETPAIR	(ERR_PAIR + 5)
#  define PAIRLISTGETVALUE	(ERR_PAIR + 6)
#  define PAIRLISTFREE		(ERR_PAIR + 7)

#endif

/**********************************************************************/

#ifndef PAIR_H_TYPES
#define PAIR_H_TYPES

  struct pair
  {
    Node    node;
    char   *name;
    char   *value;
    void   *user;
    char   *oname;
    char   *ovalue;
    size_t  sname;
    size_t  svalue;
  };

#endif

/**********************************************************************/

#ifndef PAIR_H_API
#define PAIR_H_API

  struct pair	*(PairNew)		(char **,char,char);
  struct pair	*(PairCreate)		(const char *,const char *);
  struct pair	*(PairClone)		(struct pair *);
  void		 (PairFree)		(struct pair *);

  void		 (PairListAdd)		(List *,char **,char,char);
  struct pair	*(PairListFirst)	(List *);
  struct pair	*(PairListGetPair)	(List *,const char *);
  char		*(PairListGetValue)	(List *,const char *);
  void		 (PairListFree)		(List *);

#endif

/***********************************************************************/

