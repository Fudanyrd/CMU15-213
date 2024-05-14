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

/** last block of each free list and used list */
void *end_blk;

/** head of free list of small blocks(<= 32) */
void *mm_small;
/** head of free list of middle sized blocks(32-1024) */
void *mm_middle;
/** head of free list of large blocks(>=1024) */
void *mm_large;

/**
 * Layout of free block: [size | last | pred | succ ]
 * pred(predecessor), succ(successor) are used to look up in the free list;
 * while last, next are its absolute neighbors.
 */
struct free_meta {
  size_t size_; // size of entire block(including meta)
  size_t last_; // address of previous block(used or free)
  size_t pred_; // predecessor in the free list
  size_t succ_; // successor in the free list
};
/**
 * Layout of used block: [size | last ]
 * No reason to store predecessor and successor.
 */
struct used_meta {
  size_t size_; // size of entire block(including meta)
  size_t last_; // address of previous block(used or free)
};

/**
 * @return size of free block meta
 */
inline size_t free_meta_sz() { return ALIGN(sizeof(struct free_meta)); }
/**
 * @return size of used block meta
 */
inline size_t used_meta_sz() { return ALIGN(sizeof(struct used_meta)); }

/** Utility macros */
#define static_cast(a, Tp) ((Tp)(a))

/** end of list */
#define MMEOL ((void *)0)
/** minimum block volume(to avoid fragmentation) */
#define MINVOL (16U)

/**
 * @param node: the node in a free list or others.
 * @return the address of next block of node, null if node is end_blk.
 */
