
//-----------------------------------------------------------------------------
/** @file RichRecPixelParentType.cpp
 *
 *  Implementation ile for RichRecPixel enumerations
 *
 *  CVS Log :-
 *  $Id: RichRecPixelParentType.cpp,v 1.5 2004-10-30 19:24:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.4  2004/08/19 14:07:41  jonrob
 *  Remove dependency on RichDet + doxygen updates
 *
 *  Revision 1.3  2004/08/17 11:30:48  jonrob
 *  Update printout messages for enumerations
 *
 *  Revision 1.2  2004/07/26 18:00:58  jonrob
 *  Various improvements to the doxygen comments
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
    case Rich::PixelParent::RawBuffer:   return "RawBuffer";
    case Rich::PixelParent::Digit:       return "RichDigit";
    case Rich::PixelParent::MCHit:       return "MCRichHit";
    default:                             return "SHOULD NEVER SEE THIS";
    }
}
