/***************************************************************************
*
* Copyright 2018 by Sean Conner.
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

/* See http://antirez.com/news/43 for why this exists */

#ifdef __GNUC__
#  define _GNU_SOURCE
#else
#  define __attribute__(x)
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>

#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __linux__
#  include <execinfo.h>
#endif

#include "../dump.h"
#include "../crashreport.h"

#ifndef SA_ONESHOT
#  define SA_ONESHOT 0
#endif

static int    m_argc;
static char **m_argv;
static char **m_envp;
static int    m_cnt;

/***************************************************************************/

#ifdef __linux__

  /*-----------------------------------------------------------------------
  ; we're executing within the context of a signal handler, and we are
  ; severely restricted in stack space and what library functions we can
  ; call (very few, and for the most part, only system calls).  We we have
  ; to be careful about what functions we call.
  ;-------------------------------------------------------------------------*/
  
  static void crashreport_backtrace(unsigned long pid)
  {
    static char  buffer[BUFSIZ];
    static void *report[128];
    int          fh;
    int          count;
    off_t        bytes;
    char        *s;
    char        *p;
    size_t       len;
    
    count = backtrace(report,128);
    if (count == 0)
    {
      syslog(LOG_ALERT,"CRASH(%lu/%03d): no backtrace available",pid,m_cnt++);
      return;
    }
    
    memcpy(buffer,"/tmp/crash.XXXXXX",17);
    fh = mkstemp(buffer);
    if (fh < 0)
    {
      syslog(LOG_ALERT,"CRASH(%lu/%03d): no backtrace safely available",pid,m_cnt++);
      return;
    }
    unlink(buffer);
    
    /*--------------------------------------------------------------------
    ; We use backtrace_symbols_fd() instead of backtrace_symbols() because
    ; backtrace_symbols() allocates memory, and ... well ... allocating
    ; memory from a signal handler might not be the best idea.  Besides,
    ; open(), read() and close() are safe to call from a signal handler, so,
    ; we do the backtrace_symbols_fd().
    ;--------------------------------------------------------------------*/
    
    backtrace_symbols_fd(report,count,fh);
    bytes = lseek(fh,0,SEEK_CUR);
    lseek(fh,0,SEEK_SET);
    
    syslog(LOG_ALERT,"CRASH(%lu/%03d): STACK TRACE",pid,m_cnt++);
    
    if (read(fh,buffer,bytes) < 0)
    {
      syslog(LOG_ALERT,"CRASH(%lu/%03d): can't generate backtrace",pid,m_cnt++);
      return;
    }
    
    s = buffer;
    while((p = memchr(s,'\n',bytes)) != NULL)
    {
      *p++ = '\0';
      syslog(LOG_ALERT,"CRASH(%lu/%03d):        %s",pid,m_cnt++,s);
      len = (size_t)(p - s);
      bytes -= len;
      s = p;
    }
    
    close(fh);
  }
#endif

/************************************************************************/

#ifdef __linux__

static void crashreport_hexdump(
    unsigned long  pid,
    void const    *data,
    size_t         size,
    size_t         offset
)
{
  unsigned char const *block = data;
  char                 toffs[sizeof(size_t) * 2 + 1];
  char                 tbyte[DEF_DUMP_BYTES * 3 + 1];
  
  syslog(LOG_ALERT,"CRASH(%lu/%03d): STACK DUMP",pid,m_cnt++);
  
  while(size > 0)
  {
    hex(toffs,sizeof(toffs),offset,sizeof(size_t)*2);
    hexdump_mems(tbyte,sizeof(tbyte),block,size,DEF_DUMP_BYTES);
    syslog(LOG_ALERT,"CRASH(%lu/%03d):        %s: %s",pid,m_cnt++,toffs,tbyte);
    
    if (size < DEF_DUMP_BYTES) break;
    
    block  += DEF_DUMP_BYTES;
    size   -= DEF_DUMP_BYTES;
    offset += DEF_DUMP_BYTES;
  }
}
#endif

/***********************************************************************/

