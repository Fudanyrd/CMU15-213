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
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "fdyrd",
    /* First member's full name */
    "Rundong Yang",
    /* First member's email address */
    "yangrundong7@gmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/**
 * In my Implementation I built 3 free list:
 *  1. small free blocks(smaller than 32 bytes)
 *  2. middle free blocks(no less than 32 bytes, less than 1024 bytes)
 *  3. large free blocks(larger than 1024 bytes)
 * All the three list is doubly linked, the meta of each node is as follows:
 * [size(61 bit) | used(3 bit) | prev(64 bit) | succ(64 bit) ]
 *
 * We first allocate 2 pages for the heap area, then give the space to
 * initialize large free list.
 */

/** free list of small(<= 32) blocks */
void *mm_small;
/** free list of middle(32, 1024) blocks */
void *mm_middle;
/** free list of large(>= 1024) blocks */
void *mm_large;

/**
 * put a node to the front of either mm_small, mm_middle or mm_large(based on
 * size)
 */
void push_front(void *);

/** End of (free) List */
#define MMEOL ((void *)0)
/** Size of node meta */
#define METASZ (ALIGN(sizeof(size_t) * 3))
/** C stype casting is ugly */
#define static_cast(a, tp) ((tp)(a))
#include <assert.h>

#ifndef static_assert
#define static_assert(pred)                                                    \
  switch (0) {                                                                 \
  case (0):                                                                    \
  case (pred):                                                                 \
  }
#endif

/**
 * @return the biased pointer.
 */
inline void *get_adr(void *pt, size_t bias) {
  return static_cast(static_cast(pt, char *) + bias, void *);
}

/**
 * @brief allocate new page by growing heap pointer. Set its meta.
 * @return the head of allocated page
 */
void *alloc_page(size_t num_page);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
#ifdef DEBUG
  static_assert(sizeof(size_t) == 8 || sizeof(size_t) == 4);
#endif

  // small, middle list is empty.
  mm_small = mm_middle = MMEOL;
  size_t init_sz = mem_pagesize() * 2;
#ifdef DEBUG
  assert(init_sz > METASZ);
  assert(init_sz - METASZ > 1024U);
  assert((init_sz & 0x7) == 0);
#endif
  mm_large = alloc_page(2);
  if (!static_cast(mm_large, int)) {
    // initial allocation failure?! Impossible!
    return -1;
  }

  return 0;
}

/**
 * @brief For a given size and the head pointer of a free list,
 * find the **first** block that has a larger volume than size.
 * @param head head of the list to look into.
 * @param size size of block to fit.
 *
 * @return MMEOL is failed to find any.
 */
void *find_fit(void *head, size_t size);

/**
 * @brief take the vacancy of a node and set its successor and size properly.
 * If nessesary, add the remnent to one of the 3 free lists.
 *
 * @param node the pointer to the node to take from
 * @param aligned size needed to allocate
 */
void take(void *node, size_t aligned);

