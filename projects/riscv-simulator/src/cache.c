#include "dogfault.h"
#include "cache.h"
#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "config.h"

// Milestone 4: L2 geometry. Fallback defines so cache.c (a graded file)
// also compiles against the unmodified framework cache.h; our local cache.h
// defines the same values, in which case these are skipped.
#ifndef L2_CACHE_HIT_LATENCY
#define L2_CACHE_HIT_LATENCY 10 // extra cycles to service an L1 miss that hits in L2
#endif
#ifndef L2_CACHE_SET_BITS
#define L2_CACHE_SET_BITS 6      // number of L2 sets (2^L2_CACHE_SET_BITS)
#endif
#ifndef L2_CACHE_LINES_PER_SET
#define L2_CACHE_LINES_PER_SET 8 // L2 associativity
#endif
#ifndef L2_CACHE_BLOCK_BITS
#define L2_CACHE_BLOCK_BITS 6    // L2 block size (2^L2_CACHE_BLOCK_BITS)
#endif

// zeroes out the byte-offset bits to get the block's base address
unsigned long long address_to_block(const unsigned long long address, const Cache *cache) {
  return (address >> cache->blockBits) << cache->blockBits;
}

// bits above the block offset and set index
unsigned long long cache_tag(const unsigned long long address, const Cache *cache) {
  return address >> (cache->blockBits + cache->setBits);
}

unsigned long long cache_set(const unsigned long long address, const Cache *cache) {
  if (cache->setBits == 0) return 0; // fully associative -> always set 0
  unsigned long long mask = (1ULL << cache->setBits) - 1;
  return (address >> cache->blockBits) & mask;
}

// true if address is already cached
bool probe_cache(const unsigned long long address, const Cache *cache) {
  unsigned long long idx = cache_set(address, cache);
  unsigned long long tag = cache_tag(address, cache);
  Set *set = &cache->sets[idx];
  for (int i = 0; i < cache->linesPerSet; i++) {
    if (set->lines[i].valid && set->lines[i].tag == tag) {
      return true;
    }
  }
  return false;
}

// updates LRU/LFU bookkeeping on a hit
void hit_cacheline(const unsigned long long address, Cache *cache) {
  unsigned long long idx = cache_set(address, cache);
  unsigned long long tag = cache_tag(address, cache);
  Set *set = &cache->sets[idx];
  for (int i = 0; i < cache->linesPerSet; i++) {
    if (set->lines[i].valid && set->lines[i].tag == tag) {
      set->lines[i].lru_clock = set->lru_clock;
      set->lines[i].access_counter++;
      return;
    }
  }

  assert(0); // should always find it here
}

// tries to place address in an empty line; returns false if the set is full
bool insert_cacheline(const unsigned long long address, Cache *cache) {
  unsigned long long idx = cache_set(address, cache);
  Set *set = &cache->sets[idx];
  for (int i = 0; i < cache->linesPerSet; i++) {
    if (!set->lines[i].valid) {
      set->lines[i].valid = true;
      set->lines[i].tag = cache_tag(address, cache);
      set->lines[i].block_addr = address_to_block(address, cache);
      set->lines[i].lru_clock = set->lru_clock;
      set->lines[i].access_counter = 1;
      return true;
    }
  }
  return false;
}

// picks an eviction victim: LRU takes the oldest access, LFU takes the
// least-used line (ties broken by LRU)
unsigned long long victim_cacheline(const unsigned long long address, const Cache *cache) {
  unsigned long long idx = cache_set(address, cache);
  Set *set = &cache->sets[idx];
  int victim_i = 0;

  if (!cache->lfu) { // LRU
    for (int i = 1; i < cache->linesPerSet; i++) {
      if (set->lines[i].lru_clock < set->lines[victim_i].lru_clock) {
        victim_i = i;
      }
    }
  } else { // LFU
    for (int i = 1; i < cache->linesPerSet; i++) {
      if (set->lines[i].access_counter < set->lines[victim_i].access_counter ||
          (set->lines[i].access_counter == set->lines[victim_i].access_counter &&
           set->lines[i].lru_clock < set->lines[victim_i].lru_clock)) {
        victim_i = i;
      }
    }
  }

  return set->lines[victim_i].block_addr;
}

// evicts the victim line and inserts the new address in its place
void replace_cacheline(const unsigned long long victim_block_addr, const unsigned long long insert_addr, Cache *cache) {
  unsigned long long idx = cache_set(insert_addr, cache);
  Set *set = &cache->sets[idx];
  for (int i = 0; i < cache->linesPerSet; i++) {
    if (set->lines[i].valid && set->lines[i].block_addr == victim_block_addr) {
      set->lines[i].tag = cache_tag(insert_addr, cache);
      set->lines[i].block_addr = address_to_block(insert_addr, cache);
      set->lines[i].lru_clock = set->lru_clock;
      set->lines[i].access_counter = 1;
      return;
    }
  }

  assert(0); // should always find the victim here
}

