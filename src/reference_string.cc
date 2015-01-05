// Copyright 2014. Authors, see LICENSE file
//
// Implementation of class for reference string representation.
//

#include "reference_string.h"

#include <cstring>
#include <memory>

#include "sa/sa_is.h"
#include "sa/sparse_sa.h"

ReferenceString::ReferenceString(const char seq[], Index k)
  : s_(seq), n_(strlen(seq)+1), k_(k) {
  std::unique_ptr<Index[]> sa(new Index[n_]);
  // 127 is maximal positive value for char
  suffixarray::sa_is(s_, sa.get(), n_, 127);
  if (k == 1) {
    sa_.swap(sa);
  } else {
    sa_.reset(new Index[salen()]);
    suffixarray::sa2ssa(sa.get(), sa_.get(), n_, k_);
  }
  isa_.reset(new Index[salen()]);
  suffixarray::ssa2isa(sa_.get(), isa_.get(), salen(), k_);
  lcp_.reset(new Index[salen()]);
  suffixarray::ssa2lcp(s_, sa_.get(), isa_.get(), lcp_.get(), n_, k_);
}

// Returns length of the reference string.
Index ReferenceString::slen() const {
  return n_ - 1;
}

// Returns sparse suffix array factor k.
Index ReferenceString::k() const {
  return k_;
}

// Returns character of reference string at index n.
char ReferenceString::s(Index n) const {
  return s_[n];
}

// Returns length of sa, isa and lcp.
Index ReferenceString::salen() const {
  return (n_-1)/k_ + 1;
}

// Returns (sparse) suffix array element at index n.
Index ReferenceString::sa(Index n) const {
  return sa_[n];
}

// Returns inverse suffix array element at index n.
Index ReferenceString::isa(Index n) const {
  return isa_[n];
}

// Returns longest common prefix array element at index n.
Index ReferenceString::lcp(Index n) const {
  return lcp_[n];
}

