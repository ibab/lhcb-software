// $Id: RichRecPhoton.cpp,v 1.2 2003-08-06 09:55:31 jonrob Exp $
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

  m_expPixelSignalPhots.resetData();
  m_expPixelScattPhots.resetData();

}
