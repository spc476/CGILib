
#ifndef SIO_H
#define SIO_H

struct buffer
{
  char *data;
  size_t size;
  size_t max;
  size_t idx;
};

typedef struct sinput
{
  size_t   bytes;
  int      eof;
  int      error;
  int    (*readchar)  (struct sinput *);
  size_t (*readblock) (struct sinput *,void *,size_t);
  char   (*readstring)(struct sinput *);
  int    (*unread)    (struct sinput *,int);
  int    (*close)     (struct sinput *);
} *SInput;

typedef struct soutput
{
  size_t   bytes;
  int      eof;
  int      error;
  size_t (*writechar)  (struct soutput *,int);
  size_t (*writeblock) (struct soutput *,void *,size_t);
  size_t (*writestring)(struct souput *,const char *);
  size_t (*flush)      (struct soutput *);
  int    (*close)      (struct soutput *);
} *SOutput;

/************************************************************************/

SInput		 (NullSInput)		(void);
SInput		 (FileSInput)		(const char *);
SInput		 (FHSInput)		(int);

SOutput		 (NullSOutput)		(void);
SOutput		 (FileSOutput)		(const char *,int);
SOutput		 (FHSOutput)		(int);

int		 (TCPSInputOutput)	(SInput *,SOutput *,const char *,int);

int		 (SIChar)		(SInput);
size_t		 (SIBlock)		(SInput,void *,size_t);
char		*(SIString)		(SInput);
int		 (SIEof)		(SInput);
int		 (SIFree)		(SInput);

size_t		 (SOChar)		(SOutput,int);
size_t		 (SOBlock)		(SOutput,void *,size_t);
size_t		 (SOString)		(SOutput,char *);
size_t		 (SOFlush)		(SOutput);
int		 (SOEof)		(SOutput);
int		 (SOFree)		(SOutput);

/*************************************************************************/

#ifdef SCREAM
#  define SIChar(in)		((*(in)->readchar)   ((in))
#  define SIBlock(in,d,s)	((*(in)->readblock)  ((in),(d),(s))
#  define SIString(in)		((*(in)->readstring) ((in))

#  define SOChar(out,c)		((*(out)->writechar)   ((out),(c))
#  define SOBlock(out,d,s)	((*(out)->writeblock)  ((out),(d),(s))
#  define SOString(out,s)	((*(out)->writestring) ((out),(s))
#  define SOFlush(out)		((*(out)->flush)       ((out))
#endif

#endif

