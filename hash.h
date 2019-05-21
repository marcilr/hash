/*
 * hash.h
 * Header file for hash ADT.
 */

#ifndef HASH_H
#define HASH_H

/*
 * Utilization Factor
 * If the hash table has better than
 * the following percentage of utilization
 * rehashing will occur.
 */
#define MAX_UTILIZATION .80

/*
 * This is an array of primes. We grow the table rapidly
 * initially to avoid a lot of rehashing, then just double
 * the values.  The small sizes should rehash quickly and
 * large rehashes shouldn't happen often.
 *
 * Note these are in a sense magic numbers as the largest
 * hash possible is the largest number is the table.
 * However, it doesn't make sense to waste time calculating
 * large primes on the fly. The size range is 13 to roughly
 * 130 million, which should be sufficient for most purposes.
 */

static unsigned int sizes[] = {
    13,
    127,
    1021,
    8191,
    16381,
    32749,
    65521,
    131071,
    262139,
    524287,
    1048573,
    2097143,
    4194301,
    8388593,
    16777213,
    33554393,
    67108859,
    134217689,
    0
};


/* Hash node definition */
struct HashNode {
  struct HashNode *next;
  void   *data;
  char   *vkey;
} HashNode;

/* Hash table */
/* typedef struct HashNode *Hash; */

typedef struct Hash {

  unsigned int          num_buckets;
  unsigned int          count;
  struct   HashNode     **array;

} Hash;


/* ============== public functions ================ */
/* ============== public functions ================ */

Hash *hashCreate(unsigned int num_buckets);
int hashAdd(Hash *hash, char *vkey, void *data);
void *hashGet(Hash *hash, char *vkey);
void hashDestroy(Hash *hash, void (*destructor)(void *data));
void hashPrint(Hash *hash, void (*printer)(void *data));
unsigned int hashCount(Hash *hash);
unsigned int hashSize(Hash *hash);

#endif
