#include <stdio.h>
#include <iostream>
#include "bloom.h"
#include <stdint.h>
#include <math.h>

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
|| defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
+(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

// Simple constructor for our filter
BloomFilter::BloomFilter(size_t ssize, size_t nnh, bool v)
{
    size = ROUND(ssize);        //Rounded to next multiple of CHAR
    nh = nnh;

    // Initialize all bits to 0
    arr = (unsigned char *)calloc(size, sizeof(char));

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
    unsigned int hash;
    int i;

    if(VERBOSE)  std::cout << "Inserting " << s << std::endl;
    
    // We generate nh hashes using the formula f1(s) + i*f2(s) + i^2 % size
    // and set the appropriate bit to 1
    for(i=0; i<nh; i++) {
        hash = (hash_fn1(s, size) + i*hash_fn2(s, size) + i*i) % size;
        SETBIT(arr, hash);

        if(VERBOSE) std::cout << hash << std::endl;
    }

    if(VERBOSE) std::cout << std::endl;

    return true;
}

// Check if a value is inside the hash table
bool BloomFilter::check(const char *s)
{
    unsigned int hash;
    int i;

    if(VERBOSE) std::cout << "Checking " << s << std::endl;

    //All nh bits found by the nh hash functions must be set to 1 for there to be a match
    for(i=0; i<nh; i++) {
        hash = (hash_fn1(s, size) + i*hash_fn2(s, size) + i*i) % size;

        if(VERBOSE) std::cout << hash << std::endl;
        
        if( !GETBIT(arr, hash) ) {
            return false;
        }
    }
    if(VERBOSE) std::cout << std::endl;

    return true;
}


//
//  Super fast hash as described by Paul Hsieh
//  http://www.azillionmonss.com/qed/hash.html
//

unsigned int BloomFilter::hash_fn1(const char * s, int len)
{
    uint32_t hash = len, tmp;
    int rem;
    
    if (len <= 0 || s == NULL) return 0;
    
    rem = len & 3;
    len >>= 2;
    
    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (s);
        tmp    = (get16bits (s+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        s  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }
    
    switch (rem) {
        case 3: hash += get16bits (s);
            hash ^= hash << 16;
            hash ^= ((signed char)s[sizeof (uint16_t)]) << 18;
            hash += hash >> 11;
            break;
        case 2: hash += get16bits (s);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1: hash += (signed char)*s;
            hash ^= hash << 10;
            hash += hash >> 1;
    }
    
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
    
    return hash;
}

//
//  Murmer Hash as described by Austin Appleby
//  https://sites.google.com/site/murmurhash/
//

unsigned int BloomFilter::hash_fn2(const void *s, int len)
{
    const int seed = pow(2, 17)-1;
    const unsigned int m = 0x5bd1e995;
    const int r = 24;
    unsigned int h = seed ^ len;
    const unsigned char *ss = (const unsigned char *)s;

    while( len >= 4) {
        unsigned int k = *(unsigned int *)ss;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^=k;

        ss += 4;
        len -= 4;
    }

    switch (len) {
        case 3: h ^= ss[2] << 16;
        case 2: h ^= ss[1] << 8;
        case 1: h ^= ss[0];
                h *= m;
    };


    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}