// Milestone 4: shared setup routine, so both the default L1 geometry and the bigger L2 geometry can reuse it
void cacheSetUpCustom(Cache *cache, char *name, int setBits, int linesPerSet, int blockBits) {
  // riscv.c calls this on an uninitialized struct, so configure it here
  cache->setBits      = setBits;
  cache->linesPerSet  = linesPerSet;
  cache->blockBits    = blockBits;
  cache->lfu          = CACHE_LFU;
  cache->displayTrace = CACHE_DISPLAY_TRACE;

  cache->hit_count = 0;
  cache->miss_count = 0;
  cache->eviction_count = 0;

  int numSets = 1 << cache->setBits;
  cache->sets = malloc(numSets * sizeof(Set));

  for (int s = 0; s < numSets; s++) {
    cache->sets[s].lru_clock = 0;
    cache->sets[s].lines = calloc(cache->linesPerSet, sizeof(Line)); // zeroed = all invalid
  }

  cache->name = strdup(name);
}

void cacheSetUp(Cache *cache, char *name) {
  // default L1 geometry, used for both the single-level cache and the L1-I/L1-D caches
  cacheSetUpCustom(cache, name, CACHE_SET_BITS, CACHE_LINES_PER_SET, CACHE_BLOCK_BITS);
}

void cacheSetUpL2(Cache *cache, char *name) {
  // Milestone 4: L2 is bigger than L1, so it uses the L2 geometry instead
  cacheSetUpCustom(cache, name, L2_CACHE_SET_BITS, L2_CACHE_LINES_PER_SET, L2_CACHE_BLOCK_BITS);
}

void deallocate(Cache *cache) {
  int numSets = 1 << cache->setBits;
  for (int s = 0; s < numSets; s++) {
    free(cache->sets[s].lines);
  }
  free(cache->sets);
  free(cache->name);
}

// looks up address: hit, miss (empty line found), or eviction (set full,
// replace via LRU/LFU); updates counters and returns the outcome
result operateCache(const unsigned long long address, Cache *cache) {
  result r;

  Set *set = &cache->sets[cache_set(address, cache)];
  set->lru_clock++;

  if (probe_cache(address, cache)) {
    hit_cacheline(address, cache);
    r.status = CACHE_HIT;
    cache->hit_count++;
    #ifdef PRINT_CACHE_TRACES
    printf(CACHE_HIT_FORMAT, address);
    #endif
    return r;
  }

  if (insert_cacheline(address, cache)) {
    r.status = CACHE_MISS;
    r.insert_block_addr = address_to_block(address, cache);
    cache->miss_count++;
    #ifdef PRINT_CACHE_TRACES
    printf(CACHE_MISS_FORMAT, address);
    #endif
    return r;
  }

  unsigned long long victim = victim_cacheline(address, cache);
  replace_cacheline(victim, address, cache);
  r.status = CACHE_EVICT;
  r.victim_block_addr = victim;
  r.insert_block_addr = address_to_block(address, cache);
  cache->miss_count++;
  cache->eviction_count++;
  #ifdef PRINT_CACHE_TRACES
  printf(CACHE_EVICTION_FORMAT, address);
  #endif
  return r;
}

// bridges the pipeline's 32-bit address to operateCache's 64-bit interface,
// and maps the hit/miss/eviction status to a cycle latency
int processCacheOperation(unsigned long address, Cache *cache) {
  result r = operateCache((unsigned long long)address, cache);

  if (r.status == CACHE_HIT) {
    return CACHE_HIT_LATENCY;
  } else if (r.status == CACHE_MISS) {
    return CACHE_MISS_LATENCY;
  } else {
    return CACHE_OTHER_LATENCY;
  }
}

// Milestone 4: checks L1 first, and only checks L2 on an L1 miss, returning the combined cycle latency
int processTwoLevelCacheOperation(unsigned long address, Cache *l1, Cache *l2) {
  result l1_r = operateCache((unsigned long long)address, l1);
  if (l1_r.status == CACHE_HIT) {
    return CACHE_HIT_LATENCY;
  }

  // L1 missed, so the access falls through to L2
  result l2_r = operateCache((unsigned long long)address, l2);
  if (l2_r.status == CACHE_HIT) {
    return CACHE_HIT_LATENCY + L2_CACHE_HIT_LATENCY;
  }

  // both L1 and L2 missed, so the access has to go all the way to memory
  return CACHE_HIT_LATENCY + L2_CACHE_HIT_LATENCY + MEM_LATENCY;
}
