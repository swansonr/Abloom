#include <stdio.h>
#include <iostream>
#include "qfilter.h"
#include "hashes/city.h"
#include <stdint.h>
#include <math.h>
#include <cstring>

// Simple constructor for our filter
QFilter::QFilter(int qq, int rr, bool v)
{
    if(VERBOSE) {
        printf("::Creating QFilter(%d %d %d)\n", qq, rr, v);
    }

    //Quotient
    q = 8*sizeof(unsigned int);

    //Remainder
    r = 8*sizeof(unsigned int);

    slots = pow(2, q);

    if(VERBOSE) {
        printf("NEW QFILTER - %d %d %lu\n", q, r, slots);
    }

    // Holds the fingerprint (i.e., the 3 identifier bits)
    long fslots_size = slots * 3;
    fslots = (unsigned char *)calloc(fslots_size, 1);

    // Holds the remainder (i.e., comparison data)
    long rslots_size = slots * r;
    rslots = (unsigned int *)calloc(rslots_size, sizeof(unsigned int));

    if(fslots == NULL || rslots == NULL) {
        fprintf(stderr, "WARNING: Unable to allocate Quotient Filter memory.\n");
    }

    VERBOSE = v;
}

QFilter::~QFilter()
{
    if(VERBOSE) {
        printf("::Deleting QFilter\n");
    }
    delete fslots;
    delete rslots;
}

// Add a value to the filter
bool QFilter::add(const char *s)
{
    Unit *u = hash(s);

    unsigned int rem = u->i[1]; 
    unsigned int quo = u->i[0];
    int finger = get_fingerprint(quo);          // Identifying bytes
    int runs = 1;                               // If the fingerprint is zero there must be a run
    unsigned int i = quo;
    bool run_exists = false;

    if(VERBOSE) {
        std::cout << "::Adding " << s << "(" << u->l << " -> " << u->i[1] << " + " << u->i[0] << std::endl;
    }

    if(!finger) {
        set_fingerprint(i, 0b100);           // There was no values present at the canonical position
        set_remainder(i, rem);               // So we can just go ahead and insert our value
        if(VERBOSE) {
            std::cout << "Inserted " << s << " (No fingerprint). index = " << i << std::endl;
        }
        delete u;
        return true;
    }
    run_exists = (finger & 4) >> 2;

    while(finger ^ 4) {                     // While we aren't at the start of a cluster
        finger = get_fingerprint(--i);      // Scan left to find beginning of runs/clusters
        runs += (finger & 4) >> 2;
    }

    while(runs > 1) {                       // Find run start
        finger = get_fingerprint(++i);
        runs -= ((finger ^ 2) & 2) >> 1;
    }

    unsigned char remainder = 0;
    if (run_exists) {
        do {
            finger = get_fingerprint(i);
            remainder = get_remainder(i);
            if (run_exists && remainder == rem) {
                delete u;
                return false;
            }
            i++;
        } while (finger & 2);
    }

    finger = get_fingerprint(i);
    remainder = get_remainder(i);

    set_fingerprint(i, (finger & 4) | ((run_exists) << 1) | 1 );
    set_remainder(i, rem);

    if(VERBOSE) {
        std::cout << "Finger, run_exists" << std::endl;
    }

    unsigned char k = 0;
    unsigned char next_finger = 0;
    unsigned char next_rem = 0;

    while (finger) {
        next_finger = get_fingerprint(++i);
        next_rem = get_remainder(i);

        k = ((finger & 2) + 1) | (next_finger & 4);

        set_fingerprint(i, k);
        set_remainder(i, remainder);

        finger = next_finger;
        remainder = next_rem;
    }

    if (!run_exists) {
        set_fingerprint(quo, get_fingerprint(quo) | 0b100);
    }

    delete u;
    return true;
}

