/***************************************************************************
*
* Copyright 2013 by Sean Conner.
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

#ifndef CRASHREPORT_H
#define CRASHREPORT_H

#ifdef __GNUC__
#  define _GNU_SOURCE
#endif

#include <signal.h>

extern int	crashreport		(int);
extern void	crashreport_with	(int,char **,char **);

#ifdef _POSIX_VERSION
  extern void	crashreport_coresigs	(sigset_t *);
  extern void	crashreport_allsigs	(sigset_t *);
#endif

#endif
