//  Copyright 2014. Authors, see LICENSE file
//
//  This file contains definitions of Suffix Array - Induced Sorting
//  (SA-IS) methods.
//

#ifndef BIOINF2015_SA_SA_IS_H_
#define BIOINF2015_SA_SA_IS_H_

#include "sa/sa.h"

namespace suffixarray {

//  Creates suffix array for sequence seq in sa. Both arrays must be
//  at least of length n, and symbols in seq must be from {0, .., k-1}, with
//  0 being reserved for sequence terminator character. Ordering is determined
//  by numerical values in seq.
//  Returns -1 in case of errors.
int sa_is(const char seq[], Index sa[], Index n, char k);

}  // namespace suffixarray

#endif  // BIOINF2015_SA_SA_IS_H_
