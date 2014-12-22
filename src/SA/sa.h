// Copyright 2014. Authors
//
// Contains general type definitions and useful utility methods for working
// with suffix arrays.
//
// Author: Fran Jurisic (fran.jurisic@fer.hr)
//

#ifndef BIOINF2015_SA_SA_H_
#define BIOINF2015_SA_SA_H_

namespace suffixarray {

  // Used to represent alphabet symbols encoded
  // to values {0, .., k-1} where k is the number
  // of different symbols in the alphabet.
  // 0 is reserved for termination character.
  typedef Symbol unsigned int16;

  // Represents indices.
  typedef Index int;

}  // namespace suffixarray

#endif  // BIOINF2015_SA_SA_H_
