#include "cachelab.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
/**
 * NOTE:
 * Address layout: [tag | set | block]
 * Cache line layout: [valid bit | tag | blocks]
 */

struct cache_meta {
  unsigned s; // set bit, detemine the set to look at.
  unsigned b; //
  unsigned E; // number of lines of each set.
  unsigned t; // tag bits.
  char *filename;
  short verbose; // under verbose mode??
} meta;
// for debugging
void printmeta();

// parse arguments
#include <string.h>
#define SUCC 0 // success code
#define FAIL 1 // parsing failure
#define HELP 2 // user is asking for help.
int parse_args(int argc, char **argv);

// cache statistics
typedef struct cache_stat {
  unsigned evict_;
  unsigned hit_;
  unsigned miss_;
  unsigned long stamp_; // time stamp;
} cache_stat;
// print statistics
void print_stat(cache_stat *stat);

// cache visit
enum visit_tp { Load = 0, Store, Modify };
// visit the cache
typedef struct cache_visit {
  enum visit_tp type_; // type of visiting
  unsigned long tag_;  // tag
  unsigned long s;
} cache_visit;
void printvisit(cache_visit *);

// cache line
typedef struct cache_line {
  short valid;         // valid bit
  unsigned long tag;   // tag bits
  unsigned long stamp; // maintain a time stamp.
} cache_line;

// cache set:
typedef struct cache_set {
  cache_line *lines_;
} cache_set;
// initialize a cache set.
void cache_set_init(cache_set *);
// release a cache set.
void cache_set_release(cache_set *);

// cache simulation:)
typedef struct cache {
  unsigned S;       // number of sets.
  cache_set *sets_; // total number of sets is given by 1 << (meta.s).
} cache;
// initialize the cache;
void cache_init(cache *);
// release the memory held by cache.
void cache_release(cache *);
// deal with memory access
void deal(cache *, cache_visit *, cache_stat *);

int main(int argc, char **argv) {
#ifdef DEBUG
#ifndef static_assert
#define static_assert(pred)                                                    \
  switch (0) {                                                                 \
  case (0):                                                                    \
  case (pred):                                                                 \
  }
#endif
  static_assert(sizeof(long) == 8U);
  static_assert(sizeof(int) == 4U);
#endif
  // printSummary(0, 0, 0);

  // Scenario: parse command line arguments.
  meta.filename = NULL;
  int stat = parse_args(argc, argv);
  if (stat == HELP) {
    return 0;
  }
  if (stat == FAIL) {
    return FAIL;
  }
  assert(meta.filename);
  // for degugging, check meta has been set correctly.
#ifdef DEBUG
  printmeta();
  printf("\n");
#endif

  cache cache_;
  cache_stat stats;
  stats.hit_ = 0U;
  stats.miss_ = 0U;
  stats.evict_ = 0U;
  stats.stamp_ = 0UL;
#ifdef DEBUG
  print_stat(&stats);
  printf("\n");
#endif

  FILE *mem = fopen(meta.filename, "r");
  if (!mem) {
    printf("cannot open %s, abort\n", meta.filename);
    return FAIL;
  }
  cache_init(&cache_);

  char line[64];
  int c;
  while (1) {
    for (int i = 0; (c = fgetc(mem)) != '\n'; ++i) {
      if (c == EOF) {
        goto end;
      }
      line[i] = c;
    }
    // parse the line.
    if (line[0] != ' ') {
      continue;
    }
    cache_visit visit;
    switch (line[1]) {
    case 'M':
      visit.type_ = Modify;
      break;
    case 'L':
      visit.type_ = Load;
      break;
    case 'S':
      visit.type_ = Store;
      break;
    default:
      break;
    }
    unsigned long addr = 0UL; // address to visit
    if (meta.verbose) {
      putchar(line[1]);
      putchar(line[2]);
    }
    for (int i = 3; line[i] != ','; ++i) {
      if (meta.verbose) {
        putchar(line[i]);
      }
      addr <<= 4UL;
      addr |= (unsigned long)(line[i] < 'a' ? (line[i] - '0')
                                            : (line[i] - 'a' + 0xa));
    }
    if (meta.verbose) {
      printf(",1");
    }

    visit.s = (addr << meta.t);
    visit.s >>= (meta.t + meta.b);
    visit.tag_ = addr >> (meta.s + meta.b);

#ifdef DEBUG // if you want to debug the whole program, add -DDEBUG to compiler
             // switches.
    printf("addr is %lu\n", addr);
    printvisit(&visit);
    printf("\n");
#endif
    deal(&cache_, &visit, &stats);
    if (meta.verbose) {
      putchar('\n');
    }
  }
end:

  print_stat(&stats);
  cache_release(&cache_);
  return 0;
}

/************************************************
 *           Cache Set Utilities
 ***********************************************/
void cache_set_init(cache_set *set) {
  set->lines_ = (cache_line *)(malloc(sizeof(cache_line) * meta.E));
  assert(set->lines_); // bad alloc
  for (int i = 0; i < meta.E; ++i) {
    set->lines_[i].valid = 0;
  }
}
void cache_set_release(cache_set *set) { free(set->lines_); }
/**
 * @brief Find a empty line in the set, if full, evict and then get one!
 * The eviction will be recorded in statistics if happened.
 * @return the empty line id.
 */
