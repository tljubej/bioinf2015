// Copyright 2015. Authors, see LICENSE file
//
// Executable that reads the reference string from the
// file given as first command line argument, corresponding
// suffix array from the file in binary format given as second
// argument, query string from the file given as third argument,
// values K and L as the fourth and fifth arguments, and outputs
// all found maximum exact matches accordingly.
//

#include <cstdio>
#include <cstdlib>

#include <chrono>
#include <iostream>
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

  auto start = std::chrono::system_clock::now();
  std::unique_ptr<Index[]> sa(input_util::read_sa(argv[2], &n));
  if (sa.get() == nullptr) {
    fprintf(stderr, "Failed to read sa file.\n");
    return 1;
  } else {
    std::cerr << "loaded SA file in " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - start)
        .count() << " ms \n";
  }

  Index k = atoi(argv[4]);
  Index l = atoi(argv[5]);
  start = std::chrono::system_clock::now();
  ReferenceString ref(refstr.get(), sa.release(), k);
  std::cerr << "Made refstr in " <<
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now() - start)
      .count() << " ms \n";

  input_util::FastqReader query_reader(argv[3]);
  mem::MEMFinderImpl mem_finder;   
  std::vector<mem::MEM> mems;

  do {
    std::unique_ptr<std::string> query(query_reader.next_sequence());
    if (!query)
      break;

    start = std::chrono::system_clock::now();
    mem_finder.find_mems(ref, *query, l, &mems);
    std::cerr << "Found query \n" << *query << " in " <<
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - start)
        .count() << " ms \n";
    std::cerr << "\t" << mems.size() << " matches\n";


    for (mem::MEM m : mems) {
      std::cout << 
          m.length << "\t" <<
          m.query_string_idx << "\t" <<
          m.reference_string_idx << "\n";
    }

  } while (1);

  return 0;
}
