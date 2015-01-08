// Copyright 2015. Authors, see LICENSE file
//
// Several utility functions to help with input.
//

#ifndef BIOINF2015_INPUT_UTIL_H_
#define BIOINF2015_INPUT_UTIL_H_


#include <string>

// Reads in genome as string from FASTA format in file.
// Returns ownership of resulting string.
std::string* read_fasta(const char* filename);

#endif  // BIOINF2015_INPUT_UTIL_H_
