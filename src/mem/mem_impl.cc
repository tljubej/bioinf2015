// Copyright 2014. Authors, see LICENSE file
//
// Definition of MEMFinder implementation based upon
// bioinformatics.oxfordjournals.org/content/25/13/1609.long
//

#include "mem/mem_impl.h"

#include "mem/mem.h"

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

}  // namespace

namespace mem {

int MEMFinderImpl::FindMEMs(
    const ReferenceString& ref, const std::string& query,
    Index l, std::vector<MEM>* mems) {
  // TODO(Fran): Implementation ofc
  return 0;
}


}  // namespace mem