inline void *get_next(void *node) {
  return node == end_blk ? MMEOL
                         : node + (static_cast(node, size_t *)[0] & (~0x7));
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

/**
 * @brief check the integrity of the heap, including the following rule:
 * 1. mm_small, mm_middle, mm_large holds the correct size of free blocks.
 *
 * @return 0 if no integrity violations.
 */
int mm_check();

/**
 * @brief inline check heap consistency
 */
inline void check() {
#ifdef DEBUG
  assert(mm_check() == 0);
#endif
}
/**
 * @brief inline check end_blk consistency
 */
inline void check_end() {
#ifdef DEBUG
  assert(end_blk + (static_cast(end_blk, struct free_meta *)->size_ & (~0x7)) ==
         mem_heap_hi() + 1);
#endif
}

/**
 * @brief grow the heap size by some bytes. If end_blk is free, merge with
 * end_blk. Otherwise, reset end_blk and will not put it on the free list.
 * @return 0 if succeed.
 */
int grow_heap(size_t bytes);

/**
 * @brief add a free block to one of the free list.
 *
 * WARNING: you must set its size correctly and clear its used tag before
 * calling this function!
 */
void add_free_blk(void *);

/**
 * @brief remove a node from the free list.
 */
void remove_free_blk(void *blk);

/**
 * @brief merge two free block
 * @param left block with lower address
 * @param right block with higher address
 *
 * NOTE: left, right must be neighbors(will be checked in debug mode) and
 * address of left must be lower than right! Don't worry about their size.
 *
 * NOTE: this function will not add merged block(available in left) into free
 * list instead, you should manually do so.
 */
void merge_blk(void *left, void *right);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
#ifdef DEBUG
  static_assert(sizeof(size_t) == 4 || sizeof(size_t) == 8);
#endif
  // initialize first(also last) large block.
  size_t init_size = 2 * mem_pagesize();
  mm_large = mem_sbrk(init_size);
  // assert(mm_large != NULL && mm_large != end_blk);
  assert(mm_large != NULL);
  if (mm_large == (void *)-1) {
    // oops, fail
    return -1;
  }

  // initialize end_blk(last block in the heap)
  end_blk = mm_large;

  // initialize the meta of start block.
  struct free_meta *meta = static_cast(mm_large, struct free_meta *);
#ifdef DEBUG
  // unsigned sub can be problematic, must check.
  assert(init_size > free_meta_sz());
  // must be aligned.
  assert(((init_size - free_meta_sz()) & 0x7) == 0);
  assert((free_meta_sz() & 0x7) == 0);
  assert((used_meta_sz() & 0x7) == 0);
#endif
  meta->size_ = init_size; // size
  meta->last_ = 0;         // last = null
  meta->pred_ = 0;         // predecessor = null
  meta->succ_ = 0;         // successor = null

  check_end();
#ifdef DEBUG
  // check heap integrity
  assert(mm_check() == 0);
#endif
  return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
  // actual size to allocate(not including meta)
  if (size == 0) {
    return NULL;
  }
  const size_t actual =
      ALIGN(size) >= free_meta_sz() ? ALIGN(size) : free_meta_sz();

  // look up the free list by size
  void *res;
  if (size < 32U) {
    // look up order: small, middle, large.
    res = find_fit(mm_small, actual);
    if (res != MMEOL) {
      // found
      take(res, actual);
      check();
      return res + used_meta_sz();
    }

    res = find_fit(mm_middle, actual);
    if (res != MMEOL) {
      take(res, actual);
      check();
      return res + used_meta_sz();
    }

    res = find_fit(mm_large, actual);
    if (res != MMEOL) {
      take(res, actual);
      check();
      return res + used_meta_sz();
    }

    // must allocate by growing the heap
    int grow = grow_heap(actual + used_meta_sz());
    if (grow != 0) {
      return MMEOL;
    }
    res = end_blk;
    take(end_blk, actual);
    check();
    return res + used_meta_sz();
  } else {
    if (size < 1024U) {
      res = find_fit(mm_middle, actual);
      if (res != MMEOL) {
        take(res, actual);
        check();
        return res + used_meta_sz();
      }
      res = find_fit(mm_large, actual);
      if (res != MMEOL) {
        take(res, actual);
        check();
        return res + used_meta_sz();
      }

      int grow = grow_heap(actual + used_meta_sz());
      if (grow != 0) {
        return MMEOL;
      }
      res = end_blk;
      take(end_blk, actual);
      check();
      return res + used_meta_sz();
    } else {
      res = find_fit(mm_large, actual);
      if (res != MMEOL) {
        take(res, actual);
        check();
        return res + used_meta_sz();
      }

      int grow = grow_heap(actual + used_meta_sz());
      if (grow != 0) {
        return MMEOL;
      }
      res = end_blk;
      take(end_blk, actual);
      check();
      return res + used_meta_sz();
    }
  }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) {
  if (ptr == NULL || ptr == (void *)(-1)) {
    return;
  }
  void *blk = ptr - used_meta_sz();
  struct free_meta *meta = static_cast(blk, struct free_meta *);
#ifdef DEBUG
  // make sure that you're not freeing a block that is free.
  // printf("%u %u \n", meta->size_, meta->last_);
  assert((meta->size_ & 0x7) != 0);
#endif
  // mark as free block.
  meta->size_ = meta->size_ & (~0x7);
  // meta of front block in the heap.
  struct free_meta *last_meta = static_cast(meta->last_, struct free_meta *);
  // meta of next block in the heap.
  struct free_meta *next_meta =
      blk == end_blk ? MMEOL
                     : static_cast(blk + meta->size_, struct free_meta *);

  // result block(to be added to free list)
  void *res = blk;
  if (next_meta != MMEOL && (next_meta->size_ & 0x7) == 0) {
    // next block is not null and free! you should merge them.
    remove_free_blk(static_cast(next_meta, void *));
    merge_blk(blk, static_cast(next_meta, void *));
  }

  if (last_meta != MMEOL && (last_meta->size_ & 0x7) == 0) {
    // last block is not null and free! you should merge them.
    remove_free_blk(static_cast(last_meta, void *));
    merge_blk(static_cast(last_meta, void *), blk);
    res = static_cast(last_meta, void *);
  }

  add_free_blk(res);
  check();
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
 * Helper Functions Implementation
 ************************************************/
void *find_fit(void *head, size_t size) {
  void *it = head;
  struct free_meta *meta;
  size_t volume;

  // recall: free block layout [size | last | pred | succ ]
  while (it != MMEOL) {
    meta = static_cast(it, struct free_meta *);
    volume = meta->size_;
#ifdef DEBUG
    // on the free list: unused and aligned.
    assert((volume & 0x7) == 0);
#endif
    if (volume >= size + used_meta_sz()) {
      // found!
      return it;
    }

    // else it = it->succ;
    it = static_cast(meta->succ_, void *);
  }

#ifdef DEBUG
  assert(it == MMEOL);
#endif
  return it;
}

void take(void *node, size_t aligned) {
  struct free_meta *meta = static_cast(node, struct free_meta *);
  // NOTE: Your solution should gurantee that there's no continuous free blocks.
  // And you can also assume this fact in your impl.
#ifdef DEBUG
  // the size is aligned
  assert((aligned & 0x7) == 0);
  // the size is non-zero
  assert(aligned > 0U);
  // node is valid
  assert(node && node != (void *)-1);
  // volume is avaliable and enough
  assert((meta->size_ & 0x7) == 0);
  assert(meta->size_ >= aligned + used_meta_sz());
#endif

  /**
   * Recall: layout of free block: [size | last | pred | succ]
   */
  struct free_meta *predecessor = static_cast(meta->pred_, struct free_meta *);
  struct free_meta *successor = static_cast(meta->succ_, struct free_meta *);
  struct free_meta *third = static_cast(node + meta->size_, struct free_meta *);
  // evict off the free list(won't affect end_blk)
  if (predecessor != NULL) {
    predecessor->succ_ = static_cast(successor, size_t);
  }
  if (successor != NULL) {
    successor->pred_ = static_cast(predecessor, size_t);
  }
  if (node == mm_large) {
    mm_large = static_cast(successor, void *);
  }
  if (node == mm_small) {
    mm_small = static_cast(successor, void *);
  }
  if (node == mm_middle) {
    mm_middle = static_cast(successor, void *);
  }

  size_t remain = meta->size_ - aligned - used_meta_sz();
  meta->size_ |= 0x1;

#ifdef DEBUG
  assert((meta->size_ & 0x7) != 0);
#endif
  meta->pred_ = meta->succ_ = 0;
  if (remain < free_meta_sz() + MINVOL) {
    // too small, don't mind. have to allocate all of them to the request.
    return;
  }
  // else, split the node and reuse the rest.

  // set the size of meta; mark as used.
  meta->size_ = aligned + used_meta_sz();
  meta->size_ |= 0x1;
  // rest of the block(free)
  void *rest = node + (aligned + used_meta_sz());
  struct free_meta *rest_meta = static_cast(rest, struct free_meta *);
  rest_meta->size_ = remain;
  rest_meta->last_ = static_cast(node, size_t);
  rest_meta->pred_ = rest_meta->succ_ = 0;
  if (node == end_blk) {
    // end_blk should change.
    end_blk = rest;
#ifdef DEBUG
    assert(end_blk + rest_meta->size_ == mem_heap_hi() + 1);
#endif
  } else {
    // should set the pointer of the third node!
    third->last_ = static_cast(rest, size_t);
  }
  // add_free_blk will handle its predecessor and successor.
  add_free_blk(rest);

  // now you can give node + used_meta_sz() to user, good luck!
}

void add_free_blk(void *blk) {
  // won't change the end_blk pointer.
  struct free_meta *meta = static_cast(blk, struct free_meta *);
#ifdef DEBUG
  assert(blk != NULL);
  assert((meta->size_ & 0x7) == 0);
  // meta's size is larger than the block?? Impossible!
  assert(meta->size_ >= free_meta_sz());
#endif
  meta->pred_ = 0;
  if (meta->size_ - free_meta_sz() < 32U) {
    // add to small list
    meta->succ_ = static_cast(mm_small, size_t);
    struct free_meta *m = static_cast(mm_small, struct free_meta *);
    if (m != NULL) {
      m->pred_ = static_cast(blk, size_t);
    }
    mm_small = blk;
  } else {
    if (meta->size_ - free_meta_sz() < 1024U) {
      meta->succ_ = static_cast(mm_middle, size_t);
      struct free_meta *m = static_cast(mm_middle, struct free_meta *);
      if (m != NULL) {
        m->pred_ = static_cast(blk, size_t);
      }
      mm_middle = blk;
    } else {
      meta->succ_ = static_cast(mm_large, size_t);
      struct free_meta *m = static_cast(mm_large, struct free_meta *);
      if (m != NULL) {
        m->pred_ = static_cast(blk, size_t);
      }
      mm_large = blk;
    }
  }
}

int grow_heap(size_t bytes) {
  if (bytes == 0) {
    return 0;
  }
#ifdef DEBUG
  // test bytes is aligned.
  assert((bytes & 0x7) == 0);
#endif
  void *new_blk = NULL;
  struct free_meta *end_meta = static_cast(end_blk, struct free_meta *);
  if ((end_meta->size_ & 0x7) == 0) {
#ifdef DEBUG
    // the stupid programmer may have assumed that space's not enough.
    assert(bytes >= end_meta->size_);
#endif
    // this is a free block! Have to merge the two blocks
    new_blk = mem_sbrk(bytes - end_meta->size_);
    if (new_blk == (void *)-1) {
      return -1;
    }
#ifdef DEBUG
    // this should be true, cause end_blk is the last block.
    assert(end_blk + end_meta->size_ == new_blk);
#endif
    end_meta->size_ = bytes;
  } else {
    // should allocate bytes.
    new_blk = mem_sbrk(bytes);
    if (new_blk == (void *)-1) {
      return -1;
    }
#ifdef DEBUG
    // this should be true, cause end_blk is the last block.
    assert(end_blk + (end_meta->size_ & (~0x7)) == new_blk);
#endif
    // this is a used block:)
    struct free_meta *meta = static_cast(new_blk, struct free_meta *);
    meta->size_ = bytes;
    meta->pred_ = meta->succ_ = 0;
    meta->last_ = static_cast(end_blk, size_t);
    end_blk = new_blk;
  }

  return 0;
}

/**
 * @param min_sz minimal size of block in the free list
 * @param max_sz maximal size of block in the free list
 * @return non zero if the free list is inconsistent
 */
int check_free_lst(void *head, const char *lst_name, size_t min_sz,
                   size_t max_sz) {
  min_sz += free_meta_sz();
  max_sz += free_meta_sz();
  if (head == NULL) {
    return 0;
  }
  void *it1 = head;
  struct free_meta *m1 = static_cast(it1, struct free_meta *);
  if (m1->pred_ != 0) {
    fprintf(stderr, "In %s: first node's predecessor is not null\n", lst_name);
    return -1;
  }
  if ((m1->size_ & 0x7) != 0) {
    fprintf(stderr, "In %s: first node's not free\n", lst_name);
    return -1;
  }
  if (m1->size_ >= max_sz || m1->size_ < min_sz) {
    fprintf(
        stderr,
        "In %s, got a block that has size %ul, expected in range (%ul, %ul).\n",
        lst_name, m1->size_, min_sz, max_sz);
    return -1;
  }
  void *it2 = static_cast(m1->succ_, void *);
  struct free_meta *m2 = static_cast(it2, struct free_meta *);

  while (it2 != NULL) {
    // check prev, succ link.
    if ((m2->size_ & 0x7) != 0) {
      fprintf(stderr, "In %s, have non-free block\n", lst_name);
      return -1;
    }

    if (m2->size_ >= max_sz || m2->size_ < min_sz) {
      fprintf(stderr,
              "In %s, got a block that has size %ul, expected in range (%ul, "
              "%ul).\n",
              lst_name, m2->size_, min_sz, max_sz);
      return -1;
    }

    if (m2->pred_ != static_cast(it1, size_t)) {
      fprintf(stderr, "In %s, predecessor is errorneous\n", lst_name);
      return -1;
    }

    it1 = it2;
    it2 = static_cast(m2->succ_, void *);
    m1 = static_cast(it1, struct free_meta *);
    m2 = static_cast(it2, struct free_meta *);
  }

  return 0;
}

int mm_check() {
  int res;

  // check rule 0: end_blk is pointing to the end.
  struct free_meta *end = static_cast(end_blk, struct free_meta *);
  if (end_blk == NULL) {
    fprintf(stderr, "end_blk is null?? Impossible!\n");
    goto bad;
  }
  if (end_blk + (end->size_ & (~0x7)) != mem_heap_hi() + 1) {
    fprintf(stderr, "end_blk is pointing to erroroues block\n");
    goto bad;
  }

  // check rule 1: the free list is consistent
  res = check_free_lst(mm_small, "mm_small", 0, 32U);
  if (res != 0) {
    goto bad;
  }
  res = check_free_lst(mm_middle, "mm_middle", 32U, 1024U);
  if (res != 0) {
    goto bad;
  }
  res = check_free_lst(mm_large, "mm_large", 1024U,
                       static_cast(-1, size_t) - free_meta_sz());
  if (res != 0) {
    goto bad;
  }

  return 0;
bad:
  return -1;
}

// again, won't affect end_blk! Don't worry~
void remove_free_blk(void *blk) {
  struct free_meta *meta = static_cast(blk, struct free_meta *);
#ifdef DEBUG
  // is the node null?
  assert(blk != MMEOL);
  // is the node free?
  assert((meta->size_ & 0x7) == 0);
#endif
  struct free_meta *pred_meta = static_cast(meta->pred_, struct free_meta *);
  struct free_meta *succ_meta = static_cast(meta->succ_, struct free_meta *);
  if (pred_meta != NULL) {
    pred_meta->succ_ = static_cast(succ_meta, size_t);
  }
  if (succ_meta != NULL) {
    succ_meta->pred_ = static_cast(pred_meta, size_t);
  }

  // what is blk is one of the mm_small, mm_middle, mm_large?
  if (blk == mm_small) {
    mm_small = static_cast(succ_meta, void *);
  }
  if (blk == mm_middle) {
    mm_middle = static_cast(succ_meta, void *);
  }
  if (blk == mm_large) {
    mm_large = static_cast(succ_meta, void *);
  }

  // remove the tag associated with meta.
  meta->succ_ = meta->pred_ = 0;
}

void merge_blk(void *left, void *right) {
  struct free_meta *left_mt = static_cast(left, struct free_meta *);
  struct free_meta *right_mt = static_cast(right, struct free_meta *);
#ifdef DEBUG
  // is left and right not null?
  assert(left != NULL && right != NULL);
  // is left and right free?
  assert((left_mt->size_ & 0x7) == 0);
  assert((right_mt->size_ & 0x7) == 0);
  // is left adjacent to right?
  assert(right_mt->last_ == static_cast(left, size_t));
  assert(left + left_mt->size_ == right);
#endif
  assert(left != end_blk);
  struct free_meta *third_mt =
      static_cast(right + right_mt->size_, struct free_meta *);
  if (right != end_blk) {
    // must change the last of third.
    third_mt->last_ = static_cast(left, size_t);
  } else {
    // set end_blk to be left.
    end_blk = left;
  }
  left_mt->size_ += right_mt->size_;
}

/**
 * One more thing, as a kind reminder:
 * DON'T be confused by #ifdef DEBUG ... #endif
 *
 * If you compile with switch -DDEBUG, these code will be executed to ensure
 * correctness. When you are pretty sure that your code is correct, you can
 * safely compile without -DDEBUG, and those code snippets between #ifdef and
 * #endif will not be compiled at all.
 */