unsigned fetch(cache_set *set, cache_stat *stat) {
  // implement LRU replacement policy
  unsigned long minstamp = 0xffffffffffffffffUL;
  unsigned idx = meta.E << 1U;
  for (unsigned i = 0; i < meta.E; ++i) {
    if (!set->lines_[i].valid) { // always use empty slot first!
      idx = i;
      break;
    }
    if (set->lines_[i].stamp < minstamp) {
      minstamp = set->lines_[i].stamp;
      idx = i;
    }
  }

  assert(idx < meta.E);
  if (set->lines_[idx].valid) {
    // eviction happened!
    stat->evict_ += 1U;
    if (meta.verbose) {
      printf(" eviction");
    }
  }
  set->lines_[idx].valid = 1;
  return idx;
}

// deal with cache load requests
void deal_load(cache_set *set, cache_visit *visit, cache_stat *stat) {
  for (unsigned i = 0; i < meta.E; ++i) {
    if (set->lines_[i].valid && set->lines_[i].tag == visit->tag_) {
      // Nice, We find it!
      stat->hit_ += 1U;
      if (meta.verbose) {
        printf(" hit");
      }
      set->lines_[i].stamp = stat->stamp_;
      return;
    }
  }

  // Oops, miss
  if (meta.verbose) {
    printf(" miss");
  }
  stat->miss_ += 1U;
  unsigned idx = fetch(set, stat);
#ifdef DEBUG
  printf("\nidx found is %u\n", idx);
#endif
  // maintain the valid, tag and stamp;
  set->lines_[idx].stamp = stat->stamp_;
  set->lines_[idx].tag = visit->tag_;
}

// deal with memory store requests
void deal_store(cache_set *set, cache_visit *visit, cache_stat *stat) {
  // almost identical to deal_load...
  deal_load(set, visit, stat);
}

/************************************************
 *          Cache Visit Utilities
 ***********************************************/
void printvisit(cache_visit *v) {
  if (v == NULL) {
    printf("(null)\n");
    return;
  }
  switch (v->type_) {
  case Modify:
    printf("Modify ");
    break;
  case Load:
    printf("Load ");
    break;
  case Store:
    printf("Store ");
    break;
  default:
    break;
  }
  printf("s field: %lu, t field: %lu\n", v->s, v->tag_);
}

/************************************************
 *             Cache Utilities
 ***********************************************/
void cache_init(cache *ch) {
  ch->S = 1U << meta.s; // S = 2^s
  ch->sets_ = (cache_set *)(malloc(sizeof(cache_set) * (1U << meta.s)));
  assert(ch->sets_);
  for (unsigned i = 0; i < ch->S; ++i) {
    cache_set_init(ch->sets_ + i);
  }
}
void cache_release(cache *ch) {
  for (unsigned i = 0U; i < ch->S; ++i) {
    cache_set_release(ch->sets_ + i);
  }
  free(ch->sets_);
}
void deal(cache *ch, cache_visit *visit, cache_stat *stats) {
  assert(ch);
  assert(visit); // both must not be nullptr

  // find the set to investingate.
  assert(visit->s < ch->S);
  cache_set *set = ch->sets_ + visit->s;
  switch (visit->type_) {
  case Load: {
    deal_load(set, visit, stats);
    stats->stamp_ += 1UL;
    break;
  }
  case Store: {
    deal_store(set, visit, stats);
    stats->stamp_ += 1UL;
    break;
  }
  case Modify: {
    deal_load(set, visit, stats);
    // should be treated as one visit.
    deal_store(set, visit, stats);
    stats->stamp_ += 1UL;
    break;
  }
  default:
    assert(NULL); // internal error, check your code:)
  }
}

/************************************************
 *           Cache Stat Utilities
 ***********************************************/
// void printSummary(int h, int m, int e) {
//   printf("hits: %d misses: %d evictions: %d\n", h, m, e);
// }
void print_stat(cache_stat *stat) {
  assert(stat);
  printSummary(stat->hit_, stat->miss_, stat->evict_);
}

/************************************************
 *           Cache Meta Utilities
 ***********************************************/
void printmeta() {
  printf("s = %u, b = %u, E = %u, t = %u\n", meta.s, meta.b, meta.E, meta.t);
  printf("Target file: %s\n", meta.filename);
  if (meta.verbose) {
    printf("verbose: True\n");
  } else {
    printf("verbose: False\n");
  }
}
int parse_args(int argc, char **argv) {
  if (argc < 2) {
    // no param provided!
    printf("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
    return HELP;
  }
  meta.s = 0U;
  meta.b = 0U;
  meta.E = 0U;
  meta.t = 0U;

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-v") == 0) {
      meta.verbose = 1;
      continue;
    }

    if (strcmp(argv[i], "-h") == 0) {
      printf("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
      return HELP;
    }

    if (strcmp(argv[i], "-s") == 0) {
      if (i + 1 >= argc) {
        printf("No. you have not provide -s. Abort\n");
        return FAIL;
      }
      meta.s = atoi(argv[++i]);
      continue;
    }

    if (strcmp(argv[i], "-E") == 0) {
      if (i + 1 >= argc) {
        printf("No. you have not provide -E. Abort\n");
        return FAIL;
      }
      meta.E = atoi(argv[++i]);
      continue;
    }

    if (strcmp(argv[i], "-b") == 0) {
      if (i + 1 >= argc) {
        printf("No. you have not provide -b. Abort\n");
        return FAIL;
      }
      meta.b = atoi(argv[++i]);
      continue;
    }

    if (strcmp(argv[i], "-t") == 0) {
      // file name!
      if (i + 1 >= argc) {
        printf("No. you have not provide -t. Abort\n");
        return FAIL;
      }
      meta.filename = argv[++i];
      continue;
    }
  }
  // for the present assume E, s, b will be non-zero...
  meta.t = 64U - meta.b - meta.s;
  return SUCC;
}