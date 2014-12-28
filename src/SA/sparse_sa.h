//  Copyright 2014. Authors
//
//  This file contains declarations of utilty methods related to
//  sparse suffix arrays.
//
//  Author: Fran Jurisic (fran.jurisic@fer.hr)
//

#ifndef BIOINF2015_SA_SPARSE_SA_H_
#define BIOINF2015_SA_SPARSE_SA_H_

#include "sa.h"

namespace suffixarray {

// Suffix array to sparse suffix array conversion. Suffix
// array sa of lenght is converted to sparse suffix array ssa
// where indices that are multiples of k are stored. Resulting
// ssa will be of length floor((n-1)/k) + 1, and must have at
// least that much capacity allocated. If sa and ssa are the
// same, the conversion will be in place.
// Returns -1 in case of errors.
int sa2ssa(const Index sa[], Index ssa[], Index n, Index k);

// Converts sparse suffix array ssa of length n to corresponding
// inverse suffix array isa. Sparse suffix array contains indices
// that are multiples of k.
// Returns -1 in case of errors.
int ssa2isa(const Index ssa[], Index isa[], Index n, Index k);

// Creates a LCP index based on character sequence of length n,
// and its sparse suffix array and corresponding inverse suffix
// array, with the sparse suffix array factor being k.
// Returns -1 in case of errors.
int ssa2lcp(const char seq[], const Index ssa[], const Index isa[], Index lcp[],
    Index n, Index k);

}  // namespace suffixarray

#endif  // BIOINF2015_SA_SPARSE_SA_H_
