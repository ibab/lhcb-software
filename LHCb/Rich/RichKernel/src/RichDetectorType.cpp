// $Id: RichDetectorType.cpp,v 1.1 2003-04-15 16:10:33 jonrob Exp $
// Include files 

// local
#include "RichKernel/RichDetectorType.h"

//-----------------------------------------------------------------------------
// Implementation file for enumeration : Rich::Detector
//
// 2002-06-19 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Text conversion for Detector enumeration
std::string Rich::text( const Rich::DetectorType& detector ) {
  switch( detector ) {
  case Rich::Rich1:   return "Rich1";
  case Rich::Rich2:   return "Rich2";
  default:            return "?"; // should never happen
  }
}
