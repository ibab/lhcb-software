
#ifndef LESTER_GENERICHITINDEX_H
#define LESTER_GENERICHITINDEX_H

// fwd dec
#include "GenericRingFinder/GenericHitIndex.fwd" 

// includes
#include "GenericRingFinder/Index.h"

namespace GenRingF
{

  // declaration
  class GenericHitIndex : public Index<unsigned int,GenericHitIndex> {
  public:
    explicit GenericHitIndex(const unsigned int i) : Index<unsigned int,GenericHitIndex>(i) {};
  };
  
}

#endif

