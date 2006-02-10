#ifndef TrackEvent_SHacks_H
#define TrackEvent_SHacks_H 1

#include "Kernel/GenericVectorTypes.h"
#include "Kernel/GenericMatrixTypes.h"
#include "Kernel/SymmetricMatrixTypes.h"

// Hacks to SMatrix to give CLHEP functionality

namespace SHacks {
  
  // similarity a la CLHEP 
  template <typename TYPE1, typename TYPE2>
  TYPE2 Similarity(TYPE1 F, TYPE2 C); 
  
  // similarity a la CLHEP 
  template <unsigned int N, typename TYPE>
  double Similarity(ROOT::Math::SVector<double,N> V, TYPE C);
  
  // hack to symmetrize a square matrix
  template <unsigned int N>
  void Symmetrize( ROOT::Math::SMatrix<double,N,N> M );
  
};

template <typename TYPE1, typename TYPE2>
inline TYPE2 SHacks::Similarity(TYPE1 F, TYPE2 C) {
  return F*C*ROOT::Math::Transpose(F);
};

template <unsigned int N, typename TYPE>
inline double SHacks::Similarity(ROOT::Math::SVector<double,N> V, TYPE C) {
  return ROOT::Math::Product( V, C );
};

template <unsigned int N>
void Symmetrize( ROOT::Math::SMatrix<double,N,N> M ) 
{
  for ( unsigned int i = 0; i < M.kCols; ++i ) {
    for ( unsigned int j = i+1; j < M.kCols; ++j ) {
      M(j,i) = M(i,j);
    }
  }
};

#endif
