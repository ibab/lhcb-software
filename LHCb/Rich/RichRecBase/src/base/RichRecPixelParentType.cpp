
//-----------------------------------------------------------------------------
/** @file RichRecPixelParentType.cpp
 *
 *  Implementation ile for RichRecPixel enumerations
 *
 *  CVS Log :-
 *  $Id: RichRecPixelParentType.cpp,v 1.2 2004-07-26 18:00:58 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecPixelParentType.h"

// Text conversion for Rich::RecPixel::ParentType enumeration
std::string Rich::text( const Rich::PixelParent::Type & parent )
{
  switch( parent )
    {
    case Rich::PixelParent::Digit:       return "RichDigit";
    case Rich::PixelParent::MCHit:       return "MCRichHit";
    default:                             return "?"; // should never happen
    }
}
