/************************************************************************/
/* matalloc.c - DYNAMICALLY ALLOCATE A PSEUDO-2D ARRAY			*/
/*									*/
/* Allocate a pseudo-2D array of size nrows x ncols.			*/
/* The coordinates of the first pixel will be firstRowCoord and		*/
/* firstColCoord. The data structure consists of one contiguous		*/
/* chunk of memory, consisting of a sequence of row pointers, followed	*/
/* by the array element values.						*/
/* Assume that nrows*ncols*elementSize, rounded up to a multiple of	*/
/* sizeof(long double), will fit in a long integer.  If not, then	*/
/* the "nbytes += ..." step could overflow.				*/
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

/* global variable used by matalloc & matfree */
memnode_t matlist={NULL, NULL, NULL};

void *matalloc(int32_t nrows, int32_t ncols, int32_t firstRowCoord,
  int32_t firstColCoord, int element_size)
{
  void **p, **matrix;
  size_t bytes_for_row_ptrs, i, alignment, nbytes;
  memnode_t *newnode;
 
  if(nrows < 1 || ncols < 1)
    error("matalloc: bad nrows or ncols");

  /* compute the number of bytes needed to store the row ptrs */
  bytes_for_row_ptrs = nrows * sizeof(void *);

  /* align to the next multiple of sizeof(long double) */
  alignment = bytes_for_row_ptrs % sizeof(long double);
  if(alignment != 0) alignment = sizeof(long double) - alignment;

  /* calculate total number of bytes to allocate */
  nbytes = bytes_for_row_ptrs + nrows * ncols * element_size + alignment;

  /* allocate the big chunk of memory */
  if((p = malloc(nbytes)) == NULL)
    error("matalloc: malloc failed");

  /* compute the address of matrix[firstRowCoord][0] */
  p[0] = (char *)(p+nrows) + alignment - firstColCoord * element_size;

  /* compute the address of matrix[firstRowCoord+i][0] for each i */
  for(i = 1; i < nrows; i++)
    p[i] = (char *)(p[i-1]) + ncols * element_size;
  
  /* compute address of matrix[0][0] */
  matrix = p-firstRowCoord;

  /* add the memory location to matlist for later use by matfree */
  newnode = malloc(sizeof(memnode_t));
  if(newnode == NULL)
    error("matalloc: can't malloc memnode structure");
  newnode->userptr = matrix; /* ptr that user sees */
  newnode->trueptr = p;      /* ptr that matfree will free */
  newnode->next = matlist.next; /* insert newnode at start of list */
  matlist.next = newnode;

  /* return matrix ptr */
  return matrix;
}
