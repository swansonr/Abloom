#include <stdio.h>
#include <iostream>
#include "bloom.h"
#include <stdint.h>
#include <math.h>
#include <cstring>
#include "hashes/city.h"
#include "hashes/murmur3.h"

// Simple constructor for our filter
BloomFilter::BloomFilter(unsigned long ssize, int nnh, bool v)
{
    size = ssize;
    nh = nnh;

    if(size % 8 != 0) {
        fprintf(stderr, "Warning: Bloom Filter size is not a multiple of char size.\n");
    }

    // Initialize all bits to 0
    arr = (unsigned char *)calloc(size, 1);

    VERBOSE = v;
}

// Deconstructor
BloomFilter::~BloomFilter()
{
    delete arr;
}

// Add a value to the hash table
bool BloomFilter::add(const char *s)
{
    unsigned long hash;
    int i;

    if(VERBOSE)  std::cout << "Inserting " << s << std::endl;

    unsigned long hash1 = CityHash64(s, strlen(s));
    unsigned long hash2;
    MurmurHash3_x86_128 (s, strlen(s), 4095, &hash2);

    // We generate nh hashes using the formula f1(s) + i*f2(s) + i^2 % size
    // and set the appropriate bit to 1
    for(i=0; i<nh; i++) {
        hash = (hash1 + i*hash2 + i*i) % size;
        SETBIT(arr, hash);
        if(!GETBIT(arr,hash)) printf("SETBIT ERROR\n");

        if(VERBOSE) std::cout << hash << std::endl;
    }

    if(VERBOSE) std::cout << std::endl;

    return true;
}

// Check if a value is inside the hash table
bool BloomFilter::check(const char *s)
{
    unsigned long hash;
    int i;

    if(VERBOSE) std::cout << "Checking " << s << std::endl;

    unsigned long hash1 = CityHash64(s, strlen(s));
    unsigned long hash2;
    MurmurHash3_x86_128 (s, strlen(s), 4095, &hash2);

    //All nh bits found by the nh hash functions must be set to 1 for there to be a match
    for(i=0; i<nh; i++) {

        hash = (hash1 + i*hash2 + i*i) % size;
        
        if( !GETBIT(arr, hash) ) {
            return false;
        }
    }
    if(VERBOSE) std::cout << std::endl;

    return true;
}
