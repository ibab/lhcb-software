#ifndef TrackEvent_SHacks_H
#define TrackEvent_SHacks_H 1

#include "Kernel/GenericVectorTypes.h"
#include "Kernel/GenericMatrixTypes.h"
#include "Kernel/SymmetricMatrixTypes.h"

// Hacks to SMatrix to give CLHEP functionality

namespace SHacks{

 // similarity a la CLHEP 
 template <typename TYPE1, typename TYPE2>
 TYPE2 Similarity(TYPE1 F, TYPE2 C); 

 // similarity a la CLHEP 
 template <unsigned int N, typename TYPE>
 double Similarity(ROOT::Math::SVector<double,N> V, TYPE C);

};

template <typename TYPE1, typename TYPE2>
inline TYPE2 SHacks::Similarity(TYPE1 F, TYPE2 C) {
  return F*C*ROOT::Math::Transpose(F);
};

template <unsigned int N, typename TYPE>
inline double SHacks::Similarity(ROOT::Math::SVector<double,N> V, TYPE C) {
  return ROOT::Math::Product( V, C );
};

#endif
