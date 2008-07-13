
#ifndef SIO_H
#define SIO_H

#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>

struct buffer
{
  char *data;
  size_t size;
  size_t max;
  size_t idx;
};

typedef struct sinput
{
  size_t    bytes;
  int       eof;
  int       error;
  char      eoln[3];
  int     (*readchar)  (struct sinput *);
  size_t  (*readblock) (struct sinput *,void *,size_t);
  char   *(*readline)  (struct sinput *);
  int     (*close)     (struct sinput *);
} *SInput;

typedef struct soutput
{
  size_t   bytes;
  int      eof;
  int      error;
  char     eoln[3];
  size_t (*writechar)  (struct soutput *,int);
  size_t (*writeblock) (struct soutput *,void *,size_t);
  size_t (*writeline)  (struct soutput *,const char *);
  size_t (*flush)      (struct soutput *);
  int    (*close)      (struct soutput *);
} *SOutput;

/************************************************************************/

SInput		 (NullSInput)		(void);
SInput		 (FileSInput)		(const char *);
SInput		 (FHSInput)		(int);
SInput		 (MemorySInput)		(void *,size_t);
SInput		 (BundleSInput)		(void);

SOutput		 (NullSOutput)		(void);
SOutput		 (FileSOutput)		(const char *,int);
SOutput		 (FHSOutput)		(int);
SOutput		 (MemorySOutput)	(void *,size_t);
SOutput		 (BundleSOutput)	(void);

int		 (TCPSInputOutput)	(SInput *,SOutput *,const char *,int);
void		 (BundleSInputAdd)	(SInput,SInput);
void		 (BundleSOutputAdd)	(SOutput,SOutput);
size_t		 (SIOCopy)		(SOutput,SInput);
size_t		 (SIOCopyN)		(SOutput,SInput,size_t);

void		 (SIEoln)		(SInput,const char *);
int		 (SIChar)		(SInput);
size_t		 (SIBlock)		(SInput,void *,size_t);
char		*(SILine)		(SInput);
int		 (SIEof)		(SInput);
int		 (SIFree)		(SInput);

void		 (SOEoln)		(SOutput,const char *);
size_t		 (SOChar)		(SOutput,int);
size_t		 (SOBlock)		(SOutput,void *,size_t);
size_t		 (SOLine)		(SOutput,char *);
size_t		 (SOLineF)		(SOutput,const char *,const char *, ...);
size_t		 (SOLineFv)		(SOutput,const char *,const char *,va_list);
size_t		 (SOFlush)		(SOutput);
int		 (SOEof)		(SOutput);
int		 (SOFree)		(SOutput);

	/*---------------------------------------------------------
	; the following functions are not meant to be called
	; by application code.  Instead, they are used to quickly
	; get new sub-modules running.  They work by repeatedly
	; calling the character-by-character methods.
	;---------------------------------------------------------*/

size_t		  slow_readblock	(struct sinput *,void *,size_t);
char		 *slow_readline		(struct sinput *);
size_t		  slow_writeblock	(struct soutput *,void *,size_t);
size_t		  slow_writeline	(struct soutput *,const char *);

/*************************************************************************/

#ifdef SCREAM_NO
#  define SIChar(in)		((*(in)->readchar)  ((in))
#  define SIBlock(in,d,s)	((*(in)->readblock) ((in),(d),(s))
#  define SILine(in)		((*(in)->readline)  ((in))
#  define SIFree(in)		((*(in)->close)     ((in))

#  define SOChar(out,c)		((*(out)->writechar)  ((out),(c))
#  define SOBlock(out,d,s)	((*(out)->writeblock) ((out),(d),(s))
#  define SOLine(out,s)	        ((*(out)->writeline)  ((out),(s))
#  define SOFlush(out)		((*(out)->flush)      ((out))
#  define SOFree(out)		((*(out)->close)      ((out))
#endif

#endif

