
//-----------------------------------------------------------------------------
/** @file RichRecPhoton.cpp
 *
 *  Implementation file for class : RichRecPhoton
 *
 *  CVS Log :-
 *  $Id: RichRecPhoton.cpp,v 1.4 2004-07-26 18:00:58 jonrob Exp $
 *  $Log: not supported by cvs2svn $
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
  m_expPixelSignalPhots.resetData(0);
  m_expPixelScattPhots.resetData(0);

}
