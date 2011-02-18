
//-----------------------------------------------------------------------------
/** @file RichSmartID.cpp
 *
 *  Implementation file for RICH Channel ID class : RichSmartID
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    2005-01-06
 */
//-----------------------------------------------------------------------------

// STL
#include <sstream>

// Gaudi
#include "GaudiKernel/GaudiException.h"

// local
#include "Kernel/RichSmartID.h"

std::ostream& LHCb::RichSmartID::fillStream(std::ostream& s) const
{

  // Is this smart ID valid
  if ( isValid() ) 
  {
    // If so, print out only the explicitly set fields
    s << "{";
    if ( richIsSet()        ) s << " "            << Rich::text( rich() );
    if ( panelIsSet()       ) 
    {
      const std::string PANEL 
        = ( rich() == Rich::Rich1 ?
            ( panel() == Rich::top  ? "Top     " : "Bottom  " ) :
            ( panel() == Rich::left ? "Left(A) " : "Right(C)" ) );
      s << " "            << PANEL;
    }
    if ( hpdColIsSet()      ) s << " HPDCol"      << format("%3i",hpdCol());
    if ( hpdNumInColIsSet() ) s << " HPDNumInCol" << format("%3i",hpdNumInCol());
    if ( pixelColIsSet()    ) s << " pixCol"      << format("%3i",pixelCol());
    if ( pixelRowIsSet()    ) s << " pixRow"      << format("%3i",pixelRow());
    if ( pixelSubRowIsSet() ) s << " pixSubRow"   << format("%2i",pixelSubRow());
    s << " }";
  } 
  else 
  {
    // This SmartID has no valid bits set. This is bad ...
    s << "WARNING : Invalid RichSmartID";
  }

  return s;
}

void LHCb::RichSmartID::rangeError(const int value,
                                   const int max,
                                   const std::string& message) const
{
  std::ostringstream mess;
  mess << message << " value " << value << " exceeds field maximum " << max;
  throw GaudiException ( mess.str(), "*RichSmartID*", StatusCode::FAILURE );
}

std::string LHCb::RichSmartID::toString() const
{
  std::ostringstream text;
  text << *this;
  return text.str();
}
