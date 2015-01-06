// Copyright 2014. Authors, see LICENSE file
//
// Declaration of functions for suffix link simulation

#ifndef BIOINF2015_SUFFIX_LINK_H_
#define BIOINF2015_SUFFIX_LINK_H_

#include "interval_search.h"

namespace mem {

// Finds simulated suffix link for specified interval.
MatchInterval find_suffix_link(const ReferenceString& ref,
                               const MatchInterval& interval);

}  // namespace mem

#endif  // BIOINF2015_SUFFIX_LINK_H_
