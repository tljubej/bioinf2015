// Copyright 2015. Authors, see LICENSE file
//
// Tool definitions for locating match intervals in
// suffix arrays with binary search.
//

#ifndef BIOINF2015_MEM_INTERVAL_SEARCH_H_
#define BIOINF2015_MEM_INTERVAL_SEARCH_H_

#include <string>

#include "reference_string.h"
#include "types.h"

namespace mem {

// Struct containing information about a suffix array interval
// where current query string fragment matches at least matched
// characters. Suffix array interval bounds from and to are both
// inclusive.
struct MatchInterval {
  MatchInterval(Index m, Index f, Index t)
    : matched(m), from(f), to(t) {}
  Index matched;
  Index from;
  Index to;
};

// Finds next match interval for match length of prev.matched+1 for
// reference string ref and query string query starting at query_pos.
// Next character which will attempt to be matched is:
//  query[query_pos + prev.matched]
// If no such interval exist then return value of matched will be -1.
MatchInterval match_next_char(const ReferenceString& ref,
    const std::string& query, Index query_pos, const MatchInterval& prev);

// Find match interval in SA of reference string ref for query string query
// starting at query_pos. Previous and encompassing match interval is prev,
// and resulting interval should have up to match_length matched characters.
MatchInterval find_match_interval(const ReferenceString& ref,
    const std::string& query, Index query_pos, const MatchInterval& prev,
    Index match_length);

}  // namespace mem


#endif  // BIOINF2015_MEM_INTERVAL_SEARCH_H_
