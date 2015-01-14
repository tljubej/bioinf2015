bioinf2015
==========

Maximum exact match detection algorithm implementation as a project for a [BioInformatics course](http://www.fer.unizg.hr/en/course/bio)
on Faculty of Electrical Engineering and Computing, University of Zagreb.

The algorithm used is from the article ["A practical algorithm for finding maximal exact matches in large sequence datasets using sparse suffix arrays"](http://bioinformatics.oxfordjournals.org/content/25/13/1609.long),
and the Suffix Array construction algorithm is SA-IS algorithm from ["Two Efficient Algorithms for Linear Time Suffix Array Construction"](http://www.computer.org/csdl/trans/tc/2011/10/ttc2011101471-abs.html).

Project source code is in src/, license can be found in LICENSE.
The source code adheres to [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.html).

To build:

    make

The binary files should be in `build/programs`. A C++11 compliant compiler is required.

To generate test data:

    make prep-test-data

To test against a `python3` brute-force implementation:

    make run-tests
    
To run benchmarks on large datasets with various values of 'K'

    make run-benchmark
    
The tools `pv` and GNU Time are required to run the benchmarks.

If running the tests or benchmarks produces strange errors:

    terminate called after throwing an instance of 'std::system_error'
      what():  Enable multithreading to use std::thread: Operation not permitted

then you have most likely encountered a GCC [bug](https://bugs.launchpad.net/ubuntu/+source/gcc-defaults/+bug/1228201), which is worked around by rebuilding with

    make LDFLAGS=-Wl,--no-as-needed

This bug is present on bio-linux 8. 
