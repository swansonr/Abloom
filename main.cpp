/* 
 *  Example testing methods for the Bloom and Quotient Filter.
 *
 *  Lets you easily run a number of insertion or queries while tracking
 *  the number of false positives/false insertions/false negatives and
 *  much much more!
 *
 */
#include "hashes/city.h"
#include "bloom.h"
#include "qfilter.h"
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>

int main()
{
    unsigned long MAX_REAL;
    unsigned long MAX_RAND;
    unsigned long size;
    clock_t start, diff;
    srand(time(NULL));

    //for(int ii=1; ii<=8; ii++)
    //for(int jj=2; jj<=20; jj+=2)
    {
		int ii = 5;
		int jj = 14;
        size = pow(2,4*ii);
        MAX_REAL = size*0.5;
        MAX_RAND = MAX_REAL*2;
        start = clock();

        //k = load*ln(2)            // Optimal number of hashes for Bloom Filter
        //BloomFilter *boo= new BloomFilter(size, jj, false);
		QFilter *boo = new QFilter(10,10,false);

        // Inserts a set sequence of strings
		std::cout << "Inserting:\n" << std::endl;
        for(unsigned long i=0; i<MAX_REAL; i++) {
            char t[50] = {'\0'};
            snprintf(t, 50, "hheel%lu", i);
            boo->add(t);
        }


        // Checks for values that can not possibly be in the filter
		std::cout << "Checking:\n" << std::endl;
        unsigned long fp = 0;
        unsigned long fn = 0;
        for(unsigned long i=0; i<MAX_RAND; i++) {
            char t[50] = {'\0'};
            unsigned long ran = rand()%(size*2);
            snprintf(t, 50, "hheel%lu", ran);
            bool check = boo->check(t);
            if(check && (ran >= MAX_REAL)) {
                fp++;
            }
            else if(!check && (ran < MAX_REAL) ) {
                printf("FN: %s\n", t);
                fn++;
            }
        }

        // Print out stats
        diff = clock() - start;
        int ms = diff*1000/CLOCKS_PER_SEC;
        printf("%d %d - False Negatives %lu, False Positives %lu, out of %lu (time = %d)\n", ii, jj, fn, fp, MAX_RAND, ms);
        delete boo;
    }

    return 0;
}

