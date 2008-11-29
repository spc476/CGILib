/****************************************
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
****************************************/

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>
#include "rawfmt.h"

#define MAXCONV		26

/*********************************************************************/

#ifndef CGILIB
#  ifdef DDT
#    define D(x)	x
#  else
#    define D(x)
#    define NDEBUG
#  endif
#  include <assert.h>
#  define ddt	 assert

#  ifndef FALSE
#    define FALSE	0
#  endif
#  ifndef TRUE
#    define TRUE	!FALSE
#  endif

   union all
   {
     unsigned char  c;
     long	    l;
     unsigned long  ul;
     float	    f;
     double	    d;
     void	   *p;
   };
#else
#  include "types.h"
#  include "ddt.h"
#endif

/********************************************************************/

struct tmpstr
{
  size_t  size;
  char	 *text;
};

struct conversion
{
  int	      width;
  int	      min;
  char	      type;
  char	      align;
  char	      pad;
  union all   datum;
  int	      base;
  void	    (*conv)(struct conversion *,struct tmpstr *);
};

/*******************************************************************/

static int	 rdf_getconv	(struct conversion *,int,const char *,va_list);
static void	 rdf_c_char	(struct conversion *,struct tmpstr *);
static void	 rdf_c_signed	(struct conversion *,struct tmpstr *);
static void	 rdf_c_unsigned (struct conversion *,struct tmpstr *);
static void	 rdf_c_double	(struct conversion *,struct tmpstr *);
static void	 rdf_c_pointer	(struct conversion *,struct tmpstr *);
static void	 rdf_c_padding	(struct conversion *,struct tmpstr *);
static void	 rdf_c_string	(struct conversion *,struct tmpstr *);

/******************************************************************/

size_t RawDoFmt(const char *fmt,const char *text,RDF funct,void *datum, ... )
{
  va_list list;
  size_t  size;

  va_start(list,datum);
  size = RawDoFmtv(fmt,text,funct,datum,list);
  va_end(list);
  return(size);
}

/*******************************************************************/

size_t RawDoFmtv(const char *fmt,const char *text,RDF funct,void *datum,va_list list)
{
  struct conversion rules[MAXCONV];
  struct tmpstr     tstr;
  int		    nrules;
  int               ruleset;
  char		    buffer[66];
  size_t	    output;
  size_t	    padsize;
  size_t	    lpad;
  size_t	    rpad;
  size_t            width;
  size_t	    i;
  char		    align;
  char              c;
  char		    pad;

  memset(rules,0,sizeof(rules));
  memset(buffer,0,sizeof(buffer));

  nrules = rdf_getconv(rules,MAXCONV,fmt,list);
  output = 0;

  while(*text)
  {
    c = *text++;
    if (c != '%')
    {
      (*funct)(c,datum);
      output++;
    }
    else
    {
      if (*text == '(') for ( ; *text++ != ')' ; )
	;
      if (*text == '%')
      {
	(*funct)(*text++,datum);
	output++;
	continue;
      }

      ddt(isalpha(*text));

      ruleset = toupper(*text++) - 'A'; /* HACK - works for ASCII only */
      if (ruleset > nrules-1)
      {
	ddt(0);
	continue;
      }

      pad	= rules[ruleset].pad;
      align	= rules[ruleset].align;
      width     = rules[ruleset].width;
      tstr.size = sizeof(buffer);
      tstr.text = buffer;

      memset(buffer,0,sizeof(buffer));
      (*rules[ruleset].conv)(&rules[ruleset],&tstr);
            
      if (rules[ruleset].width == 0)
        width = tstr.size;
      else if (tstr.size > rules[ruleset].width)
      {
	tstr.size = 0;
	pad	  = '*';
	align	  = 'L';
      }
      padsize = width - tstr.size;
      
      if (align == 'L')
      {
	lpad = 0;
	rpad = padsize;
      }
      else if (align == 'R')
      {
	lpad = padsize;
	rpad = 0;
      }
      else
      {
	lpad = padsize / 2;
	rpad = lpad + (padsize & 1);
      }

      for (i = 0 ; i < lpad ; i++ , output++)
	(*funct)(pad,datum);
      for (i = 0 ; i < tstr.size ; i++ , output++)
	(*funct)(tstr.text[i],datum);
      for (i = 0 ; i < rpad ; i++ , output++)
	(*funct)(pad,datum);
    }
  }
  return(output);
}

/************************************************************************/

