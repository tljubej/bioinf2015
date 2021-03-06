// Copyright 2015. Authors, see LICENSE file
//
// Tool definitions for locating match intervals in
// suffix arrays with binary search.

#include "interval_search.h"
#include "types.h"


namespace mem {

namespace {

enum Side { LEFT, RIGHT };

// Locates left or right end (depending on s being LEFT or RIGHT) of matched
// interval.
Index bsearch(char c, const ReferenceString& ref, const MatchInterval& p,
              Side s) {
  Index l = p.from;
  Index r = p.to;
  Index t;
  Index lessequal = 0;
  if (s == LEFT) {
    t = l;
    lessequal = 1;
  } else {
    t = r;
  }

  if (c == ref.s(ref.sa(t) + p.matched)) {
    return t;
  }

  while (r - l > 1) {
    Index m = (l + r) / 2;
    if (c < ref.s(ref.sa(m) + p.matched) + lessequal) {
      r = m;
    } else {
      l = m;
    }
  }

  if (s == LEFT) {
    return r;
  } else {
    return l;
  }
}
}  // namespace


MatchInterval match_next_char(const ReferenceString& ref,
                              const std::string& query, Index query_pos,
                              const MatchInterval& prev) {
  MatchInterval newm(-1, 0, 0);

  char c = query[query_pos + prev.matched];

  if (c < ref.s(ref.sa(prev.from) + prev.matched) ||
      c > ref.s(ref.sa(prev.to) + prev.matched)) {
    return newm;
  }

  Index l, r;
  l = bsearch(c, ref, prev, LEFT);
  r = bsearch(c, ref, prev, RIGHT);

  if (l <= r) {
    newm.matched = prev.matched + 1;
    newm.from = l;
    newm.to = r;
  }

  return newm;
}

MatchInterval find_match_interval(const ReferenceString& ref,
                                  const std::string& query, Index query_pos,
                                  const MatchInterval& prev,
                                  Index match_length) {
  MatchInterval candidate = prev;

  while (query_pos+candidate.matched < (Index)query.size()) {
    MatchInterval next = match_next_char(ref, query, query_pos, candidate);

    if (next.matched == -1) {
      return candidate;
    }

    candidate = next;

    if (candidate.matched >= match_length) {
      break;
    }
  }

  return candidate;
}

}  // namespace mem
