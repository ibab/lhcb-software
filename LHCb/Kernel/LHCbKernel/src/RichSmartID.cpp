
//-----------------------------------------------------------------------------
/** @file RichSmartID.cpp
 *
 *  Implementation file for RICH Channel ID class : RichSmartID
 *
 *  CVS Log :-
 *  $Id: RichSmartID.cpp,v 1.1 2005-01-07 12:28:56 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    2005-01-06
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/RichSmartID.h"

std::ostream& RichSmartID::fillStream(std::ostream& s) const
{

  // Is this smart ID valid
  if ( isValid() ) {

    // If so, print out only the explicitly set fields
    s << "{";
    if ( richIsSet()     ) s << " "       << rich();
    if ( panelIsSet()    ) s << " Panel " << static_cast<unsigned int>(panel());
    if ( PDRowIsSet()    ) s << " PDRow"  << format("%3i",PDRow());
    if ( PDColIsSet()    ) s << " PDCol"  << format("%3i",PDCol());
    if ( pixelRowIsSet() ) s << " pixRow" << format("%3i",pixelRow());
    if ( pixelColIsSet() ) s << " pixCol" << format("%3i",pixelCol());
    if ( subPixelIsSet() ) s << " subPix" << format("%2i",subPixel());
    s << " }";

  } else {

    // This SmartID has no valid bits set. This is in correct.
    s << "WARNING : Invalid RichSmartID";

  }

  return s;
}
