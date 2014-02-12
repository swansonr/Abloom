#pragma once

#include <stdlib.h>

class BloomFilter
{
public:
    BloomFilter();
    ~BloomFilter();

    bool add(const char *s);
    bool check(const char *s);

private:
    size_t m;
    size_t k;

    unsigned char *arr;
    
    unsigned int hash_fn1(const char *s);
    unsigned int hash_fn2(const char *s);
}; 
