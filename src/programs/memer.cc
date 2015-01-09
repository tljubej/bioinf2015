// Copyright 2015. Authors, see LICENSE file
//
// Executable that reads the reference string from the
// file given as first command line argument, corresponding
// suffix array from the file in binary format given as second
// argument, query string from the file given as third argument,
// values K and L as the fourth and fifth arguments, and outputs
// all found maximum exact matches accordingly.
//

#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <string>
#include <vector>

#include "input_util.h"
#include "mem/mem.h"
#include "mem/mem_impl.h"
#include "reference_string.h"
#include "types.h"

int main(int argc, char* argv[]) {
  if (argc < 6) {
    fprintf(stderr, "Arguments are: <refstr_file> <sa_file> "
                    "<query_strings_file> <K> <L>.\n");
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
  Index k = atoi(argv[4]);
  Index l = atoi(argv[5]);
  ReferenceString ref(refstr.get(), sa.release(), k);

  //TODO(Fran): Add query string input and action. The next part are dummy
  //skeletons.
  std::string query;
  mem::MEMFinderImpl mem_finder;   
  std::vector<mem::MEM> mems;
  mem_finder.find_mems(ref, query, l, &mems);
  return 0;
}
