// $Id: RichDetector.cpp,v 1.1 2002-06-21 15:34:03 jonesc Exp $
// Include files 

// local
#include "RichKernel/RichDetector.h"

//-----------------------------------------------------------------------------
// Implementation file for enumeration : Rich::Detector
//
// 2002-06-19 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Text conversion for Detector enumeration
std::string Rich::text( const Rich::Detector& detector ) {
  switch( detector ) {
  case Rich::Rich1:   return "Rich1";
  case Rich::Rich2:   return "Rich2";
  default:            return "?"; // should never happen
  }
}