static int rdf_getconv(
			struct conversion *rules,
			int		   mrules,
			const char	  *mfmt,
			va_list 	   list
		      )
{
  char *fmt;
  int   i = -1;

  ddt(rules  != NULL);
  ddt(mrules >	0);
  ddt(mfmt   != NULL);

  fmt = (char *)mfmt;

  while((i < mrules) && (*fmt))
  {
    if (isspace(*fmt))
    {
      fmt++;
      continue;
    }
    
    i++;
    rules[i].type  = *fmt++;
    
    if (*fmt == '*')		/* width */
      rules[i].width = va_arg(list,int) , fmt++;
    else
      rules[i].width = strtoul(fmt,&fmt,10);

    if (*fmt == '.')		/* minimum/precision */
    {
      fmt++;
      if (*fmt == '*')
        rules[i].min = va_arg(list,int) , fmt++;
      else
        rules[i].min = strtoul(fmt,&fmt,10);
    }
    else
      rules[i].min   = 0;
    ddt(rules[i].width >= rules[i].min);

    if (*fmt == '*')		/* alignment */
      rules[i].align = va_arg(list,int) , fmt++;
    else if ((*fmt) && (strchr("LRC",toupper(*fmt)) != NULL))
      rules[i].align = toupper(*fmt++);
    else
      rules[i].align = 'R';

    if (*fmt == '*')		/* padding character */
      rules[i].pad = va_arg(list,int) , fmt++;
    else if ((*fmt) && (!isspace(*fmt)))
      rules[i].pad = *fmt++;
    else
      rules[i].pad = ' ';

    /* XXX add check for printing sign character */

    rules[i].base = 10;

    switch(rules[i].type)
    {
      case '%':
	   rules[i].datum.c = '%';
	   rules[i].conv    = rdf_c_char;
	   break;
      case 'c':
      case 'C':
	   rules[i].datum.c  = va_arg(list,int);
	   rules[i].conv     = rdf_c_char;
	   break;
      case 's':
	   rules[i].datum.l  = va_arg(list,int);
	   rules[i].conv     = rdf_c_signed;
	   break;
      case 'S':
	   rules[i].datum.ul = va_arg(list,int);
	   rules[i].conv     = rdf_c_unsigned;
	   break;
      case 'i':
	   rules[i].datum.l  = va_arg(list,int);
	   rules[i].conv     = rdf_c_signed;
	   break;
      case 'o':
      case 'I':
	   rules[i].datum.ul = va_arg(list,unsigned int);
	   rules[i].conv     = rdf_c_unsigned;
	   rules[i].base     = (rules[i].type == 'o')
				? 8
				: 10;
	   break;
      case 'l':
	   rules[i].datum.l  = va_arg(list,long);
	   rules[i].conv     = rdf_c_signed;
	   break;
      case 'x':
      case 'L':
	   rules[i].datum.ul = va_arg(list,unsigned long);
	   rules[i].conv     = rdf_c_unsigned;
	   rules[i].base     = (rules[i].type == 'x')
				? 16
				: 10;
	   break;
      case 'f':
      case 'd':
	   rules[i].datum.d  = va_arg(list,double);
	   rules[i].conv     = rdf_c_double;
	   break;
      case '$':
	   rules[i].datum.p  = va_arg(list,char *);
	   rules[i].conv     = rdf_c_string;
	   break;
      case 'p':
	   rules[i].datum.p  = va_arg(list,char *);
	   rules[i].conv     = rdf_c_pointer;
	   rules[i].base     = 16;
	   break;
      case '>':
	   rules[i].conv     = rdf_c_padding;
	   break;
      default:
	   break;
    }
  }
  return(i+1);
}

/******************************************************************/

static void rdf_c_char(struct conversion *psc,struct tmpstr *pts)
{
  pts->size    = 1;
  pts->text[0] = psc->datum.c;
}

/******************************************************************/

static void rdf_c_signed(struct conversion *psc,struct tmpstr *pts)
{
  int minus = FALSE;

  if (psc->datum.l < 0)
  {
    minus         = TRUE;
    psc->datum.ul = -psc->datum.l;
    pts->text++;	/* adjust things so we can include the '-'      */
    pts->size--;	/* sign when we return ...			*/
  }
  else
    psc->datum.ul = psc->datum.l;

  rdf_c_unsigned(psc,pts);

  if (minus)
  {
    *--(pts->text) = '-';
    pts->size++;
    psc->datum.l = -psc->datum.ul;
  }
}

/**********************************************************************/

static void rdf_c_unsigned(struct conversion *psc,struct tmpstr *pts)
{
  static const char  digs[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  unsigned long      uval;
  long		     val;
  size_t	     idx;
  char		    *s;
  int                base;
  ldiv_t             qr;

  uval	   = psc->datum.ul;
  base     = psc->base;
  idx	   = pts->size;
  s        = pts->text;

  s[--idx] = digs[uval % (unsigned long)base];
  val	   = uval / (unsigned long)base;

  while((val > 0) && (idx > 0))
  {
    qr	     = ldiv(val,base);
    val      = qr.quot;
    s[--idx] = digs[qr.rem];
  }
  pts->text  = &s[idx];
  pts->size -= idx;
  for ( ; (idx > 0) && (pts->size < psc->min) ; idx-- , pts->size++)
  {
    *(--pts->text) = '0';
  }
}

/*********************************************************************/

static void rdf_c_double(struct conversion *psc,struct tmpstr *pts)
{
  psc->width = INT_MAX - 1;
  pts->size  = INT_MAX;
}

/**********************************************************************/

static void rdf_c_pointer(struct conversion *psc,struct tmpstr *pts)
{
  psc->datum.ul = (unsigned long)psc->datum.p;
  pts->text++;		/* adjust things so we can include the '$'	*/
  pts->size--;
  rdf_c_unsigned(psc,pts);
  *--(pts->text) = '$';
  pts->size++;
}

/**********************************************************************/

static void rdf_c_padding(struct conversion *psc,struct tmpstr *pts)
{
  pts->size = 0;
}

/********************************************************************/

static void rdf_c_string(struct conversion *psc,struct tmpstr *pts)
{
  pts->size = strlen(psc->datum.p);
  pts->text = psc->datum.p;
}

