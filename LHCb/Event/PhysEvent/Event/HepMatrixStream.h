#ifndef EVENT_HEPMATRIX_H
#define EVENT_HEPMATRIX_H 1


// Include files
#include "GaudiKernel/StreamBuffer.h"
#include "LHCbEvent/Definitions.h"


#include "CLHEP/Matrix/Matrix.h"

// Output operator
inline StreamBuffer& operator<< ( StreamBuffer& s, const HepMatrix& value ) {
  int   nrow = value.num_row();
  int   ncol = value.num_col();
  for( int i=1; i<=nrow; i++ ) {
    for( int j=1; j<=ncol; j++ ) {
      s << float(value(i,j));
    }
  }
  return s;
}

// Input operator
inline StreamBuffer& operator>> ( StreamBuffer& s, HepMatrix& value )       {
  float _val;
  int   nrow = value.num_row();
  int   ncol = value.num_col();
  for( int i=1; i<=nrow; i++ ) {
    for( int j=1; j<=ncol; j++ ) {
      s >> _val;
      value(i,j) = _val;
    }
  }
  return s;
}

#endif    // EVENT_HEPMATRIX
