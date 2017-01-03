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

/******************************************************************/

static const int m_coresigs[] =
{
  
  /* ANSI C signals */
    
  SIGILL,
  SIGABRT,
  SIGFPE,
  SIGSEGV,
    
  /* POSIX signals */
    
  SIGQUIT,
  SIGBUS,
  SIGSYS,
  SIGTRAP,
  SIGXCPU,
  SIGXFSZ,
    
  /* other signals sometimes defined */
#ifdef SIGIOT
  SIGIOT,
#endif
};
  
# define CORESIGS       (sizeof(m_coresigs) / sizeof(int))

/******************************************************************/
  
void crashreport_coresigs(sigset_t *set)
{
  sigemptyset(set);
  for (size_t i = 0 ; i < CORESIGS ; i++)
    sigaddset(set,m_coresigs[i]);
}
  
/******************************************************************/
  
void crashreport_allsigs(sigset_t *set)
{
  sigfillset(set);
  for (size_t i = 0 ; i < CORESIGS ; i++)
    sigdelset(set,m_coresigs[i]);
}
  
/******************************************************************/
