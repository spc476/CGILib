
/**************************************
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
**************************************/

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

#ifndef FALSE
#  define FALSE	0
#endif
#ifndef TRUE
#  define TRUE	!FALSE
#endif

#ifndef SIZET_MAX
#  define SIZET_MAX	~((size_t)0)
#endif

typedef unsigned char   Byte;
typedef unsigned long	Size;
typedef unsigned long	Flags;
typedef unsigned long   Type;
typedef int             Tag;
typedef void		*Ptr;
typedef int		(*Fptr)();	/* warning on this */

union iall
{
  signed char         c;
  unsigned char       uc;
  signed short int    s;
  unsigned short int  us;
  signed int          i;
  unsigned int        ui;
  signed long int     l;
  unsigned long int   ul;
  void               *p;
};

union all
{
  signed char         c;
  unsigned char       uc;
  signed short int    s;
  unsigned short int  us;
  signed int          i;
  unsigned int        ui;
  signed long int     l;
  unsigned long int   ul;
  float               f;
  double              d;
  void               *p;
};

struct dstring
{
  const char *s1;
  const char *s2;
};

#endif

