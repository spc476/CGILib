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
*************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include "buffer.h"
#include "types.h"
#include "util.h"
#include "memory.h"
#include "rawfmt.h"
#include "ddt.h"

/***************************************************************************/

char *spc_getenv(const char *env)
{
  const char *p;
  char       *ret;
  
  ddt(env != NULL);
  
  p = getenv(env);
  if (p == NULL)
  {
    p = "";
  }

  ret = dup_string(p);  
  return(ret);
}

/************************************************************************/

char *up_string(char *s)
{
  char *r = s;
  
  ddt(s != NULL);
  
  for ( ; *s ; s++)
    *s = toupper(*s);
  return(r);
}

/***************************************************************************/

char *down_string(char *s)
{
  char *r = s;
  
  ddt(s != NULL);
  
  for ( ; *s ; s++)
    *s = tolower(*s);
  return(r);
}

/*************************************************************************/

char *dup_string(const char *s)
{
  char *r;
  
  ddt(s != NULL);

  r = MemAlloc(strlen(s) + 1);
  if (r == NULL) return(NULL);
  strcpy(r,s);
  return(r);
}

/***************************************************************************/

void free_string(const char *s)
{
  MemFree(s,strlen(s) + 1);
}

/***************************************************************************/

char *concat_strings(const char *str1, ... )
{
  va_list     parms;
  char       *str;
  const char *s;
  char       *nstr;
  size_t      sz;
  size_t      nsz;
            
  ddt(str1 != NULL);
              
  va_start(parms,str1);

  for (
        str = NULL , s = str1 , sz = 0 ;
        s != (char *)NULL ;
        s = va_arg(parms,char *)
      )
  {
    nsz  = strlen(s);
    nstr = MemResize(str,(sz) ? (sz+1) : sz,sz + nsz + 1);
    memcpy(&nstr[sz],s,nsz);
    sz         += nsz;
    nstr[sz]  = '\0';
    str         = nstr;
  }
  
  va_end(parms);
  return(str);
}

/**************************************************************************/

int empty_string(const char *s)
{
  ddt(s != NULL);
  
  for ( ; *s ; s++)
  {
    if (!isspace(*s)) return(FALSE);
  }
  return(TRUE);
}

/*************************************************************************/

char *remove_ctrl(char *s)
{
  ddt(s != NULL);
  
  return (remove_char(s,iscntrl));
}

/*************************************************************************/

char *remove_char(char *s,int (*tstchar)(int))
{
  char *ret = s;
  char *d   = s;
  
  ddt(s != NULL);
  ddt(tstchar);		/* can function pointers be compared to NULL? */
  
  for ( ; *s ; s++)
  {
    if (!(*tstchar)(*s)) *d++ = *s;
  }
  *d = '\0';
  return(ret);
}

/************************************************************************/

char *trim_lspace(char *s)
{
  ddt(s != NULL);  
  for ( ; (*s) && (isspace(*s)) ; s++)
    ;
  return(s);
}

/*************************************************************************/

char *trim_tspace(char *s)
{
  char *p;
  
  ddt(s         != NULL);
  ddt(strlen(s) >  0);
  
  for (p = s + strlen(s) - 1 ; (p > s) && (isspace(*p)) ; p--)
    ;
  *(p+1) = '\0';
  return(s);
}

/**************************************************************************/

char *trim_space(char *s)
{
  ddt(s != NULL);
  return(trim_tspace(trim_lspace(s)));
}

/***************************************************************************/

int ctohex(char c)
{
  ddt(isxdigit(c));
 
  c = toupper(c); 
  c -= '0';
  if (c > 9) c -= 7;
  ddt(c >= 0);	/* warning on AIX - apparently chars are unsigned */
  ddt(c <  16);
  return(c);
}

/***************************************************************************/

char hextoc(int i)
{
  char c;
  
  ddt(i >= 0);
  ddt(i <  16);
  
  c = i+'0';
  if (c > '9') c += 7;
  ddt(isxdigit(c));
  return(c);
}

/**************************************************************************/

char *cat_string(char *dest,const char *src)
{
  ddt(dest  != NULL);
  ddt(src   != NULL);
  ddt(*dest == '\0');

  for ( ; (*dest = *src) ; dest++ , src++ )
    ;
  return(dest);
}

/**********************************************************************/

struct mystring
{
  size_t size;
  char   *p;
};

static void fcallback(char c,void *d)
{
  ddt(d != NULL);
  
  if (((struct mystring *)d)->size)
  {
    *(((struct mystring *)d)->p++) = c;
    *(((struct mystring *)d)->p)   = '\0'; 
     (((struct mystring *)d)->size)--;
  }
}

size_t formatstr(char *dest,size_t dsize,const char *fmt,const char *msg, ... )
{
  va_list         list;
  struct mystring data;
  size_t          size;
  
  ddt(dest  != NULL);
  ddt(dsize >  0);
  ddt(fmt   != NULL);
  ddt(msg   != NULL);
  
  va_start(list,msg);
  data.size = dsize;
  data.p    = dest;
  size      = RawDoFmtv(fmt,msg,fcallback,&data,list);
  va_end(list);
  return(size);
}

/************************************************************************/

#define LINESIZE	16

void dump_memory(Buffer out,const unsigned char *block,size_t size,size_t offset)
{
  char ascii[LINESIZE + 1];
  int  skip;
  int  j;
  
  ddt(out   != NULL);
  ddt(block != NULL);
  ddt(size  >  0);

  while(size > 0)
  {
    BufferFormatWrite(out,"x8.8l0","%a: ",(unsigned long)offset);
    
    for ( skip = offset % LINESIZE , j = 0 ; skip ; j++ , skip-- )
    {
      BufferFormatWrite(out,"","   ");
      ascii[j] = ' ';
    }
    
    do
    {
      BufferFormatWrite(out,"x2.2l0","%a ",(unsigned long)*block);
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
        BufferFormatWrite(out,"","   ");
    }
    BufferFormatWrite(out,"$","%a\n",ascii);
  }
}

/*********************************************************************/


      
