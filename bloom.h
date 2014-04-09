#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <bitset>

#define SETBIT(a,n) (a[n/CHAR_BIT] |= (1<<(n%CHAR_BIT)))
#define GETBIT(a,n) (a[n/CHAR_BIT] & (1<<(n%CHAR_BIT)))
#define ROUND(size) ((size + CHAR_BIT - size%CHAR_BIT))

class BloomFilter
{
public:
    // Constructor taking the size of hash table and number of hashes to generate
    BloomFilter(unsigned long ssize, int nnh, bool v = false);   
    BloomFilter();
    ~BloomFilter();

    bool add(const char *s);                            // Add a value to the bloom filter
    bool check(const char *s);                          // Check if a value is present in the bloom filter

private:
    unsigned char *arr;                                 // Hash table pointer
    unsigned long size;                                 // Size of the hash table
    int nh;                                             // Number of hash functions to generate

    bool VERBOSE;                                       // Outputs additional (debug) information during insertions, checks, etc.
}; 
