
//--------------------------------------------------------------------------------
/** @file RichSmartIDCnv.cpp
 *
 *  Implementation file for RichSmartID conversion utilities
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
            << " pdX="  << globalPdX()
            << " pdY="  << globalPdY()
            << " pixX=" << globalPixelX()
            << " pixY=" << globalPixelY()
            << " ]";
}
