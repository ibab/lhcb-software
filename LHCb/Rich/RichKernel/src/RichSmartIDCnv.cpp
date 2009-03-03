
//--------------------------------------------------------------------------------
/** @file RichSmartIDCnv.cpp
 *
 *  Implementation file for RichSmartID conversion utilities
 *
 *  CVS Log :-
 *  $Id: RichSmartIDCnv.cpp,v 1.6 2009-03-03 12:01:00 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   05/02/2008
 *  @edits: Young Min Kim (s0679231@sms.ed.ac.uk) on 13/06/2008
 */
//--------------------------------------------------------------------------------

// local
#include "RichKernel/RichSmartIDCnv.h"

MsgStream&
Rich::SmartIDGlobalOrdering::fillStream( MsgStream& os ) const
{
  return os << "["
            << " hpdX=" << globalHpdX()
            << " hpdY=" << globalHpdY()
            << " pixX=" << globalPixelX()
            << " pixY=" << globalPixelY()
            << " ]";
}
