// Copyright 2014. Authors, see LICENSE file
//
// Definition of functions for suffix link simulation

#include "suffix_link.h"
#include "interval_search.h"

#include <cassert>


namespace mem {

namespace {

// Returns log2 of an Index (integral type), -1 if x is 0
Index log2i(Index x) {
  assert(x > 0);

  Index res = -1;

  while (x) {
    x >>= 1;
    res++;
  }

  return res;
}

// Expands interval by LCP expansion
MatchInterval expand_link(const ReferenceString& ref,
                          const MatchInterval& interval) {
  Index q = interval.matched;
  Index n = ref.salen();
  if (q == 0) {
    return MatchInterval(0, 0, n - 1);
  }
  Index t = 2 * q * log2i(n);
  Index e = 0;
  Index l = interval.from;
  Index r = interval.to;

  while (l >= 0 && ref.lcp(l) >= q) {
    e++;
    if (e >= t) {
      return MatchInterval(-1, 0, 0);
    }
    l--;
  }

  while (r <= n-1 && ref.lcp(r+1) >= q) {
    e++;
    if (e >= t) {
      return MatchInterval(-1, 0, 0);
    }
    r++;
  }
  return MatchInterval(q, l, r);
}

}  // namespace

MatchInterval find_suffix_link(const ReferenceString& ref,
                               const MatchInterval& interval) {
  Index q = interval.matched - ref.k()
  if (q <= 0) {
    return MatchInterval(-1, 0, 0);
  }
  Index l = interval.from;
  Index r = interval.to;
  Index newl = ref.isa(ref.sa(l)/ref.k() + 1);
  Index newr = ref.isa(ref.sa(r)/ref.k() + 1);

  assert(newl < ref.salen() && newr < ref.salen());

  MatchInterval link(q, newl, newr);

  return expand_link(ref, link);

}

}  // namespace mem