static char const *crashreport_code(int const sig,int const code)
{
  static char const unspef[] = "Unspecified/untranslated error";
  
  switch(code)
  {
    case SI_USER:    return "User process via kill() or raise()";
    case SI_QUEUE:   return "Signal sent by the sigqueue()";
    case SI_TIMER:   return "Signal generated by expiration of a timer set by timer_settime()";
    case SI_ASYNCIO: return "Signal generated by completion of an asynchronous I/O request";
    case SI_MESGQ:   return "Signal generated by arrival of a message on an empty message queue";
    default:         break;
  }
  
  switch(sig)
  {
    case SIGBUS:
         switch(code)
         {
           case BUS_ADRALN:    return "Invalid address alignment";
           case BUS_ADRERR:    return "Nonexistent physical address";
           case BUS_OBJERR:    return "Object-specific hardware error";
#ifdef BUS_MCEERR_AO
           case BUS_MCEERR_AO: return "Hardware memory error; action optional";
#endif
#ifdef BUS_MCEERR_AR
           case BUS_MCEERR_AR: return "Hardware memory error; action required";
#endif
           default:            break;
         }
         break;
         
    case SIGFPE:
         switch(code)
         {
           case FPE_INTDIV: return "Integer divide-by-zero";
           case FPE_INTOVF: return "Integer overflow";
           case FPE_FLTDIV: return "Floating-point divide-by-zero";
           case FPE_FLTINV: return "Invalid floating-point operation";
           case FPE_FLTOVF: return "Floating-point overflow";
           case FPE_FLTRES: return "Floating-point inexact result";
           case FPE_FLTUND: return "Floating-point underflow";
           case FPE_FLTSUB: return "Subscript out of range";
           default:         break;
         }
         break;
         
    case SIGILL:
         switch(code)
         {
           case ILL_BADSTK: return "Internal stack error";
           case ILL_COPROC: return "Coprocessor error";
           case ILL_ILLADR: return "Illegal addressing mode";
           case ILL_ILLOPC: return "Illegal opcode";
           case ILL_ILLOPN: return "Illegal operand";
           case ILL_ILLTRP: return "Illegal trap";
           case ILL_PRVOPC: return "Priviledged opcode";
           case ILL_PRVREG: return "Priviledged register";
           default:         break;
         }
         break;
         
#ifdef SIGPOLL
    case SIGPOLL:
         switch(code)
         {
           case POLL_ERR: return "I/O error";
           case POLL_HUP: return "Device disconnected";
           case POLL_IN:  return "Data input available";
           case POLL_OUT: return "Output buffers available";
           case POLL_MSG: return "Input message available";
           case POLL_PRI: return "High priority input available";
           default:       break;
         }
         break;
#endif

    case SIGSEGV:
         switch(code)
         {
           case SEGV_ACCERR: return "Invalid permissions for mapped object";
           case SEGV_MAPERR: return "Address not mapped for object";
           default:          break;
         }
         break;
         
    case SIGTRAP:
         switch(code)
         {
           case TRAP_BRKPT:  return "Process breakpoint";
           case TRAP_TRACE:  return "Process trace trap";
#ifdef TRAP_BRANCH
           case TRAP_BRANCH: return "Process branch trap";
#endif
#ifdef TRAP_HWBKPT
           case TRAP_HWBKPT: return "Hardware breakpoint/watchpoint";
#endif

           default:          break;
         }
         break;
         
    case SIGCHLD:
         switch(code)
         {
           case CLD_EXITED:    return "Child has exited";
           case CLD_KILLED:    return "Child has terminated abnormally and did not create a core file";
           case CLD_DUMPED:    return "Child has terminated abnormally and created a core file";
           case CLD_TRAPPED:   return "Traced child has trapped";
           case CLD_STOPPED:   return "Child has stopped";
           case CLD_CONTINUED: return "Stopped child has continued";
           default:            break;
        }
        break;
        
    default:
         break;
  }
  
  return unspef;
}

/***************************************************************************/

