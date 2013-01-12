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

/***************************************************************************/

#ifdef __linux__

  /*-----------------------------------------------------------------------
  ; we're executing within the context of a signal handler, and we are
  ; severely restricted in stack space and what library functions we can
  ; call (very few, and for the most part, only system calls).  We we have
  ; to be careful about what functions we call.
  ;-------------------------------------------------------------------------*/
  
  static void crashreport_backtrace(void)
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
      syslog(LOG_ALERT,"CRASH: no backtrace available");
      return;
    }
    
    memcpy(buffer,"/tmp/crash.XXXXXX",17);
    fh = mkstemp(buffer);    
    if (fh < 0)
    {
      syslog(LOG_ALERT,"CRASH: no backtrace safely available");
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
    
    syslog(LOG_ALERT,"CRASH: STACK TRACE");
    
    if (read(fh,buffer,bytes) < 0)
    {
      syslog(LOG_ALERT,"CRASH: can't generate backtrace");
      return;
    }
    
    s = buffer;
    while((p = memchr(s,'\n',bytes)) != NULL)
    {
      *p++ = '\0';
      syslog(LOG_ALERT,"CRASH:        %s",s);
      len = (size_t)(p - s);
      bytes -= len;
      s = p;
    }
    
    close(fh);
  }
#endif  

/************************************************************************/

#define LINESIZE	16

static void crashreport_hexdump(void *data,size_t size,size_t offset)
{
  const unsigned char *block = data;
  char                *p;
  char                 line[80];
  char                 ascii[LINESIZE + 1];
  int                  skip;
  int                  j;
  
  syslog(LOG_ALERT,"CRASH: STACK DUMP");
  
  while(size > 0)
  {
    p = line;
    p += sprintf(p,"%08lX: ",(unsigned long)offset);

    for (skip = offset % LINESIZE , j = 0 ; skip ; j++ , skip--)
    {
      p += sprintf(p,"   ");
      ascii[j] = ' ';
    }
    
    do
    {
      p += sprintf(p,"%02X ",*block);
      if (isprint(*block))
        ascii[j] = *block;
      else
        ascii[j] = '.';
      
      block++;
      offset++;
      j++;
      size--;
    } while ((j < LINESIZE) && (size > 0));
    
    ascii[j] = '\0';
    
    if (j < LINESIZE)
    {
      int i;
      
      for (i = j ; i < LINESIZE ; i++)
        p += sprintf(p,"   ");
    }
    syslog(LOG_ALERT,"CRASH: %s",line);
  } 
}

/***********************************************************************/

