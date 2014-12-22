//  Copyright 2014. Authors
//
//  This file contains implementations of Suffix Array - Induced Sorting
//  (SA-IS) methods.
//
//  Author: Fran Jurisic (fran.jurisic@fer.hr)
//

#include "SA/sa_is.h"

#include <vector>
#include <algorithm>

// Namespace for local methods.
namespace {

enum SymbolType { S, L };

// Fills buckets with indicices of beginning of corresponding symbol
// buckets for Symbol sequence seq of length n and alphabet of size k.
//
// To get beggining of bucket for Symbol S use:
//    bucket[S],
// and for inclusive ending of the same bucket use:
//    bucket[S+1] - 1
// where S is from {0, .., k-1}.
void get_symbol_buckets(const Symbol seq[], Index n, Symbol k,
    std::vector<Index>* buckets) {
  buckets->resize(k+1);
  for (Index i = 0; i < n; ++i) {
    ++buckets->at(seq[i]);
  }
  Index count = 0;
  for (Symbol i = 0; i < k; ++i) {
    count += buckets->at(i);
    buckets->at(i) = count - buckets->at(i);
  }
  buckets->at(i) = n;
}

// Induced sort of LMS substrings where seq is the symbol sequence of length n
// and alphabet of size k, sa is se suffix array that is filled, t is the array
// containing seq S- or L- symbol types and lms_subs_idx is a vector of
// LMS substring indices.
void lms_induced_sort(const Symbol seq[], Index sa[], Index n, Symbol k
    const std::vector<SymbolType> t, const std::vector<Index> lms_subs_idx) {
  std::vector<Index> buckets;
  get_symbol_buckets(seq, buckets, n, k);
  std::vector<Index> bucket_it(k, 0);

  // First sweep
  for (Index i = 0; i < k; ++i) {
    bucket_it[i] = buckets[i+1] - 1;
  }
  for (Index i = 0; i < lms_subs_idx; ++i) {
    sa[bucket_it[seq[lms_subs_idx[i]]]] = lms_subs_idx[i];
    --bucket_it[seq[lms_subs_idx[i]]];
  }

  // Second sweep
  for (Index i = 0; i < k; ++i) {
    bucket_it[i] = buckets[i];
  }
  for (Index i = 0; i < n; ++i) {
    if (sa[i] > 0 && t[sa[i] - 1] == L) {
      bucket_it[seq[sa[i] - 1]] = sa[i] - 1;
      ++bucket_it[seq[sa[i] - 1]];
    }
  }

  // Third sweep
  for (Index i = 0; i < k; ++i) {
    bucket_it[i] = buckets[i+1] - 1;
  }
  for (Index i = n - 1; i >= 0; --i) {
    if (sa[i] > 0 && t[sa[i] - 1] == S) {
      bucket_it[seq[sa[i] - 1]] = sa[i] - 1;
      --bucket_it[seq[sa[i] - 1]];
    }
  }
}

}  // namespace

namespace suffixarray {

int sa_is(const Symbol seq[], Index sa[], Index n, Symbol k) {
  if (seq == NULL || sa == NULL || n < 1 || k < 1) return -1;

  // Classify symbols as S- or L- type
  // And find LMS substring indices.
  std::vector<SymbolType> t(n, S);
  std::vector<Index> lms_subs_idx;
  for (Index i = n - 2; i >= 0; --i) {
    if (seq[i] == seq[i+1]) {
      t[i] = t[i+1];
    } else if (seq[i] < seq[i+1]) {
      t[i] = S;
    } else {
      t[i] = L;
      if (t[i+1] == S && i+1 != n-1)
        lms_subs_idx.push_back(i+1);
    }
    // Initial sa values for next step
    sa[i] = -1;
  }
  sa[n-1] = -1;
  // LMS substring indices are in decreasing order and are missing the
  // terminating character index.
  std::reverse(lms_subs_idx.begin(), lms_subs_idx.end());
  lms_subs_idx.push_back(n-1);

  // LMS substring induced sort
  lms_induced_sort(seq, sa, n, k, t, lms_subs_idx);

  // Rename LMS substrings

  // If LMS substring names unique, compute SA1 from LMS substrings
  // Else solve recursively

  // Induce SA from SA1

  return 0;
}

}  // namespace suffixarray
