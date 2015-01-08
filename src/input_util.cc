// Copyright 2015. Authors, see LICENSE file
//
// Several utility functions to help with input.
//

#include "input_util.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <memory>
#include <string>

std::string* read_fasta(const char* filename) {
  FILE* fasta_file = fopen(filename, "r");
  if (fasta_file == NULL)
    return nullptr;
  fseek(fasta_file, 0, SEEK_END);
  size_t size = ftell(fasta_file);
  std::unique_ptr<std::string> buffer(new std::string());
  buffer->reserve(size);
  fseek(fasta_file, 0, SEEK_SET);
  size_t max_line = 512;
  char line[max_line];
  fgets(line, max_line, fasta_file);
  while (fgets(line, max_line, fasta_file)) {
      size_t len = strlen(line);
      if (line[len-1] == '\n')
        line[len-1] = '\0';
      buffer->append(line);
  }
  fclose(fasta_file);
  return buffer.release();
}
