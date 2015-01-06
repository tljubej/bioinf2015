// Copyright 2015. Authors, see LICENSE file
//
// Definition of MEMFinder implementation based upon
// bioinformatics.oxfordjournals.org/content/25/13/1609.long
//

#include "mem/mem_impl.h"

#include <algorithm>
#include <string>
#include <set>
#include <vector>

#include "mem/interval_search.h"
#include "mem/mem.h"
#include "types.h"

namespace mem {
namespace {

// A comparison functor for MEM structs. Compares first by length,
// then by index of beginning in reference string and finally
// by index of beginning in query string.
struct MEMComparator {
  bool operator() (const mem::MEM& l, const mem::MEM& r) const {
    if (l.length != r.length) {
      return l.length < r.length;
    } else if (l.reference_string_idx != r.reference_string_idx) {
      return l.reference_string_idx < r.reference_string_idx;
    } else {
      return l.query_string_idx < r.query_string_idx;
    }
  }
};

// Takes an existing MEM in reference string and query string and returns
// a MEM maximally extended to the left of the current match.
// Extension goes only up to k-1 characters since the remainder of algorithm
// guarantees that if extensions is possible to the k-th character, then that
// match was previosly found already.
MEM mem_extend_left(const ReferenceString& ref, const std::string& query,
    const MEM& mem) {
  Index q = mem.query_string_idx;
  Index r = mem.reference_string_idx;
  for (Index i = 0; i < ref.k(); ++i) {
    if ((r - i == 0 || q - i == 0) || (ref.s(r - i - 1) != query[q - i - 1])) {
      return MEM(r - i, q - i, mem.length + i);
    }
  }
}

// Collects MEMs for query string position query_pos of length l or greater.
// min_match and max_match are minimal and maximal match intervals in SA,
// and found MEMs are added to set mems.
void collect_mems(const ReferenceString& ref, const std::string& query,
    Index l, Index query_pos, const MatchInterval& min_match,
    const MatchInterval& max_match, std::set<MEM, MEMComparator>* mems) {
  Index left = max_match.from;
  Index right = max_match.to;
  Index matched = max_match.matched;
  for (Index i = left; i <= right; ++i) {
    MEM mem = mem_extend_left(ref, query,
        MEM(ref.sa(i), query_pos, matched));
    if (mem.length >= l)
      mems->insert(mem);
  }

  while (matched >= min_match.matched) {
    if (right + 1 < ref.salen())
      matched = std::max(ref.lcp(left), ref.lcp(right));
    else
      matched = ref.lcp(left);

    if (matched >= min_match.matched) {
      // lcp[0] is -1 by definition.
      while (ref.lcp(left) >= matched) {
        MEM mem = mem_extend_left(ref, query,
            MEM(ref.sa(--left), query_pos, matched));
        if (mem.length >= l)
          mems->insert(mem);
      }
      while (right + 1 < ref.salen() && ref.lcp(right + 1) >= matched) {
        MEM mem = mem_extend_left(ref, query,
            MEM(ref.sa(++right), query_pos, matched));
        if (mem.length >= l)
          mems->insert(mem);
      }
    }
  }
}

// Internal implementation of MEMFinderImpl::find_mems, taking an
// additional argument query_p0 which indicates starting offset,
// allowing for query_p0 = 0..k-1 to be called in parrallel, and
// instead of filling a vector, fills a set for authors convenienve.
int find_mems_internal(
    const ReferenceString& ref, const std::string& query,
    Index l, Index query_p0, std::set<MEM, MEMComparator>* mems) {

  MatchInterval min_match(0, 0, ref.salen());
  MatchInterval max_match(0, 0, ref.salen());
  Index query_pos = query_p0;

  while (query_pos < query.size() - (ref.k() - query_p0)) {
    min_match = find_match_interval(ref, query, query_pos, min_match,
        l - (ref.k() - 1));
    max_match = find_match_interval(ref, query, query_pos, max_match,
        query.size());
    if (min_match.matched <= 1) {
      min_match = MatchInterval(0, 0, ref.salen());
      max_match = MatchInterval(0, 0, ref.salen());
      query_pos += ref.k();
      continue;
    }
    if (min_match.matched > l - (ref.k() - 1)) {
      collect_mems(ref, query, l, query_pos, min_match, max_match, mems);
    }
    query_pos += ref.k();
    // TODO(Fran): Use suffixlink magic to reset match intervals.
    if (min_match.matched < 1) {
      min_match = MatchInterval(0, 0, ref.salen());
      max_match = MatchInterval(0, 0, ref.salen());
    }
  }
  return 0;
}

}  // namespace

int MEMFinderImpl::find_mems(
    const ReferenceString& ref, const std::string& query,
    Index l, std::vector<MEM>* mems) {

  std::set<MEM, MEMComparator> mems_set;
  for (Index i = 0; i < ref.k(); ++i) {
    if (find_mems_internal(ref, query, l, i, &mems_set) == -1)
      return -1;
  }
  for (const MEM& mem : mems_set) {
    mems->push_back(mem);
  }
  return 0;
}

}  // namespace mem
