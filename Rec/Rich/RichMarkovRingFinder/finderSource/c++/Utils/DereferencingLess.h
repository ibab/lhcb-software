
#ifndef LESTER_DEREFERENCING_LESS_H
#define LESTER_DEREFERENCING_LESS_H

// fwd dec
#include "Utils/DereferencingLess.fwd" 

// includes

// declaration
//template<class PointerType>
class DereferencingLess {
public:
  template<class PointerType>
  bool operator()(const PointerType a, const PointerType b) const {
    // Should return true if a preceeds b and false otherwise.
    return ((*a) < (*b));
  };
};


#endif

