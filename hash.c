/*
 * hash.c
 * Created Wed Aug  7 13:15:06 AKDT 2002
 *   by Raymond E. Marcil <marcilr@rockhounding.net>
 *
 * This is a generic Hash ADT
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

/* =============== Private Function Prototypes ================*/
/* =============== Private Function Prototypes ================*/

static
unsigned int hash_fval_string(const void *vkey,
			      unsigned int num_buckets);

static
void hashFreeNode(struct HashNode *hashNodePtr,
                  void (*destructor)(void *data));

static
int hashArrayInsert(struct HashNode **hashArray,
                    struct HashNode *hashNode,
                    unsigned int num_buckets);
static
void hashRehash(Hash *hash, unsigned int num_buckets);

static
unsigned int hashPrime(unsigned int value);

static
void freemem(void *mem);

/* =================== Public Functions ====================== */
/* =================== Public Functions ====================== */

/*
 * hashCreate()
 * This function creates a new hash table.
 * The num_buckets is a hint for the relative number
 * of initial buckets.  The actual number of buckets will be
 * the next largest number from the sizes[] array in hash.h
 * The hash table should be about twice the size of the maximum
 * number of anticipate entrys.
 *
 * INPUT:     num_buckets     Number of hash table entries.
 * RETURNS:   hash            Pointer to new hash table
 *            NULL            Error allocating memory
 */

Hash *hashCreate(unsigned int num_buckets){

  Hash *hash;
  unsigned int  bucket_count;

  hash = (Hash *) malloc (sizeof(Hash));

  bucket_count = hashPrime(num_buckets);

  hash->num_buckets = bucket_count;
  hash->count       = 0;

  /*
   * Allocate space for hash table
   * This is an array of pointers to hash nodes.
   */
  hash->array = (struct HashNode **) malloc ((bucket_count) *
                                             sizeof (struct HashNode *));

  /* Initialize array pointers to null */
  memset(hash->array, 0 ,bucket_count * sizeof (struct HashNode *));

  /* Return pointer to hash table */
  return hash;
}


/* ====================== hashCount() ======================== */
/* ====================== hashCount() ======================== */

/*
 * hashCount()
 * This function returns the number of nodes
 * in the hash table.
 *
 * INPUT:      Hash            Hash table
 * RETURNS:    unsigned int    Number of nodes in hash table
 */
unsigned int hashCount(Hash *hash){
  return (hash->count);
}


/* ======================= hashSize() ======================== */
/* ======================= hashSize() ======================== */

/*
 * hashSize()
 * This function returns the size of the hash table
 *
 * INPUT:      Hash            Hash table
 * RETURNS:    unsigned int    Size of hash table
 */
unsigned int hashSize(Hash *hash){
  return (hash->num_buckets);
}



/* ===================== hashDestroy() ======================= */
/* ===================== hashDestroy() ======================= */

/*
 * hashDestroy()
 * This function destroys the hash table.
 * The function requires a destructor function
 * which deallocates the data container.
 *
 * INPUT:    hash         Pointer to hash table
 *           destructor   Function pointer to destructor
 * RETURNS:  None.
 */
void hashDestroy(Hash *hash, void (*destructor)(void *data)){

  unsigned int      i;
  struct HashNode   *hashNodePtr;
  struct HashNode   *tmp;

  /* Loop through hash table and deallocate buckets */
  for (i = 0; i < hash->num_buckets; i++){

    /* Get first node of bucket list */
    hashNodePtr = hash->array[i];

    while (hashNodePtr != NULL){

      /*
       * Save pointer to next node,
       * free current node, and continue.
       */
      tmp = hashNodePtr->next;
      hashFreeNode(hashNodePtr,destructor);
      hashNodePtr = tmp;

    } /* end while (hashNodePtr != NULL) */

  } /* end for (i = 0; i <= hash.num_buckets; i++) */

  /* Free hash table array */
  if (hash->array != NULL)
    free(hash->array);

  /* Free hash structure */
  if (hash != NULL)
    free(hash);

} /* end hashDestroy() */


/* ======================= hashAdd() ========================= */
/* ======================= hashAdd() ========================= */

/*
 * hashAdd()
 * This function adds a new key/data pair to the
 * hash table.
 *
 * INPUT:    hash    Hash table to add key/data to.
 *           vkey    String key (that gets hashed)
 *           data    Void pointer to data container
 * RETURNS:  0       Success
 *           -1      Failure
 */
