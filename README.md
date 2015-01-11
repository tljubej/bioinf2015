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

The binary files should be in `build/programs`.

To test against a brute-force implementation:

    make run-tests

A C++11 compliant compiler is required.
