// $Id: RichAlgorithmType.cpp,v 1.1 2002-06-21 15:33:57 jonesc Exp $
// Include files 

// local
#include "RichKernel/RichAlgorithmType.h"

//-----------------------------------------------------------------------------
// Implementation file for enumeration : Rich::AlgorithmType
//
// 2002-06-19 : Chris Jones
//-----------------------------------------------------------------------------

// Text conversion for ParticleIDType enumeration
std::string Rich::text( const Rich::AlgorithmType& algorithm ) {
  switch( algorithm ) {
  case Rich::SummaryAlgorithm: return "summary";
  case Rich::GlobalAlgorithm:  return "global";
  case Rich::LocalAlgorithm:   return "local";
  default:                     return "?"; // should never happen
  }
}
