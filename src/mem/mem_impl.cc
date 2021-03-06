// Copyright 2015. Authors, see LICENSE file
//
// Definition of MEMFinder implementation based upon
// bioinformatics.oxfordjournals.org/content/25/13/1609.long
//

#include "mem/mem_impl.h"

#include <algorithm>
#include <future>
#include <string>
#include <vector>

#include "mem/interval_search.h"
#include "mem/mem.h"
#include "mem/suffix_link.h"
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
  return MEM(-1, 0, 0);
}

// Collects MEMs for query string position query_pos of length l or greater.
// min_match and max_match are minimal and maximal match intervals in SA,
// and found MEMs are added to vector mems.
void collect_mems(const ReferenceString& ref, const std::string& query,
    Index l, Index query_pos, const MatchInterval& min_match,
    const MatchInterval& max_match, std::vector<MEM>* mems) {
  Index left = max_match.from;
  Index right = max_match.to;
  Index matched = max_match.matched;
  for (Index i = left; i <= right; ++i) {
    MEM mem = mem_extend_left(ref, query,
        MEM(ref.sa(i), query_pos, matched));
    if (mem.length >= l)
      mems->push_back(mem);
  }

  while (matched >= min_match.matched) {
    if (right + 1 < ref.salen())
      matched = std::max(ref.lcp(left), ref.lcp(right+1));
    else
      matched = ref.lcp(left);

    if (matched >= min_match.matched) {
      // lcp[0] is -1 by definition.
      while (ref.lcp(left) >= matched) {
        MEM mem = mem_extend_left(ref, query,
            MEM(ref.sa(--left), query_pos, matched));
        if (mem.length >= l)
          mems->push_back(mem);
      }
      while (right + 1 < ref.salen() && ref.lcp(right + 1) >= matched) {
        MEM mem = mem_extend_left(ref, query,
            MEM(ref.sa(++right), query_pos, matched));
        if (mem.length >= l)
          mems->push_back(mem);
      }
    }
  }
}

// Internal implementation of MEMFinderImpl::find_mems, taking an
// additional argument query_p0 which indicates starting offset,
// allowing for query_p0 = 0..k-1 to be called in parallel, and stores
// found MEMs in vector mems.
int find_mems_internal(
    const ReferenceString& ref, const std::string& query,
    Index l, Index query_p0, std::vector<MEM>* mems) {
  MatchInterval min_match(0, 0, ref.salen()-1);
  MatchInterval max_match(0, 0, ref.salen()-1);
  Index query_pos = query_p0;

  while (query_pos <= (Index)query.size() - (ref.k() - query_p0)) {
    min_match = find_match_interval(ref, query, query_pos, min_match,
        l - (ref.k() - 1));
    max_match = find_match_interval(ref, query, query_pos, max_match,
        query.size());
    if (min_match.matched <= 1) {
      min_match = MatchInterval(0, 0, ref.salen()-1);
      max_match = MatchInterval(0, 0, ref.salen()-1);
      query_pos += ref.k();
      continue;
    }
    if (min_match.matched >= l - (ref.k() - 1)) {
      collect_mems(ref, query, l, query_pos, min_match, max_match, mems);
    }
    query_pos += ref.k();
    min_match = find_suffix_link(ref, min_match);
    max_match = find_suffix_link(ref, max_match);
    if (min_match.matched < 1) {
      min_match = MatchInterval(0, 0, ref.salen()-1);
      max_match = MatchInterval(0, 0, ref.salen()-1);
    }
  }
  return 0;
}

}  // namespace

int MEMFinderImpl::find_mems(
    const ReferenceString& ref, const std::string& query,
    Index l, std::vector<MEM>* mems) {

  std::vector<std::vector<MEM>> mems_parallel;
  std::vector<std::future<int>> futures;
  mems_parallel.resize(ref.k());
  futures.resize(ref.k());
  for (Index i = 0; i < ref.k(); ++i) {
    futures[i] = std::async(std::launch::async,
        find_mems_internal, std::ref(ref), std::ref(query),
        l, i, &(mems_parallel[i]));
  }
  int ret = 0;
  for (Index i = 0; i < ref.k(); ++i) {
    if (futures[i].get() == -1) {
      ret = -1;
    } else {
      for (const MEM& mem : mems_parallel[i]) {
        mems->push_back(mem);
      }
    }
  }
  return ret;
}

}  // namespace mem
