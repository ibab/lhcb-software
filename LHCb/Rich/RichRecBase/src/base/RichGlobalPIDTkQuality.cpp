// $Id: RichGlobalPIDTkQuality.cpp,v 1.1.1.1 2003-04-01 13:21:54 jonesc Exp $
// Include files 

// local
#include "RichRecBase/RichGlobalPIDTkQuality.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichGlobalPIDTkQuality
//
// 2002-07-12 : Chris Jones
//-----------------------------------------------------------------------------

// Text conversion for Rich::RecTrack::ParentType enumeration
std::string Rich::text( const Rich::GlobalPID::TkQuality& track ) {
  switch( track ) {
  case Rich::GlobalPID::Unknown:         return "unknown";
  case Rich::GlobalPID::Unusable:        return "unusable";
  case Rich::GlobalPID::LikelihoodOnly:  return "likelihood only";
  case Rich::GlobalPID::Physics:         return "physics";
  default:                               return "?"; // should never happen
  }
}
