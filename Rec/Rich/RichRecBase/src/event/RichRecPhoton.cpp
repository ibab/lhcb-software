
//-----------------------------------------------------------------------------
/** @file RichRecPhoton.cpp
 *
 *  Implementation file for class : LHCb::RichRecPhoton
 *
 *  CVS Log :-
 *  $Id: RichRecPhoton.cpp,v 1.7 2008-06-14 10:39:46 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "Event/RichRecPhoton.h"

// Reset information to force recalculation
void LHCb::RichRecPhoton::reset()
{
  // reset the data values
  expPixelSignalPhots().resetData(0);
  expPixelScattPhots().resetData(0);
}