// Check if a value is inside the filter
bool QFilter::check(const char *s)
{
    if(VERBOSE) {
        std::cout << "::Checking " << s << std::endl;
    }

    Unit *u = hash(s);
    unsigned int rem = u->i[1];
    unsigned int quo = u->i[0]; 
    int finger = get_fingerprint(quo);          // Identifying bytes
    int runs = 1;                               // If the fingerprint is zero there must be a run
    unsigned int i = quo;

    // Check if the fingerprint is empty
    if(!(finger & 4)) {
        if(VERBOSE) {
            std::cout << "Not found (Fingerprint empty) -- Hash(s) = " << u->l << " , index = " << quo << std::endl;
        }
        delete u;
        return false;
    }

    while(finger ^ 4) {                         // While we aren't at the start of a cluster
        finger = get_fingerprint(--i);          // Scan left to find beginning of runs/clusters
        runs += (finger & 4) >> 2;
    }

    while(runs > 1) {                       // Find run start
        finger = get_fingerprint(++i);
        runs -= ((finger ^ 2) & 2) >> 1;
    }

    do {                                    // Compare remainders to see if our value is in this run
        if (get_remainder(i) == rem) {
            delete u;
            return true;
        }
        finger = get_fingerprint(++i);
    } while(finger & 2);

    delete u;
    return false;
}

Unit *QFilter::hash(const char *s)
{
    Unit *u = (Unit *)malloc(sizeof(Unit));
    u->l = CityHash64(s, strlen(s)) % slots;

    return u;
}

unsigned char QFilter::get_fingerprint(int i) //, unsigned char *arr = fslots, int bits = 3)
{
    int bits = FINGER_BITS;
    unsigned char *arr = fslots;

    unsigned int start = GETSLOT(i) * bits;
    unsigned int start_byte = start >> 3;
    unsigned int start_bit = start & 7;
    unsigned int end = start + bits - 1;
    unsigned int end_byte = end >> 3;
    unsigned int end_bit = end & 7;
    unsigned char result = 0;

    // If the values are in one slot the job is easy
    if (start_byte == end_byte) {
        return (arr[start_byte] >> (7 - end_bit)) & ((1 << bits) - 1);
    }

    // Here we've unrolled the for loop since we know it will be in a maximum of two slots
    result = (arr[start_byte] << start_bit & BYTE_MASK) >> start_bit;
    result = (result << (end_bit + 1)) | (arr[end_byte] >> (7 - end_bit));

    return result;
}

void QFilter::set_fingerprint(int i, unsigned char s) //, unsigned char *arr = fslots, int bits = 3)
{
    int bits = FINGER_BITS;
    unsigned char *arr = fslots;

    unsigned int start = GETSLOT(i) * bits;
    unsigned int start_byte = start >> 3;
    unsigned int start_bit = start & 7;

    unsigned int end = start + bits - 1;
    unsigned int end_byte = end >> 3;
    unsigned int end_bit = (end & 7) + 1;
    unsigned char current, mask, value;

    if(VERBOSE) {
        std::cout << "Setting Fingerprint:\n";
        std::cout << "Start/End Byte " << start_byte << " " << end_byte << std::endl << std::endl;
    }

    // If there's only one slot the job is fairly easy
    if (start_byte == end_byte) {
        current = arr[start_byte];

        mask = (BYTE_MASK & (BYTE_MASK << (8 - start_bit))) | (BYTE_MASK >> end_bit);
        value = s << (8 - end_bit);
        current = current & mask;

        if(VERBOSE) {
            std::cout << "Set byte " << start_byte << " to " << (int)(current | value) << std::endl;
            std::cout << "Current= " << (int)current << ", mask=" << (int)mask << ", value=" << (int)value <<std::endl;
        }

        arr[start_byte] = current | value;
    } else {
            // Here the while loop is unrolled again due to the fact that we are always in at most two slots
            current = arr[end_byte];
            mask = BYTE_MASK;
            value = (s << (8 - end_bit)) & BYTE_MASK; 
            s >>= end_bit;
            mask &= BYTE_MASK >> end_bit; 
            current = current & mask;
            arr[end_byte] = current | value;

            current = arr[start_byte];
            mask = BYTE_MASK;
            value = s & (BYTE_MASK >> start_bit);
            mask &= BYTE_MASK << (8 - start_bit); 
            current = current & mask;
            arr[start_byte] = current | value;
    }
}