static void crashreport_handler(int sig,siginfo_t *info,void *context __attribute__((unused)))
{
  unsigned long pid = getpid();
  
  syslog(LOG_ALERT,"CRASH(%lu/%03d): pid=%lu signal='%s'",pid,m_cnt++,pid,strsignal(sig));
  syslog(LOG_ALERT,"CRASH(%lu/%03d): reason='%s'",pid,m_cnt++,crashreport_code(info->si_signo,info->si_code));
  
  if (info->si_signo != sig)
    syslog(LOG_ALERT,"CRASH(%lu/%03d): reported sig %d doesn't match passed in signal %d",pid,m_cnt++,info->si_signo,sig);
    
  if (info->si_code == SI_USER)
    syslog(LOG_ALERT,"CRASH(%lu/%03d): sending_process=%lu user=%lu",pid,m_cnt++,(unsigned long)info->si_pid,(unsigned long)info->si_uid);
    
  switch(info->si_signo)
  {
    case SIGBUS:
    case SIGSEGV:
         syslog(LOG_ALERT,"CRASH(%lu/%03d): address=%p",pid,m_cnt++,info->si_addr);
         break;
         
    case SIGILL:
    case SIGFPE:
         syslog(LOG_ALERT,"CRASH(%lu/%03d): pc=%p",pid,m_cnt++,info->si_addr);
         break;
         
    default:
         break;
  }
  
  /*--------------------------------------------------------------------
  ; what follows is system dependent information.  This additional
  ; information should include a register and (maybe) a stack dump to help
  ; locate the source of the crash in case we don't get a core file.
  ;---------------------------------------------------------------------*/
  
#ifdef __linux__
  ucontext_t *cpu = context;
  
#  ifdef __i386__
     syslog(
       LOG_ALERT,
       "CRASH(%lu/%03d): CS=%04X DS=%04X ES=%04X FS=%04X GS=%04X",
       pid,
       m_cnt++,
       cpu->uc_mcontext.gregs[REG_CS],
       cpu->uc_mcontext.gregs[REG_DS],
       cpu->uc_mcontext.gregs[REG_ES],
       cpu->uc_mcontext.gregs[REG_FS],
       cpu->uc_mcontext.gregs[REG_GS]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH(%lu/%03d): EIP=%08X EFL=%08X ESP=%08X EBP=%08X ESI=%08X EDI=%08X",
       pid,
       m_cnt++,
       cpu->uc_mcontext.gregs[REG_EIP],
       cpu->uc_mcontext.gregs[REG_EFL],
       cpu->uc_mcontext.gregs[REG_ESP],
       cpu->uc_mcontext.gregs[REG_EBP],
       cpu->uc_mcontext.gregs[REG_ESI],
       cpu->uc_mcontext.gregs[REG_EDI]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH(%lu/%03d): EAX=%08X EBX=%08X ECX=%08X EDX=%08X",
       pid,
       m_cnt++,
       cpu->uc_mcontext.gregs[REG_EAX],
       cpu->uc_mcontext.gregs[REG_EBX],
       cpu->uc_mcontext.gregs[REG_ECX],
       cpu->uc_mcontext.gregs[REG_EDX]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH(%lu/%03d): UESP=%08X TRAPNO=%08X ERR=%08X",
       pid,
       m_cnt++,
       cpu->uc_mcontext.gregs[REG_UESP],
       cpu->uc_mcontext.gregs[REG_TRAPNO],
       cpu->uc_mcontext.gregs[REG_ERR]
    );
    
    crashreport_hexdump(
        pid,
        (void *)cpu->uc_mcontext.gregs[REG_ESP],
        256,
        (size_t)cpu->uc_mcontext.gregs[REG_ESP]
    );
    
#  endif

#  ifdef __x86_64
     syslog(
       LOG_ALERT,
       "CRASH(%lu/%03d): RIP=%016llX EFL=%016llX RSP=%016llX RBP=%016llX RSI=%016llX RDI=%016llX",
       pid,
       m_cnt++,
       cpu->uc_mcontext.gregs[REG_RIP],
       cpu->uc_mcontext.gregs[REG_EFL],
       cpu->uc_mcontext.gregs[REG_RSP],
       cpu->uc_mcontext.gregs[REG_RBP],
       cpu->uc_mcontext.gregs[REG_RSI],
       cpu->uc_mcontext.gregs[REG_RDI]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH(%lu/%03d): RAX=%016llX RBX=%016llX RCX=%016llX RDX=%016llX",
       pid,
       m_cnt++,
       cpu->uc_mcontext.gregs[REG_RAX],
       cpu->uc_mcontext.gregs[REG_RBX],
       cpu->uc_mcontext.gregs[REG_RCX],
       cpu->uc_mcontext.gregs[REG_RDX]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH(%lu/%03d):  R8=%016llX  R9=%016llX R10=%016llX R11=%016llX",
       pid,
       m_cnt++,
       cpu->uc_mcontext.gregs[REG_R8],
       cpu->uc_mcontext.gregs[REG_R9],
       cpu->uc_mcontext.gregs[REG_R10],
       cpu->uc_mcontext.gregs[REG_R11]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH(%lu/%03d): R12=%016llX R13=%016llX R14=%016llX R15=%016llX",
       pid,
       m_cnt++,
       cpu->uc_mcontext.gregs[REG_R12],
       cpu->uc_mcontext.gregs[REG_R13],
       cpu->uc_mcontext.gregs[REG_R14],
       cpu->uc_mcontext.gregs[REG_R15]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH(%lu/%03d): TRAPNO=%016llX ERR=%016llX OLDMASK=%016llX CR2=%016llX",
       pid,
       m_cnt++,
       cpu->uc_mcontext.gregs[REG_TRAPNO],
       cpu->uc_mcontext.gregs[REG_ERR],
       cpu->uc_mcontext.gregs[REG_OLDMASK],
       cpu->uc_mcontext.gregs[REG_CR2]
     );
     
     crashreport_hexdump(
        pid,
        (void *)cpu->uc_mcontext.gregs[REG_RSP],
        256,
        (size_t)cpu->uc_mcontext.gregs[REG_RSP]
     );
     
#  endif
  crashreport_backtrace(pid);
#endif

#ifdef __SunOS
#endif

  /*-----------------------------------------------------------
  ; report any additional information, if it exists
  ;-------------------------------------------------------------*/
  
  if (m_argv != NULL)
  {
    syslog(LOG_ALERT,"CRASH(%lu/%03d): COMMAND LINE",pid,m_cnt++);
    for (int i = 0 ; i < m_argc ; i++)
      syslog(LOG_ALERT,"CRASH(%lu/%03d):        %s",pid,m_cnt++,m_argv[i]);
  }
  
  if (m_envp != NULL)
  {
    syslog(LOG_ALERT,"CRASH(%lu/%03d): ENVIRONMENT",pid,m_cnt++);
    for (int i = 0 ; m_envp[i] != NULL ; i++)
      syslog(LOG_ALERT,"CRASH(%lu/%03d):        %s",pid,m_cnt++,m_envp[i]);
  }
  
  syslog(LOG_ALERT,"CRASH(%lu/%03d): DONE",pid,m_cnt++);
  
  /*------------------------------------------------------------------------
  ; The handler is a one-shot deal.  We did this because we can now re-raise
  ; the signal to set the parent process know how we died and possibly
  ; generate a core file.
  ;-------------------------------------------------------------------------*/
  
  raise(sig);
}

/************************************************************************/

int crashreport(int sig)
{
  struct sigaction sa;
  
  sa.sa_sigaction = crashreport_handler;
  sa.sa_flags     = SA_SIGINFO | SA_ONESHOT;
  sigemptyset(&sa.sa_mask);
  
  return sigaction(sig,&sa,NULL);
}

/********************************************************************/

void crashreport_args(int argc,char **argv,bool env)
{
  m_argc = argc;
  m_argv = argv;
  if (env)
    m_envp = environ;
}

/********************************************************************/
