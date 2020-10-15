/************************************************************************/
/* matfree.c - FREE A PSEUDO-2D ARRAY CREATED BY MATALLOC		*/
/*									*/
/* ptr = pointer value that was returned by matalloc()			*/
/*									*/
/* Ver. 2.0.4 (2016-09-28)						*/
/* (c) 2016 Jeffrey J. Rodriguez. All rights reserved.			*/
/* jjrodrig@email.arizona.edu						*/

#include "dip.h"

/* typedef of struct used by matalloc & matfree */
#ifndef _memnode
#define _memnode
typedef struct memnode_struct
  {void *userptr, *trueptr; struct memnode_struct *next;}
  memnode_t;
#endif

/* declaration of global variable used by matalloc & matfree */
extern memnode_t matlist;

void matfree(void *ptr)
{
  memnode_t *prev, *p;

  if(ptr == NULL) error("matfree: can't free a NULL ptr");

  prev = &matlist;
  for(p = matlist.next; p != NULL; prev = p, p = p->next)
    if(p->userptr == ptr)
    { /* found the matching user ptr */
      free(p->trueptr); /* free the corresponding true ptr */
      prev->next = p->next;
      free(p); /* free the memnode */
      break;
    }
  if(p == NULL)
    error("matfree: can't free a ptr not created by matalloc");
}
