
//-----------------------------------------------------------------------------
/** @file RichRecPixelParentType.cpp
 *
 *  Implementation ile for RichRecPixel enumerations
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecPixelParentType.h"

// Text conversion for Rich::RecPixel::ParentType enumeration
std::string Rich::Rec::text( const Rich::Rec::PixelParent::Type parent )
{
  switch( parent )
  {
  case Rich::Rec::PixelParent::RawBuffer:   return "RawBuffer";
  case Rich::Rec::PixelParent::Digit:       return "RichDigit";
  case Rich::Rec::PixelParent::MCHit:       return "MCRichHit";
  case Rich::Rec::PixelParent::NoParent:    return "No Parent";
  default:                                  return "SHOULD NEVER SEE THIS";
  }
}
