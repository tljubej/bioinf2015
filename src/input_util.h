// Copyright 2015. Authors, see LICENSE file
//
// Several utility functions to help with input.
//

#ifndef BIOINF2015_INPUT_UTIL_H_
#define BIOINF2015_INPUT_UTIL_H_

#include <string>
#include <fstream>

#include "types.h"

namespace input_util {

// Reads in genome as string from FASTA format in file.
// Assumes only one genome is in the file.
// Returns ownership of resulting string or nullptr
// in case of errors.
std::string* read_fasta(const char* filename);

// Reads SA from file in binary format And
// returns ownership of it, storing SA size in *n.
// Returns nullptr in case of errors.
Index* read_sa(const char* filename, Index* n);

// Reads through a FASTA file with multiple genomes and returns 
// them as strings, one at a time.
class FastqReader {
 public:

  // Construct the reader which will read from given filename
  FastqReader(std::string const & filename);

  // Returns the next sequence in the file, or nullptr if EOF
  std::string* next_sequence();

  // Rewinds the reader to read from the beginning of file again
  void rewind();

 private:
  std::ifstream ifs_;
};

}  // namespace input_util

#endif  // BIOINF2015_INPUT_UTIL_H_
