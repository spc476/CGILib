
#ifndef LLRB_TREE_H
#define LLRB_TREE_H

typedef union all
{
  unsigned char      uc;
  signed char        c;
  unsigned short int us;
  short int          s;
  unsigned int       ui;
  int                i;
  unsigned long      ul;
  long               l;
  unsigned long long ull;
  long long          ll;
  void              *p;
} all__t;

typedef struct llrbnode
{
  struct llrbnode *left;
  struct llrbnode *right;
  bool             red;
  all__t           key;
  all__t           value;
} llrbnode__t;

typedef struct llrbtree
{
  struct llrbnode  *left;
  int             (*cmp)(all__t,all__t);
} llrbtree__t;

/**************************************************************************/

void	LLRBTreeInsert	(llrbtree__t *const,all__t,all__t);
bool	LLRBTreeFind	(llrbtree__t *const,all__t,all__t *);
void	LLRBTreeDelete	(llrbtree__t *const,all__t);

/**************************************************************************/

#endif
