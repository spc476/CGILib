/***************************************************************************
*
* Copyright 2009,2013 by Sean Conner.
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

#ifndef RFC822_H
#define RFC822_H

#include "nodelist.h"

char		*(RFC822LineRead)	(FILE *);
void		 (RFC822HeadersRead)	(FILE *,const List *);
size_t		 (RFC822HeadersWrite)	(FILE *,const List *);
size_t		 (RFC822HeaderWrite)	(FILE *,const char *,const char *);

#endif
