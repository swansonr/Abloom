#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#define SETBIT(a,n) (a[n/CHAR_BIT] |= (1<<(n%CHAR_BIT)))
#define GETBIT(a,n) (a[n/CHAR_BIT] & (1<<(n%CHAR_BIT)))
#define ROUND(size) ((size + CHAR_BIT - 1) / CHAR_BIT)

class BloomFilter
{
public:
    BloomFilter(size_t ssize, size_t nnh, bool v = false);              // Constructor taking the size of hash table and number of hashes to generate
    BloomFilter();
    ~BloomFilter();

    bool add(const char *s);                            // Add a value to the bloom filter
    bool check(const char *s);                          // Check if a value is present in the bloom filter

private:
    size_t size;                                        // Size of the hash table
    size_t nh;                                          // Number of hash functions to generate

    unsigned char *arr;                                 // Hash table pointer
    
    unsigned int hash_fn1(const char *s, int len);      // Our two hash functions
    unsigned int hash_fn2(const void *s, int len);

    bool VERBOSE;                                       // Outputs additional (debug) information during insertions, checks, etc.
}; 
