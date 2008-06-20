
#ifndef LESTER_GENERICRINGINDEX_H
#define LESTER_GENERICRINGINDEX_H

// fwd dec
#include "GenericRingFinder/GenericRingIndex.fwd" 

// includes
#include "GenericRingFinder/Index.h" 

namespace GenRingF
{

  // declaration
  class GenericRingIndex : public Index<unsigned int,GenericRingIndex> {
  public:
    explicit GenericRingIndex(const unsigned int i) : Index<unsigned int,GenericRingIndex>(i) {};
  };

}

#endif

