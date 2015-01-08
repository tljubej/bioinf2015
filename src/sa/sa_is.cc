//  Copyright 2014. Authors, see LICENSE file
//
//  This file contains implementations of Suffix Array - Induced Sorting
//  (SA-IS) methods.
//

#include "sa/sa_is.h"

#include <vector>
#include <algorithm>

namespace suffixarray {
// Namespace for local methods.
namespace {

typedef char SymbolType;
const char S = 2;
const char L = 1;

// Returns true if symbol at index idx is
// a LMS symbol according to symbol_types.
inline bool isLMS(const std::vector<SymbolType>& symbol_types, Index idx) {
  return idx > 0 && symbol_types[idx] == S && symbol_types[idx-1] == L;
}

// Fills buckets with indicices of beginning or end, both inclusive,
// of corresponding symbol buckets for Symbol sequence seq of length n
// and alphabet of size k.
template <typename Symbol>
void get_symbol_buckets(const Symbol seq[], Index n, Symbol k, bool begin,
    std::vector<Index>* buckets) {
  buckets->resize(k, 0);
  for (Index i = 0; i < n; ++i) {
    ++buckets->at(seq[i]);
  }
  Index count = 0;
  for (Symbol i = 0; i < k; ++i) {
    count += buckets->at(i);
    if (begin) {
      buckets->at(i) = count - buckets->at(i);
    } else {
      buckets->at(i) = count - 1;
    }
  }
}

// First sweep of induced sort of LMS substrings where seq is the symbol
// sequence of length n and alphabet of size k, sa is se suffix array that
// is filled, and t is a vector of corresponding symbol types.
template <typename Symbol>
void lms_induced_sort_1(const Symbol seq[], Index sa[], Index n, Symbol k,
    const std::vector<SymbolType>& t) {
  std::vector<Index> bucket_it;
  get_symbol_buckets(seq, n, k, false, &bucket_it);
  for (Index i = 0; i < n; ++i) {
    if (isLMS(t, i)) {
      sa[bucket_it[seq[i]]] = i;
      --bucket_it[seq[i]];
    }
  }
}

// Second sweep of induced sort of LMS substrings where seq is the symbol
// sequence of length n and alphabet of size k, sa is se suffix array that
// is filled, and t is a vector of corresponding symbol types.
template <typename Symbol>
void lms_induced_sort_2(const Symbol seq[], Index sa[], Index n, Symbol k,
    const std::vector<SymbolType>& t) {
  std::vector<Index> bucket_it;
  get_symbol_buckets(seq, n, k, true, &bucket_it);
  for (Index i = 0; i < n; ++i) {
    if (sa[i] > 0 && t[sa[i] - 1] == L) {
      sa[bucket_it[seq[sa[i] - 1]]] = sa[i] - 1;
      ++bucket_it[seq[sa[i] - 1]];
    }
  }
}

// Third sweep of induced sort of LMS substrings where seq is the symbol
// sequence of length n and alphabet of size k, sa is se suffix array that
// is filled, and t is a vector of corresponding symbol types.
template <typename Symbol>
void lms_induced_sort_3(const Symbol seq[], Index sa[], Index n, Symbol k,
    const std::vector<SymbolType>& t) {
  std::vector<Index> bucket_it;
  get_symbol_buckets(seq, n, k, false, &bucket_it);
  for (Index i = n - 1; i >= 0; --i) {
    if (sa[i] > 0 && t[sa[i] - 1] == S) {
      sa[bucket_it[seq[sa[i] - 1]]] = sa[i] - 1;
      --bucket_it[seq[sa[i] - 1]];
    }
  }
}

// Comparator for LMS strings contained in seq, with first_idx
// and second_idx being string start indices, and first_len and
// second_len being lengths respectively and symbol_types containing
// symbol types for symbols contained in seq.
// LMS strings are compared symbol by symbol first lexically, and
// then by symbol types with S- considered to come lexically after L-.
//
// Returns a negative integer, zero, or a positive integer as the first
// string is lesser than, equal to, or greater than the second.
template <typename Symbol>
int compare_lms(const Symbol seq[], const std::vector<SymbolType>& symbol_types,
    Index first_idx, Index second_idx) {
  for (Index i = 0;; ++i) {
    if (seq[i+first_idx] < seq[i+second_idx])
      return -1;
    if (seq[i+first_idx] > seq[i+second_idx])
      return 1;
    if (symbol_types[i+first_idx] == L && symbol_types[i+second_idx] == S)
      return -1;
    if (symbol_types[i+first_idx] == S && symbol_types[i+second_idx] == L)
      return 1;
    if (i > 0) {
      if (isLMS(symbol_types, i+first_idx)) {
        if (!isLMS(symbol_types, i+second_idx))
          return -1;
        else
          return 0;
      } else if (isLMS(symbol_types, i+second_idx)) {
        return 1;
      }
    }
  }
  return 0;
}

// Internal implementation for sa_is that allows templating for the purpose
// of type changing in recursive calls.
template <typename Symbol>
int sa_is_internal(const Symbol seq[], Index sa[], Index n, Symbol k) {
  if (seq == NULL || sa == NULL || n < 1 || k < 1) return -1;

  // Classify symbols as S- or L- type
  // And find LMS substring indices.
  std::vector<SymbolType> symbol_types(n, S);
  for (Index i = n - 2; i >= 0; --i) {
    if (seq[i] == seq[i+1]) {
      symbol_types[i] = symbol_types[i+1];
    } else if (seq[i] < seq[i+1]) {
      symbol_types[i] = S;
    } else {
      symbol_types[i] = L;
    }
    // Initial sa values for next step
    sa[i] = -1;
  }
  sa[n-1] = -1;
  // LMS substring indices are in decreasing order and are missing the
  // terminating character index.

  // LMS substring induced sort.
  lms_induced_sort_1<Symbol>(seq, sa, n, k, symbol_types);
  lms_induced_sort_2<Symbol>(seq, sa, n, k, symbol_types);
  lms_induced_sort_3<Symbol>(seq, sa, n, k, symbol_types);

  // Move LMS indices forward to make room for S1 in SA memory.
  Index lms_count = 0;
  for (Index i = 0; i < n; ++i) {
    if (isLMS(symbol_types, sa[i])) {
      sa[lms_count++] = sa[i];
      if (lms_count-1 != i)
        sa[i] = -1;
    } else {
      sa[i] = -1;
    }
  }

  // Rename LMS substrings
  Index current_name = 0;
  Index* s1 = sa + lms_count;
  s1[sa[0]/2] = current_name;
  for (Index i = 1; i < lms_count; ++i) {
    if (compare_lms<Symbol>(seq, symbol_types, sa[i-1], sa[i]) != 0) {
      ++current_name;
    }
    // We divide the index by two so it fits inside half of SA memory,
    // while preserving LMS ordering but assigning new names.
    s1[sa[i]/2] = current_name;
  }
  // Move new LMS names forward to have them sequential in memory.
  for (Index i = 0, curr = 0; i < n-lms_count; ++i) {
    if (s1[i] != -1) {
      s1[curr++] = s1[i];
    }
  }
  // If LMS substring names unique, compute SA1 from LMS substrings
  // Else solve recursively
  if (lms_count > current_name + 1) {
    // SA1 memory starts at the beggining of SA memory.
    sa_is_internal<Index>(s1, sa, lms_count, current_name + 1);
  } else {
    for (Index i = 0; i < lms_count; ++i) {
      sa[s1[i]] = i;
    }
  }
  // Induce SA from SA1
  {
    std::vector<Index> buckets;
    get_symbol_buckets(seq, n, k, false, &buckets);
    // Store P1 into S1 which is no longer needed.
    for (Index i = 0, j = 0; i < n; ++i) {
      if (isLMS(symbol_types, i)) {
        s1[j++] = i;
      }
    }
    std::vector<Index> tmp_index_holder(lms_count, 0);
    // Put P1[SA1[i]] values in a temporary holder to
    // free up SA memory from other uses.
    for (Index i = 0; i < lms_count; ++i) {
      tmp_index_holder[i] = s1[sa[i]];
      sa[i] = -1;
    }
    // Init the rest of the sa.
    for (Index i = lms_count; i < n; ++i) {
      sa[i] = -1;
    }
    for (Index i = lms_count-1; i >= 0; --i) {
      sa[buckets[seq[tmp_index_holder[i]]]] = tmp_index_holder[i];
      --buckets[seq[tmp_index_holder[i]]];
    }
  }
  lms_induced_sort_2<Symbol>(seq, sa, n, k, symbol_types);
  lms_induced_sort_3<Symbol>(seq, sa, n, k, symbol_types);
  return 0;
}

}  // namespace

int sa_is(const char seq[], Index sa[], Index n, char k) {
  return sa_is_internal<char>(seq, sa, n, k);
}

}  // namespace suffixarray
