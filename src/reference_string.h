// Copyright 2014. Authors, see LICENSE file
//
// Declarations of class for reference string representation.
//

#ifndef BIOINF2015_REFERENCE_STRING_H_
#define BIOINF2015_REFERENCE_STRING_H_

#include <memory>

#include "types.h"

// Class aggregating reference string information:
//  - the string itself
//  - (sparse) suffix array
//  - inverse suffix array
//  - longest common prefix array
class ReferenceString {
 public:
  // Creates a reference string object from the corresponding
  // string of length n and sparse suffix array factor k.
  // Does not take ownership or make a copy of the string.
  ReferenceString(const char seq[], Index n, Index k);

  // Returns length of the reference string.
  Index slen() const;

  // Returns sparse suffix array factor k.
  Index k() const;

  // Returns character of reference string at index n.
  char s(Index n) const;

  // Returns length of sa, isa and lcp.
  Index salen() const;

  // Returns (sparse) suffix array element at index n.
  Index sa(Index n) const;

  // Returns inverse suffix array element at index n.
  Index isa(Index n) const;

  // Returns longest common prefix array element at index n.
  Index lcp(Index n) const;

 private:
  const char* s_;
  std::unique_ptr<Index[]> sa_;
  std::unique_ptr<Index[]> isa_;
  std::unique_ptr<Index[]> lcp_;
  Index n_, k_;
};

#endif  // BIOINF2015_REFERENCE_STRING_H_
