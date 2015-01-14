// Copyright 2015. Authors, see LICENSE file
//
// Several utility functions to help with input.
//

#include "input_util.h"

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <memory>
#include <string>

#include "types.h"

namespace input_util {

namespace {

const size_t kBufferSize = 512;

// Returns size of the file in bytes.
size_t get_file_size(FILE* file) {
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);
  return size;
}

}  // namespace

std::string* read_fasta(const char* filename) {
  FILE* fasta_file = fopen(filename, "r");
  if (fasta_file == NULL)
    return nullptr;
  size_t size = get_file_size(fasta_file);
  std::unique_ptr<std::string> buffer(new std::string());
  buffer->reserve(size);
  char line[kBufferSize];
  fgets(line, kBufferSize, fasta_file);
  while (fgets(line, kBufferSize, fasta_file)) {
      size_t len = strlen(line);
      if (line[len-1] == '\n')
        line[len-1] = '\0';
      buffer->append(line);
  }
  fclose(fasta_file);
  return buffer.release();
}

Index* read_sa(const char* filename, Index* n) {
  FILE* sa_file = fopen(filename, "rb");
  if (sa_file == NULL)
    return nullptr;
  *n = get_file_size(sa_file) / sizeof(Index);
  std::unique_ptr<Index[]> sa(new Index[*n]);
  size_t read = fread(sa.get(), sizeof(Index), *n, sa_file);
  if ((Index)read != *n)
    return nullptr;
  return sa.release();
}

FastqReader::FastqReader(std::string const &filename) : ifs_(filename) {
}

std::string* FastqReader::next_sequence() {
  if (!ifs_.good())
    return nullptr;

  std::string linebuf;

  std::unique_ptr<std::string> seq(new std::string());

  // the identifier line
  std::getline(ifs_, linebuf);
  if (ifs_.eof())
    return nullptr;

  while (ifs_.peek() != '+' && ifs_.peek() != '@' && !ifs_.eof()) {
    // line that is part of sequence
    std::getline(ifs_, linebuf);
    *seq += linebuf;
  }

  while (ifs_.peek() != '@' && !ifs_.eof()) {
    // quality lines
    std::getline(ifs_, linebuf);
  }

  return seq.release();
}

void FastqReader::rewind() {
  ifs_.seekg(0);
}

}  // namespace input_util
