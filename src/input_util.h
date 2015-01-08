// Copyright 2015. Authors, see LICENSE file
//
// Several utility functions to help with input.
//

#ifndef BIOINF2015_INPUT_UTIL_H_
#define BIOINF2015_INPUT_UTIL_H_

#include <string>

#include "types.h"

namespace input_util {

// Reads in genome as string from FASTA format in file.
// Returns ownership of resulting string or nullptr
// in case of errors.
std::string* read_fasta(const char* filename);

// Reads SA from file in binary format And
// returns ownership of it, storing SA size in *n.
// Returns nullptr in case of errors.
Index* read_sa(const char* filename, Index* n);

}  // namespace input_util

#endif  // BIOINF2015_INPUT_UTIL_H_