static const char *crashreport_code(const int sig,const int code)
{
  static const char unspef[] = "Unspecified/untranslated error";
  
  if (code == SI_USER)
    return "User process via kill() or raise()";
  
  switch(sig)
  {
    case SIGBUS:
         switch(code)
         {
           case BUS_ADRALN:	return "Invalid address alignment";
           case BUS_ADRERR:	return "Nonexistent physical address";
           case BUS_OBJERR:	return "Object-specific hardware error";
#ifdef BUS_MCEERR_AO
           case BUS_MCEERR_AO:	return "Hardware memory error; action optional";
#endif
#ifdef BUS_MCEERR_AR
           case BUS_MCEERR_AR:	return "Hardware memory error; action required";
#endif
           default: return unspef;
         }
         break;
         
    case SIGFPE:
         switch(code)
         {
           case FPE_FLTDIV:	return "Floating-point divide-by-zero";
           case FPE_FLTINV:	return "Invalid floating-point operation";
           case FPE_FLTOVF:	return "Floating-point overflow";
           case FPE_FLTRES:	return "Floating-point inexact result";
           case FPE_FLTUND:	return "Floating-point underflow";
           case FPE_INTDIV:	return "Integer divide-by-zero";
           case FPE_INTOVF:	return "Integer overflow";
#ifdef FPE_SUB
           case FPE_SUB:	return "Subscript out of range";
#endif
           default: return unspef;
         }
         break;
    
    case SIGILL:
         switch(code)
         {
           case ILL_BADSTK:	return "Internal stack error";
           case ILL_COPROC:	return "Coprocessor error";
           case ILL_ILLADR:	return "Illegal addressing mode";
           case ILL_ILLOPC:	return "Illegal opcode";
           case ILL_ILLOPN:	return "Illegal operand";
           case ILL_ILLTRP:	return "Illegal trap";
           case ILL_PRVOPC:	return "Priviledged opcode";
           case ILL_PRVREG:	return "Priviledged register";
           default: return unspef;
         }
         break;

    case SIGPOLL:
         switch(code)
         {
           case POLL_ERR:	return "I/O error";
           case POLL_HUP:	return "Device disconnected";
         }
         break;
         
    case SIGSEGV:
         switch(code)
         {
           case SEGV_ACCERR:	return "Invalid permissions for mapped object";
           case SEGV_MAPERR:	return "Address not mapped for object";
         }
         break;
         
    case SIGTRAP:
         switch(code)
         {
#ifdef TRAP_BRANCH
           case TRAP_BRANCH:	return "Process branch trap";
#endif
           case TRAP_BRKPT:	return "Process breakpoint";
#ifdef TRAP_HWBKPT
           case TRAP_HWBKPT:	return "Hardware breakpoint/watchpoint";
#endif
           case TRAP_TRACE:	return "Process trace trap";
         }
         break;
         
    default: return unspef;
  }
  
  return unspef;
}
    
/***************************************************************************/

