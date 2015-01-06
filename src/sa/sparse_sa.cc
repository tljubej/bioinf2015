//  Copyright 2014. Authors, see LICENSE file
//
//  This file contains definitions of utilty methods related to
//  sparse suffix arrays.
//

#include "sa/sparse_sa.h"

#include <cstdlib>

namespace suffixarray {

int sa2ssa(const Index sa[], Index ssa[], Index n, Index k) {
  if (sa == NULL || ssa == NULL || n < 1 || k < 1) return -1;

  for (Index i = 0, j = 0; i < n; ++i) {
    if (sa[i] % k == 0) {
      ssa[j++] = sa[i];
      if (j > (n-1)/k + 1)
        return -1;
    }
  }
  return 0;
}

int ssa2isa(const Index ssa[], Index isa[], Index n, Index k) {
  if (isa == NULL || isa == NULL || n < 1 || k < 1) return -1;

  for (Index i = 0; i < n; ++i) {
    isa[ssa[i] / k] = i;
  }
  return 0;
}

// This is a variation of O(n) LCP construction algorithm modified for sparse
// suffix arrays. The modification requires k-1 more comparisons per sparse
// suffix array element, but there is only n/k elements, therefore the
// compelxity remains O(n).
int ssa2lcp(const char seq[], const Index ssa[], const Index isa[], Index lcp[],
    Index n, Index k) {
  if (seq == NULL || isa == NULL || isa == NULL || lcp == NULL
      || n < 1 || k < 1) return -1;

  Index l = 0;
  lcp[0] = -1;
  for (Index i = 0; i < (n-1)/k; ++i) {
    Index curr_ssa = isa[i];
    // We know that isa[(n-1)/k] == 0.
    Index prev = ssa[curr_ssa - 1];
    while (seq[i * k + l] == seq[prev + l]) ++l;
    lcp[curr_ssa] = l;
    if (l > k-1) {
      l = l - k;
    } else {
      l = 0;
    }
  }
  return 0;
}

}  // namespace suffixarray



