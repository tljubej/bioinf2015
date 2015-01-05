// Copyright 2015. Authors, see LICENSE file
//
// Declaration of MEMFinder implementation based upon
// bioinformatics.oxfordjournals.org/content/25/13/1609.long
//

#ifndef BIOINF2015_MEM_MEM_IMPL_H
#define BIOINF2015_MEM_MEM_IMPL_H

#include <string>
#include <vector>

#include "mem/mem.h"

namespace mem {

// Implementation of MEMFinder class based upon aforementioned
// alghorithm.
class MEMFinderImpl : public MEMFinder {
 public:
  int FindMEMs(const ReferenceString& ref, const std::string&  query,
      Index l, std::vector<MEM>* mems) override;
};

}  // namespace mem

#endif  // BIOINF2015_MEM_MEM_IMPL_H
