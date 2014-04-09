#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#define GETSLOT(i) ((i + slots) % slots)
#define BYTE_MASK 255
#define INT_MASK 4294967295

#define FINGER_BITS 3

// Used for hashing. Removes the need to shift and mask to get our q and r
union Unit
{
    unsigned long l;
    unsigned int i[2];
};

class QFilter
{
public:
    QFilter(int qq, int rr, bool v = false);            // Constructor taking the size of hash table and number of hashes to generate
    QFilter();
    ~QFilter();

    bool add(const char *s);                            // Add a value to the bloom filter
    bool check(const char *s);                          // Check if a value is present in the bloom filter

private:
    unsigned long slots;                                       // Size of the hash table

    unsigned int q;
    unsigned int r;

    unsigned int *rslots;                         // Bit-array pointer
    unsigned char *fslots;
    
    Unit *hash(const char *s);                   // Hashing function

    //void set_slot(int i, int bits, unsigned char s, unsigned char *arr);
    //unsigned char get_slot(int i, int bits, unsigned char *arr);

    //void set_fingerprint(int i, unsigned char s, unsigned char *arr, int bits);
    //unsigned char get_fingerprint(int i, unsigned char *arr, int bits);
    void set_fingerprint(int i, unsigned char s); //, unsigned char *arr = fslots, int bits = 3); //{ set_slot(i, 3, s, fslots); }
    unsigned char get_fingerprint(int i); //, unsigned char *arr = fslots, int bits = 3); //{ return get_slot(i, 3, fslots); }

    inline void set_remainder(int i, unsigned int s) {rslots[i] = s;} 
    inline unsigned int get_remainder(int i) {return rslots[i]; }

    //const void printFast();
    //const void printSlow();

    bool VERBOSE;                                       // Outputs additional (debug) information during insertions, checks, etc.
}; 
