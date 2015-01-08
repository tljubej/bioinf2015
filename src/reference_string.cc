// Copyright 2014. Authors, see LICENSE file
//
// Implementation of class for reference string representation.
//

#include "reference_string.h"

#include <string>
#include <memory>

#include "sa/sa_is.h"
#include "sa/sparse_sa.h"

namespace {

// Shared part of the constructors which takes
// an full SA array of lenght currently stored in
// n_, and constructs reference string fields of
// sparse SA, ISA and LCP.
void construct_reference_data(Index sa[]) {
  if (k_ == 1) {
    sa_.swap(sa);
  } else {
    // Add padding to the string so length (including the terminating
    // character) is a multiple of k (+1), and the final character is the
    // terminating character. Original length is stored for convenience.
    if (seq->size() % k_ != 0) {
      Index n_cop = n_;
      n_ += k_ - seq->size() % k_;
      seq->append(k - seq->size() % k, '\0');
      sa_.reset(new Index[salen()]);
      suffixarray::sa2ssa(sa.get(), sa_.get() + 1, n_cop, k_);
      sa_[0] = n_ - 1;
    } else {
      sa_.reset(new Index[salen()]);
      suffixarray::sa2ssa(sa.get(), sa_.get(), n_, k_);
    }
  }
  isa_.reset(new Index[salen()]);
  suffixarray::ssa2isa(sa_.get(), isa_.get(), salen(), k_);
  lcp_.reset(new Index[salen()]);
  suffixarray::ssa2lcp(s_->c_str(), sa_.get(), isa_.get(),
      lcp_.get(), n_, k_);
}


}  // namespace

ReferenceString::ReferenceString(std::string* seq, Index k)
  : s_(seq), k_(k) {
  n_ = seq->size() + 1;
  std::unique_ptr<Index[]> sa(new Index[n_]);
  // 127 is maximal positive value for char
  suffixarray::sa_is(s_->c_str(), sa.get(), n_, 127);
  construct_reference_data(sa.release());
}

ReferenceString::ReferenceString(std::string* seq, Index sa[],
    Index k)
  : s_(seq), k_(k) {
  n_ = seq->size() + 1;
  construct_reference_data(sa);
}

// Returns length of the reference string.
Index ReferenceString::slen() const {
  return n_;
}

// Returns sparse suffix array factor k.
Index ReferenceString::k() const {
  return k_;
}

// Returns character of reference string at index n.
char ReferenceString::s(Index n) const {
  return (*s_)[n];
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