void *mm_malloc(size_t size) {
  // actual space needed.
  if (size == 0U) {
    return MMEOL;
  }
  const size_t actual = ALIGN(size);
  void *res = MMEOL;
  if (actual < 32U) {
    // first look into mm_small.
    res = find_fit(mm_small, actual);
    // find!
    if (static_cast(res, int)) {
      take(res, actual);
      return get_adr(res, METASZ);
    }

    // else look into mm_middle.
    res = find_fit(mm_middle, actual);
    if (static_cast(res, int)) {
      // find!
      take(res, actual);
      return get_adr(res, METASZ);
    }

    // else look into mm_large.
    res = find_fit(mm_large, actual);
    if (static_cast(res, int)) {
      // find!
      take(res, actual);
      return get_adr(res, METASZ);
    }

    // woa, need to allocate new heap page:)
    void *new_page = alloc_page(1U);
    if (new_page == MMEOL) {
      return MMEOL;
    }
    take(new_page, actual);
    return get_adr(new_page, METASZ);
  } else {
    if (actual < 1024U) {
      res = find_fit(mm_middle, actual);
      if (static_cast(res, int)) {
        // find!
        take(res, actual);
        return get_adr(res, METASZ);
      }

      res = find_fit(mm_large, actual);
      if (static_cast(res, int)) {
        // find!
        take(res, actual);
        return get_adr(res, METASZ);
      }
      // woa, need to allocate new heap page:)
      void *new_page = alloc_page(1U);
      if (new_page == MMEOL) {
        return MMEOL;
      }
      take(new_page, actual);
      return get_adr(new_page, METASZ);

    } else {
      res = find_fit(mm_large, actual);
      if (static_cast(res, int)) {
        // find!
        take(res, actual);
        return get_adr(res, METASZ);
      }
      // woa, need to allocate new heap page:)
      void *new_page = alloc_page(1U + (actual + METASZ - 1U) / mem_pagesize());
      if (new_page == MMEOL) {
        return MMEOL;
      }
      take(new_page, actual);
      return get_adr(new_page, METASZ);
    }
  }

  return MMEOL;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc_naive(size_t size) {
  int newsize = ALIGN(size + SIZE_T_SIZE);
  void *p = mem_sbrk(newsize);
  if (p == (void *)-1)
    return NULL;
  else {
    *(size_t *)p = size;
    return (void *)((char *)p + SIZE_T_SIZE);
  }
}

/**
 * @brief Free the pointer allocated by mm_malloc.
 */
void mm_free(void *ptr) {
  if (!static_cast(ptr, int)) {
    return;
  }
  // findout the size of it.
  void *node = static_cast(static_cast(ptr, char *) - METASZ, void *);
  size_t *meta = static_cast(node, size_t *);
#ifdef DEBUG
  assert((meta[0] & 0x7) == 0);
#endif
  // add it to the free list.
  push_front(node);
}

/*
 * mm_free - Freeing a block does nothing.
 */

void mm_free_naive(void *ptr) {

  if (!static_cast(ptr, int)) {
    return;
  }
  // findout the size of it.
  void *node = static_cast(static_cast(ptr, char *) - METASZ, void *);
  size_t *meta = static_cast(node, size_t *);
#ifdef DEBUG
  assert((meta[0] & 0x7) == 0);
#endif
  // add it to the free list.
  push_front(node);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
  void *oldptr = ptr;
  void *newptr;
  size_t copySize;

  newptr = mm_malloc(size);
  if (newptr == NULL)
    return NULL;
  copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
  if (size < copySize)
    copySize = size;
  memcpy(newptr, oldptr, copySize);
  mm_free(oldptr);
  return newptr;
}

/************************************************
 * HELPER FUNCTIONS
 ************************************************/
void *find_fit(void *head, size_t size) {
  if (head == MMEOL) {
    return MMEOL;
  }
  // iterator over the whole free list.
  void *it = head;
  size_t *meta;
  // available size of a node
  size_t avail;

  while (it != MMEOL) {
    meta = static_cast(it, size_t *);
    avail = meta[0] & (~0x7);
    if ((meta[0] & 0x7) == 0 && avail >= size) {
      // OK, find one node;
      return it;
    }
    // else it = it->successor
    it = static_cast(meta[2], void *);
  }

  // failure:)
  return it;
}

void take(void *node, size_t aligned) {
  size_t *meta = static_cast(node, size_t *);
#ifdef DEBUG // will only be run in debug mode.
  // make sure the size is aligned.
  assert((aligned & 0x7) == 0);
  // make sure the node is empty.
  assert((meta[0] & 0x7U) == 0);
  // make sure space is enough.
  assert((meta[0] & (~0x7)) >= aligned);
#endif
  size_t left = meta[0] - aligned;
  void *predecessor = static_cast(meta[1], void *);
  void *successor = static_cast(meta[2], void *);
  // evict the node from the free list; allocate all of it.
  if (predecessor != MMEOL) {
    size_t *pred_meta = static_cast(predecessor, size_t *);
    pred_meta[2] = static_cast(successor, size_t);
  }
  if (successor != MMEOL) {
    size_t *succ_meta = static_cast(successor, size_t *);
    succ_meta[1] = static_cast(predecessor, size_t);
  }
  if (node == mm_small) {
    mm_small = successor;
  }
  if (node == mm_middle) {
    mm_middle = successor;
  }
  if (node == mm_large) {
    mm_large = successor;
  }
  // too small; don't care.
  if (left < 16U + METASZ) {
  } else {
    // the left size is still large, split the node instead.
    meta[0] = aligned;
    void *splitted = get_adr(node, METASZ + aligned);
    meta = static_cast(splitted, size_t *);
    meta[0] = left - METASZ; // at least 16.
    push_front(splitted);
  }
}

void push_front(void *node) {
  size_t *meta = static_cast(node, size_t *);
  size_t *head_meta;
#ifdef DEBUG
  assert((meta[0] & 0x7) == 0);
#endif
  meta[1] = static_cast(MMEOL, size_t);
  if (meta[0] < 32U) {
    meta[2] = static_cast(mm_small, size_t);
    if (static_cast(mm_small, int)) {
      head_meta = static_cast(mm_small, size_t *);
      head_meta[1] = static_cast(node, size_t);
    }
    mm_small = node;
  } else {
    if (meta[0] < 1024U) {
      meta[2] = static_cast(mm_middle, size_t);
      if (static_cast(mm_middle, int)) {
        head_meta = static_cast(mm_middle, size_t *);
        head_meta[1] = static_cast(node, size_t);
      }
      mm_middle = node;
    } else {
      meta[2] = static_cast(mm_large, size_t);
      if (static_cast(mm_large, int)) {
        head_meta = static_cast(mm_large, size_t *);
        head_meta[1] = static_cast(node, size_t);
      }
      mm_large = node;
    }
  }
}

void *alloc_page(size_t num_page) {
  if (num_page == 0) {
    // refuse the request
    return MMEOL;
  }
  void *node = mem_sbrk(num_page * mem_pagesize());
  if (node == (void *)-1) {
    return MMEOL;
  }
  size_t *meta = static_cast(node, size_t *);
#ifdef DEBUG
  assert(num_page * mem_pagesize() >= METASZ);
#endif
  meta[0] = num_page * mem_pagesize() - METASZ;
  meta[1] = meta[2] = static_cast(MMEOL, size_t);

  return node;
}
