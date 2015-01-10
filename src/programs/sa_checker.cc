// Copyright 2015. Authors, see LICENSE file
//
// A manual testing tool for Suffix array correctness.
// Meant for use on smaller reference strings.
//

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <memory>
#include <string>

#include "input_util.h"
#include "types.h"

// Compares suffixes of reference string str of length n ('\0' included)
// at s1 and s2.
// Returns 0 if suffix at s1 is lesser than suffix at s2,
// or 1 if not.
int compare_suffixes(const char str[], Index n, Index s1, Index s2) {
  Index greater_idx = std::max(s1, s2);
  for (Index i = 0; greater_idx + i < n; ++i) {
    if (str[s1 + i] > str[s2 + i]) {
      return 1;
    } else if (str[s1 + i] < str[s2 + i]) {
      return 0;
    }
  }
  // No need to compare lengths, terminating character is '\0'
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Arguments are: <refstr_file> <sa_file>\n");
    return 1;
  }
  std::unique_ptr<std::string> refstr(input_util::read_fasta(argv[1]));
  if (refstr.get() == nullptr) {
    fprintf(stderr, "Failed to read reference string file.\n");
    return 1;
  }
  Index n;
  std::unique_ptr<Index[]> sa(input_util::read_sa(argv[2], &n));
  if (sa.get() == nullptr) {
    fprintf(stderr, "Failed to read sa file.\n");
    return 1;
  }
  const char* str = refstr->c_str();
  for (Index i = 0; i < n-1; ++i) {
    if (compare_suffixes(str, n, sa[i], sa[i+1])) {
      printf("Invalid SA ordering!\n"
             "SA indices: %d %d\n"
             "S locations: %d %d\n"
             "%s\n"
             "%s\n\n",
             i, i+1, sa[i], sa[i+1], str + sa[i], str + sa[i+1]);
    }
  }
  return 0;
}
