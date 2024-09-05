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

#ifndef I_4007C572_D16E_538C_88BA_7948289D7F06
#define I_4007C572_D16E_538C_88BA_7948289D7F06

#include <stdbool.h>
#include <signal.h>

extern int const cgilib_coresigs[];

extern int  crashreport      (int);
extern void crashreport_args (int,char **,bool);
extern void crashreport_core (void);

#if defined(__linux__) || defined(__APPLE__) || defined(__SunOS) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
  extern void crashreport_coresigs (sigset_t *);
  extern void crashreport_allsigs  (sigset_t *);
#endif

#endif
