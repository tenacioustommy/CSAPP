/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)
#define MAX(a,b) a>b?a:b

#define PACK(size,alloc) ((size)|(alloc))
#define GET(p) (*(unsigned int*)(p))
#define PUT(p,val) (*(unsigned int*)(p)=(val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char*)(bp) - WSIZE)
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char*)(bp) - GET_SIZE((char*)(bp)- DSIZE))

static char* heap_listp;
/* 
 * mm_init - initialize the malloc package.
 */
static void* coalesce(void *bp){
    size_t prev_alloc=GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc=GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(prev_alloc&&next_alloc){
        return bp;
    }
    else if(prev_alloc &&!next_alloc){
        size+=GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp),PACK(size,0));
        PUT(FTRP(bp),PACK(size,0));   
    }
    else if(!prev_alloc &&next_alloc){
        size+=GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp=PREV_BLKP(bp);
    }
    else{
        size+=GET_SIZE(HDRP(PREV_BLKP(bp)))+GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
        bp=PREV_BLKP(bp);
    }
    return bp;
}
static void *extend_heap(size_t size){
    char *bp;
    size=ALIGN(size);
    if((bp=mem_sbrk(size))==(void*)-1){
        return NULL;
    }
    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));
    
    return coalesce(bp);
}

int mm_init(void)
{
    if((heap_listp=mem_sbrk(4*ALIGNMENT))==(void*)-1){
        return -1;
    }
    PUT(heap_listp,0);
    PUT(heap_listp+(1*WSIZE),PACK(DSIZE,1));
    PUT(heap_listp+(2*WSIZE),PACK(DSIZE,1));
    PUT(heap_listp+(3*WSIZE),PACK(0,1));
    heap_listp+=2*WSIZE;

    if(extend_heap(CHUNKSIZE)==NULL){
        return -1;
    }
    return 0;
}


static void* find_fit(size_t asize){
    char* bp=heap_listp+2*WSIZE;
    while(GET_SIZE(HDRP(bp))!=0){
       if(GET_ALLOC(HDRP(bp))==0&&GET_SIZE(HDRP(bp))>=asize){
        return bp;
       } 
       bp=NEXT_BLKP(bp);
    }
    return NULL;

}
static void place(char * bp,size_t asize){
    size_t currsize=GET_SIZE(bp);
    size_t size=currsize-asize;
    PUT(HDRP(bp),PACK(asize,1));
    PUT(FTRP(bp),PACK(asize,1));
    PUT(HDRP(NEXT_BLKP(bp)),PACK(size,0));
    PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
}
/* 
 
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *bp;
    if(size==0)return NULL;
    asize=ALIGN(size+DSIZE);

    if((bp=find_fit(asize))!=NULL){
        place(bp,asize);
        return bp;
    }
    extendsize=MAX(asize,CHUNKSIZE);
    if((bp=extend_heap(extendsize))==NULL){
        return NULL;
    }
    place(bp,asize);
    return bp;

}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    size_t size=GET_SIZE(HDRP(bp));
    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    coalesce(bp);
}

static void move_data(void *curr, void *target,size_t asize){
    
    unsigned int* cur=curr;
    unsigned int* tar=target;
    for(int i=0;i<asize/WSIZE-2;i++)
    {
        PUT(tar,*cur);
        cur++;
        target++;
    }
}
/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *bp, size_t size)
{
    size_t asize=ALIGN(size+DSIZE);
    size_t nextsize=GET_SIZE(NEXT_BLKP(HDRP(bp)));
    size_t currsize=GET_SIZE(HDRP(bp));
    size_t extendsize;
    int alloc=GET_ALLOC(NEXT_BLKP(HDRP(bp)));
    if(!alloc&&asize<=nextsize+currsize){
        place(bp,asize);
        return bp;
    }
    else{
        char *tmp=bp;
        coalesce(bp);
        if((bp=find_fit(asize))!=NULL){
            place(bp,asize);
            move_data(tmp,bp,asize);
            return bp;
        }
        extendsize =MAX(asize,CHUNKSIZE);
        if((bp=extend_heap(extendsize))==NULL){
            return NULL;
        }
        place(bp,asize);
        move_data(tmp,bp,asize);
        return bp;
       
    }
}