static void crashreport_handler(int sig,siginfo_t *info,void *context __attribute__((unused)))
{
  syslog(LOG_ALERT,"CRASH: pid=%lu signal='%s'",(unsigned long)getpid(),strsignal(sig));
  syslog(LOG_ALERT,"CRASH: reason='%s'",crashreport_code(info->si_signo,info->si_code));
  
  if (info->si_signo != sig)
    syslog(LOG_ALERT,"CRASH: reported sig %d doesn't match passed in signal %d",info->si_signo,sig);
  
  if (info->si_code == SI_USER)
    syslog(LOG_ALERT,"CRASH: sending_process=%lu user=%lu",(unsigned long)info->si_pid,(unsigned long)info->si_uid);

  switch(info->si_signo)
  {
    case SIGBUS:
    case SIGSEGV:
         syslog(LOG_ALERT,"CRASH: address=%p",info->si_addr);
         break;
         
    case SIGILL:
    case SIGFPE:
         syslog(LOG_ALERT,"CRASH: pc=%p",info->si_addr);
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
       "CRASH: CS=%04X DS=%04X ES=%04X FS=%04X GS=%04X",
       cpu->uc_mcontext.gregs[REG_CS],
       cpu->uc_mcontext.gregs[REG_DS],
       cpu->uc_mcontext.gregs[REG_ES],
       cpu->uc_mcontext.gregs[REG_FS],
       cpu->uc_mcontext.gregs[REG_GS]
     );
    
     syslog(
       LOG_ALERT,
       "CRASH: EIP=%08X EFL=%08X ESP=%08X EBP=%08X ESI=%08X EDI=%08X",
       cpu->uc_mcontext.gregs[REG_EIP],
       cpu->uc_mcontext.gregs[REG_EFL],
       cpu->uc_mcontext.gregs[REG_ESP],
       cpu->uc_mcontext.gregs[REG_EBP],
       cpu->uc_mcontext.gregs[REG_ESI],
       cpu->uc_mcontext.gregs[REG_EDI]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH: EAX=%08X EBX=%08X ECX=%08X EDX=%08X",
       cpu->uc_mcontext.gregs[REG_EAX],
       cpu->uc_mcontext.gregs[REG_EBX],
       cpu->uc_mcontext.gregs[REG_ECX],
       cpu->uc_mcontext.gregs[REG_EDX]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH: UESP=%08X TRAPNO=%08X ERR=%08X",
       cpu->uc_mcontext.gregs[REG_UESP],
       cpu->uc_mcontext.gregs[REG_TRAPNO],
       cpu->uc_mcontext.gregs[REG_ERR]
    );

    crashreport_hexdump(
    	(void *)cpu->uc_mcontext.gregs[REG_ESP],
    	256,
    	(size_t)cpu->uc_mcontext.gregs[REG_ESP]
    );
    	
#  endif

#  ifdef __x86_64
     syslog(
       LOG_ALERT,
       "CRASH: RIP=%016lX EFL=%016lX RSP=%016lX RBP=%016lX RSI=%016lX RDI=%016lX",
       cpu->uc_mcontext.gregs[REG_RIP],
       cpu->uc_mcontext.gregs[REG_EFL],
       cpu->uc_mcontext.gregs[REG_RSP],
       cpu->uc_mcontext.gregs[REG_RBP],
       cpu->uc_mcontext.gregs[REG_RSI],
       cpu->uc_mcontext.gregs[REG_RDI]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH: RAX=%016lX RBX=%016lX RCX=%016lX RDX=%016lX",
       cpu->uc_mcontext.gregs[REG_RAX],
       cpu->uc_mcontext.gregs[REG_RBX],
       cpu->uc_mcontext.gregs[REG_RCX],
       cpu->uc_mcontext.gregs[REG_RDX]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH:  R8=%016lX  R9=%016lX R10=%016lX R11=%016lX",
       cpu->uc_mcontext.gregs[REG_R8],
       cpu->uc_mcontext.gregs[REG_R9],
       cpu->uc_mcontext.gregs[REG_R10],
       cpu->uc_mcontext.gregs[REG_R11]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH: R12=%016lX R13=%016lX R14=%016lX R15=%016lX",
       cpu->uc_mcontext.gregs[REG_R12],
       cpu->uc_mcontext.gregs[REG_R13],
       cpu->uc_mcontext.gregs[REG_R14],
       cpu->uc_mcontext.gregs[REG_R15]
     );
     
     syslog(
       LOG_ALERT,
       "CRASH: TRAPNO=%016lX ERR=%016lX OLDMASK=%016lX CR2=%016lX",
       cpu->uc_mcontext.gregs[REG_TRAPNO],
       cpu->uc_mcontext.gregs[REG_ERR],
       cpu->uc_mcontext.gregs[REG_OLDMASK],
       cpu->uc_mcontext.gregs[REG_CR2]
     );
     
     crashreport_hexdump(
     	(void *)cpu->uc_mcontext.gregs[REG_RSP],
     	256,
     	(size_t)cpu->uc_mcontext.gregs[REG_RSP]
     );
     
#  endif
  crashreport_backtrace();
#endif

#ifdef __SunOS
#endif

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
  stack_t          sigstack;
  
  /*-----------------------------------------------------------------------
  ; We use a separate stack for handing the signal, just in case there's an
  ; issue with running off past the use stack.  We make the signal handler
  ; one-shot, so that the default action is restored after we handle the
  ; signal (see comment in crashreport_handler()).  We first check to see if
  ; an alternative stack has been install, and if not, set one up.
  ;------------------------------------------------------------------------*/
  
  if (sigaltstack(NULL,&sigstack) < 0)
    return errno;
  
  if (sigstack.ss_sp == NULL)
  {
    sigstack.ss_sp = malloc(SIGSTKSZ);
    if (sigstack.ss_sp == NULL)
      return errno;
  
    sigstack.ss_size  = SIGSTKSZ;
    sigstack.ss_flags = 0;
    
    if (sigaltstack(&sigstack,NULL) < 0)
    {
      int err = errno;
      free(sigstack.ss_sp);
      return err;
    }
  }
  
  sa.sa_sigaction = crashreport_handler;
  sa.sa_flags     = SA_ONSTACK | SA_SIGINFO | SA_ONESHOT;
  sigemptyset(&sa.sa_mask);

  return sigaction(sig,&sa,NULL);
}
