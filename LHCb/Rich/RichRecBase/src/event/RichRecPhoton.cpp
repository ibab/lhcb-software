// $Id: RichRecPhoton.cpp,v 1.3 2004-06-03 16:19:42 jonesc Exp $
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

  // reset the data values
  m_expPixelSignalPhots.resetData(0);
  m_expPixelScattPhots.resetData(0);

}
