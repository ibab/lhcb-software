#ifndef TrackEvent_SHacks_H
#define TrackEvent_SHacks_H 1

#include "Kernel/GenericMatrixTypes.h"
#include "Kernel/SymmetricMatrixTypes.h"

// Hacks to SMatrix to give CLHEP functionality

namespace SHacks{

 // similarity a la CLHEP 
 template <typename TYPE1, typename TYPE2>
 TYPE2 Similarity(TYPE1 F, TYPE2 C); 

};

template <typename TYPE1, typename TYPE2>
inline TYPE2 SHacks::Similarity(TYPE1 F, TYPE2 C){
  return F*C*ROOT::Math::Transpose(F);
}

#endif
