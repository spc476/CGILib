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

#ifdef __GNUC__
#  define _GNU_SOURCE
#endif

#include <signal.h>

int const cgilib_coresigs[] =
{
  /* ANSI C signals */
  
  SIGABRT,
  SIGFPE,
  SIGILL,
  SIGSEGV,
  
  /* POSIX signals */

#ifdef _POSIX_VERSION
  SIGBUS,
  SIGQUIT,
  SIGSYS,
  SIGTRAP,
  SIGXCPU,
  SIGXFSZ,
#endif
};
