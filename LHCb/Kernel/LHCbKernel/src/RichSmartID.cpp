
//-----------------------------------------------------------------------------
/** @file RichSmartID.cpp
 *
 *  Implementation file for RICH Channel ID class : RichSmartID
 *
 *  $Id: RichSmartID.cpp,v 1.3 2005-02-09 09:08:37 cattanem Exp $
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
    if ( pdColIsSet()    ) s << " PDCol"  << format("%3i",pdCol());
    if ( pdRowIsSet()    ) s << " PDRow"  << format("%3i",pdRow());
    if ( pixelColIsSet() ) s << " pixCol" << format("%3i",pixelCol());
    if ( pixelRowIsSet() ) s << " pixRow" << format("%3i",pixelRow());
    if ( subPixelIsSet() ) s << " subPix" << format("%2i",subPixel());
    s << " }";

  } else {

    // This SmartID has no valid bits set. This is in correct.
    s << "WARNING : Invalid RichSmartID";

  }

  return s;
}