int hashAdd(Hash *hash, char *vkey, void *data){
  unsigned int      key;
  struct HashNode   *hashNode;
  int               ret = -1;

  hashNode = (struct HashNode *) malloc (sizeof(struct HashNode));
  if (hashNode != NULL) {

    /* Allocate space for vkey */
    hashNode->vkey = (char *) malloc (strlen(vkey) * sizeof(char));

    if (hashNode->vkey != NULL){

      strcpy(hashNode->vkey,vkey);       /* Copy vkey to hashNode */
      hashNode->data = data;             /* Point data to data container */

      /* Insert node into bucket array */
      hashArrayInsert(hash->array,hashNode,hash->num_buckets);

      hash->count++;                     /* Increase node count */

      /*
       * Check if we're reached max allowable table
       * utilization then rehash with next largest
       * prime from sizes[] array.
       */
      if (hash->count/hash->num_buckets > MAX_UTILIZATION)
	hashRehash(hash, hashPrime(hash->num_buckets + 1));

      ret = 0;                           /* Return success */
    }
    else {
      free(hashNode);                    /* Malloc failure free node */
    }

  } /* end if (hashNode != NULL) */
 

  return ret;

} /* end hashAdd() */


/* ======================= hashDelete() ====================== */
/* ======================= hashDelete() ====================== */

/*
 * hashDelete()
 * This function deletes the specified note
 * from the hash table given the key.
 */
void hashDelete(Hash *hash, char *vkey,
                void (*destructor)(void *data)){

  unsigned int     key;
  struct HashNode  *prevHashNodePtr;
  struct HashNode  *nextHashNodePtr;

  if (hash != NULL){

    /* Get hashed key */
    key = hash_fval_string(vkey,hash->num_buckets);

    if (hash->array[key] != NULL) {

      /* Get bucket pointer */
      prevHashNodePtr = hash->array[key];

      /* Handle case when first node matches */
      if (strcmp(prevHashNodePtr->vkey,vkey) == 0){
	    hash->array[key] = prevHashNodePtr->next;
	    hashFreeNode(prevHashNodePtr,destructor);
      }
      else {
	    nextHashNodePtr = prevHashNodePtr->next;

	    /* Loop through bucket list */
	    while (nextHashNodePtr != NULL){

	      if (strcmp(nextHashNodePtr->vkey,vkey) == 0){
	        prevHashNodePtr->next = nextHashNodePtr->next;
	        break;
	      }

	      /* Update prev and next pointers */
	      prevHashNodePtr = nextHashNodePtr;
	      nextHashNodePtr->next = nextHashNodePtr->next;

	    } /* end while (hashNodePtr != NULL ) */

      } /* else */

      hash->count--;

    } /* end if (hash->array[key] != NULL) */

  } /* end if (hash != NULL) */

} /* hashDelete() */


/*
 * hashGet()
 * This function accepts a value and returns a
 * pointer to the associated data container.
 *
 * INPUT:     hash      Pointer to hash table.
 *            vkey      String key for lookup
 * RETURNS:   data      Pointer to data container
 *            NULL      vkey not found in hash table
 */
void *hashGet(Hash *hash, char *vkey){
  unsigned int     key;
  struct HashNode  *hashNodePtr;
  void             *ret = NULL;

  /* Get hashed key */
  key = hash_fval_string(vkey,hash->num_buckets);

  if (hash->array[key] != NULL) {

    hashNodePtr = hash->array[key];

    while (hashNodePtr != NULL) {

      /* Check node for vkey match */
      if (strcmp(hashNodePtr->vkey,vkey) == 0){

	  /* Return pointer to data */
	  ret = hashNodePtr->data;

	  break;
      }

      /* Get next node in hash bucket list */
      hashNodePtr = hashNodePtr->next;

    } /* end while (hashNodePtr != NULL) */

  } /* end if (hash.array != NULL) */

  return ret;

} /* end hashGet() */

/* ===================== hashPrint() ==================== */
/* ===================== hashPrint() ==================== */

/*
 * hashPrint()
 * This function prints the entire hash table.
 * Used for testing purposes, not advisable
 * for large maps!
 *
 * INPUT:     hash       Pointer to hash table
 *            printer    Function point to printer function.
 * RETURNS:   NONE
 */

void hashPrint(Hash *hash, void (*printer)(void *data)){

  unsigned int             i;
  struct HashNode  *hashNode;

  if (hash != NULL){
    for (i = 0; i < hash->num_buckets; i++){

      hashNode = hash->array[i];

      while (hashNode != NULL){

	    printf ("key: %u ",i);
	    printer(hashNode->data);
	    hashNode = hashNode->next;

      } /* end while (hashNode != NULL) */

    } /* end for (i = 0; i < hash->count; i++) */

  } /* end if (hash != NULL) */

} /* end hashPrint() */


/* ====================== Private Functions ====================== */
/* ====================== Private Functions ====================== */


/*
 * hash_fval_string()
 * This function accepts a string and generates
 * a hash key from it. I found this function when doing
 * a search for good hash functions on the net. This one
 * looked pretty reasonable.
 *
 * INPUT:     vkey           String to generate hash key for
 *            num_buckets    Number of buckets in hash.
 * RETURNS:   int            Hash values
 */

static
unsigned int hash_fval_string(const void *vkey,
                               unsigned int num_buckets){

  const char   *key   = (const char *)vkey;
  unsigned     result = 0;
  int          i;

  for (i = 0; *key && i<32; i++)
    result = result * 33U + *key++;

  return (result % num_buckets);

}

