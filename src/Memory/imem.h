
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
***********************************************************************/

#ifndef IMEM_H
#define IMEM_H

#include <stddef.h>

#ifdef MIPS
#  define PADDING	0xB1
#  define NULLP		0xB1B1B1B1
#elif defined __i386__
#  define PADDING	0xCC
#  define NULLP		0xCCCCCCCC
#elif defined MC68K
#  define PADDING	0xA1
#  define NULLP		0xA1A1A1A1
#elif defined RS6000
#  define PADDING	0xB1
#  define NULLP		0xB1B1B1B1
#else
#  error Please define system
#endif

#ifdef DDT
#  define FUDGE		64
#  define HFUDGE	(FUDGE/2)
#  define NULLPTR(p)	((void *)(p)) == ((void *)NULLP)
#else
#  define FUDGE		0
#  define HFUDGE	0
#  define NULLPTR(p)	(p) == NULL
#endif

#endif

