// $Id: RichRecPixelParentType.cpp,v 1.1.1.1 2003-04-01 13:21:55 jonesc Exp $
// Include files 

// local
#include "RichRecBase/RichRecPixelParentType.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecPixelParentType
//
// 2002-07-12 : Chris Jones
//-----------------------------------------------------------------------------

// Text conversion for Rich::RecPixel::ParentType enumeration
std::string Rich::text( const Rich::RecPixel::ParentType& parent ) {
  switch( parent ) {
  case Rich::RecPixel::Digit:          return "reconstructed digit";
  case Rich::RecPixel::MCHit:          return "MC hit";
  default:                             return "?"; // should never happen
  }
}
