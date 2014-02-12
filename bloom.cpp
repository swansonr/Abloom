#include "bloom.h"

bool BloomFilter::add(const char *s)
{
    return true;
}

bool BloomFilter::check(const char *s)
{
    return false;
}

unsigned int BloomFilter::hash_fn1(const char *s)
{
    return 0;
}

unsigned int BloomFilter::hash_fn2(const char *s)
{
    return 0;
}