/* ==================== hashFreeNode() ======================= */
/* ==================== hashFreeNode() ======================= */

/*
 * hashFreeNode()
 * This private function completely frees a hash
 * node.  Make sure you've saved all pointers
 * before calling this function!
 *
 * INPUT:    hashNode    Pointer to hash node to free.
 *           destructor  Destructor function for data container.
 *
 */
static
void hashFreeNode(struct HashNode *hashNodePtr,
                  void (*destructor)(void *data)){

  if (hashNodePtr != NULL){
    freemem(hashNodePtr->data);          /* Free data container */
    freemem(hashNodePtr->vkey);          /* Free string key */
    freemem(hashNodePtr);                /* Free node struct */
  }

} /* end hashFreeData() */



/* ======================== hashRehash() ======================= */
/* ======================== hashRehash() ======================= */

/*
 * hashRehash()
 * This function will rehash the hash table to any specified
 * size.  For growing the hash table this be a prime number
 * about twice the size of the current hash. It is also possible
 * to shrink the hash table if the number of nodes decreases.
 *
 * INPUT:       hash            Pointer to hash table
 *              num_buckets     New number of buckets in hash table
 */
static
void hashRehash(Hash *hash, unsigned int num_buckets){

  struct HashNode   **newArray;          /* Array to hold rehash */
  struct HashNode   *hashNode;
  struct HashNode   *tmp;
  unsigned int      key;
  int               i;

  if (hash != NULL){
    /*
     * Allocate space for rehash table
     * This is an array of pointers to hash nodes.
     */
    newArray = (struct HashNode **) malloc ((num_buckets) *
                                         sizeof (struct HashNode *));

    /* Initialize array pointers to null */
    memset(newArray, 0 ,num_buckets * sizeof (struct HashNode *));

    /* Loop through old hash table */
    for (i = 0; i < hash->num_buckets; i++){

      /* Get start of bucket list */
      hashNode = hash->array[i];

      /* Loop though bucket list */
      while (hashNode != NULL){

        /* Rehash node into new bucket array */
        tmp = hashNode->next;
        hashArrayInsert(newArray,hashNode,num_buckets);
        hashNode = tmp;

      } /* end while (hashNode != NULL) */

    } /* end for (i = 0; i < hash->num_buckets; i++) */

    freemem(hash->array);               /* Free up old hash array */
    hash->array = newArray;             /* Save new array to hash */
    hash->num_buckets = num_buckets;    /* Save new hash table size */

  } /* end if (hash != NULL) */

} /* end hashRehash() */



/* ===================== hashArrayInsert() ===================== */
/* ===================== hashArrayInsert() ===================== */

/*
 * hashArrayInsert()
 * This function inserts a new hashNode into an array of
 * of hash buckets.  This function was abstracted out since
 * it is used by both the hashAdd() and hashRehash() functions.
 *
 * INPUT:     hashArray         Array of buckets
 *            hashNode          Node to insert into array
 *            num_buckets       Number of buckets in the array
 * RETURNS:   0                 Success
 *            -1                Failure
 * USES:      hash_fval_string()
 *
 * NOTES:     hashNode->vkey    Initialized to key string
 *            hashNode->data    Initialized to point to data container
 *
 * Remember to increment hash->count as this function can't!
 *
 */
static
int hashArrayInsert(struct HashNode **hashArray,
                    struct HashNode *hashNode,
                    unsigned int num_buckets){

    unsigned int key;
    int          ret = -1;

    if (hashArray != NULL && hashNode != NULL){

      /* Get hashed key */
      key = hash_fval_string(hashNode->vkey,num_buckets);

      /* Insert hash node at head of bucket list */
      hashNode->next = hashArray[key];
      hashArray[key] = hashNode;

      ret = 0;

    } /* end if (hashArray != NULL && hashNode != NULL) */

    return ret;

} /* end hashArrayInsert() */

/*
 * hashPrime()
 * This function accepts a value and scans the
 * sizes[] array (a list of primes) and returns
 * the next value equal to or greater than the
 * value passed in.
 *
 * INPUT:     value      Minimum size threshold
 * RETURNS:   prime      Next prime in sizes[] table
 *                       larger than the specified
 *                       threshold.
 */
static
unsigned int hashPrime(unsigned int value){
  int i;

  /*
   * Loop through sizes table to find next largest value.
   * Also, check to make sure we don't walk of the end of
   * the table.
   */
  for (i = 0; (sizes[i] < value) && (sizes[i + 1] != 0); i++);

  return(sizes[i]);
}


/* ========================== freemem() ======================== */
/* ========================== freemem() ======================== */

/*
 * freemem()
 * This is a quick wrapper function for free
 * that does NULL checking.
 *
 * INPUT:       mem     Pointer to memory to free
 * RETURNS:     NONE
 */

static
void freemem(void *mem){

  if (mem)
    free(mem);

} /* end freemem() */
