// Copyright 2015. Authors, see LICENSE file
//
// A simple executable that takes genetic data in FASTA format
// from the file given as the only command line argument, and outputs
// the corresponding suffix array to standard output.
//

#include <stdio.h>

#include <memory>
#include <string>

#include "input_util.h"
#include "sa/sa_is.h"
#include "types.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Missing filename with FASTA formatted data.");
    return -1;
  }
  std::unique_ptr<std::string> refstr(read_fasta(argv[1]));
  if (refstr.get() == nullptr) {
    fprintf(stderr, "Failed to read FASTA file.");
    return -1;
  }
  Index n = refstr->size()+1;
  std::unique_ptr<Index[]> sa(new Index[n]);
  suffixarray::sa_is(refstr->c_str(), sa.get(), n, 127);
  for (Index i = 0; i < n; ++i) {
    printf("%d ", sa[i]);
  }
  printf("\n");
  return 0;
}
