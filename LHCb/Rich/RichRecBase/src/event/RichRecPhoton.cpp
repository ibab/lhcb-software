
//-----------------------------------------------------------------------------
/** @file RichRecPhoton.cpp
 *
 *  Implementation file for class : RichRecPhoton
 *
 *  CVS Log :-
 *  $Id: RichRecPhoton.cpp,v 1.5 2005-11-15 13:32:16 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "Event/RichRecPhoton.h"

// Reset information to force recalculation
void RichRecPhoton::reset()
{
  // reset the data values
  expPixelSignalPhots().resetData(0);
  expPixelScattPhots().resetData(0);
}
