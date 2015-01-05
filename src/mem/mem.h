// Copyright 2014. Authors, see LICENSE file
//
// Declaration of general Maximal Exact Match tools.
//

#ifndef BIOINF2015_MEM_MEM_H_
#define BIOINF2015_MEM_MEM_H_

#include <string>
#include <vector>

#include "reference_string.h"
#include "types.h"

namespace mem {

// Represents a resulting exact match, containing the indices
// of match beginnings in the reference and query string and
// the match length.
struct MEM {
  Index reference_string_idx;
  Index query_string_idx;
  Index length;
};

// Interface for classes that look for maximal exact matches.
class MEMFinder {
 public:
  // Finds maximal exact matches of query string in reference string ref, of
  // minimal length l and stores them in vector mems.
  // Matches in resulting vector are unique.
  // Returns -1 in case of errors.
  virtual int FindMEMs(ReferenceString ref, std::string query, Index l,
      std::vector<MEM>* mems) = 0;
};

}  // namespace mem

#endif  // BIOINF2015_MEM_MEM_H_

