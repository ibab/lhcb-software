// $Id: RichRecPhoton.cpp,v 1.1 2003-06-30 15:11:58 jonrob Exp $
// Include files 

// local
#include "Event/RichRecPhoton.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecPhoton
//                                 derived from class DataObject
//
// 2003-05-10 : Chris Jones
//-----------------------------------------------------------------------------

// Reset information to force recalculation
void RichRecPhoton::reset() {

  std::vector<float> resetVector( Rich::NParticleTypes, -1.0 );
  setExpPixelSignalPhots( resetVector );
  setExpPixelScattPhots( resetVector );

}
