// Copyright 2015. Authors, see LICENSE file
//
// Definition of MEMFinder implementation based upon
// bioinformatics.oxfordjournals.org/content/25/13/1609.long
//

#include "mem/mem_impl.h"

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
    // TODO(Fran): Get min_match and max_match.
    if (min_match.matched <= 1) {
      min_match = MatchInterval(0, 0, ref.salen());
      max_match = MatchInterval(0, 0, ref.salen());
      query_pos += ref.k();
      continue;
    }
    if (min_match.matched > l - (ref.k() - 1)) {
      // TODO(Fran): Collect mems to set.
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
