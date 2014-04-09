Abloom
======

Class project for COMP4420  - Comparison/Analysis of Bloom Filters and Quotient Filters

This is a typical Bloom Filter implementation using a few newer techniques to bring it up to date.

The Quotient filter is much more customized in an attempt to speed it up to be comparable to the 
Bloom Filter. I've unrolled a few loops and made the remainder lookups to require a singular lookup.

Both filters appear to be working well -- they both return the expected number of false positives as
well as no false negatives.


!! Hash functions are included for convenience but were in no way implemented by me. 


Todo: Quotient Filter merge function still missing
