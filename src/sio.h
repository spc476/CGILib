
#ifndef SIO_H
#define SIO_H

#include <stdlib.h>
#include <stddef.h>

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
  int     (*readchar)  (struct sinput *);
  size_t  (*readblock) (struct sinput *,void *,size_t);
  char   *(*readline)  (struct sinput *);
  int     (*unread)    (struct sinput *,int);
  int     (*close)     (struct sinput *);
} *SInput;

typedef struct soutput
{
  size_t   bytes;
  int      eof;
  int      error;
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

SOutput		 (NullSOutput)		(void);
SOutput		 (FileSOutput)		(const char *,int);
SOutput		 (FHSOutput)		(int);
SOutput		 (MemorySOutput)	(void *,size_t);

int		 (TCPSInputOutput)	(SInput *,SOutput *,const char *,int);

int		 (SIChar)		(SInput);
size_t		 (SIBlock)		(SInput,void *,size_t);
char		*(SILine)		(SInput);
int		 (SIEof)		(SInput);
int		 (SIFree)		(SInput);

size_t		 (SOChar)		(SOutput,int);
size_t		 (SOBlock)		(SOutput,void *,size_t);
size_t		 (SOLine)		(SOutput,char *);
size_t		 (SOFlush)		(SOutput);
int		 (SOEof)		(SOutput);
int		 (SOFree)		(SOutput);

/*************************************************************************/

#ifdef SCREAM
#  define SIChar(in)		((*(in)->readchar)   ((in))
#  define SIBlock(in,d,s)	((*(in)->readblock)  ((in),(d),(s))
#  define SILine(in)		((*(in)->readline)   ((in))

#  define SOChar(out,c)		((*(out)->writechar)   ((out),(c))
#  define SOBlock(out,d,s)	((*(out)->writeblock)  ((out),(d),(s))
#  define SOLine(out,s)	        ((*(out)->writeline) ((out),(s))
#  define SOFlush(out)		((*(out)->flush)       ((out))
#endif

#endif

