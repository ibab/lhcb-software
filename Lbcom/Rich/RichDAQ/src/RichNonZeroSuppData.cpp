
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppData.cpp
 *
 *  Implementation file for RICH DAQ helper class : RichNonZeroSuppData
 *
 *  $Id: RichNonZeroSuppData.cpp,v 1.8 2006-03-01 09:56:12 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

// local
#include "RichNonZeroSuppData.h"

// namespaces
using namespace LHCb; ///< LHCb general namespace

// =================================================================================================

namespace RichNonZeroSuppDataV1 
{

  void RichNonZeroSuppData::fillRichSmartIDs( RichSmartID::Vector & ids,
                                              const LHCb::RichSmartID hpdID ) const
  {
    // header word object
    //const Header head ( header() );

    // Get HPD software ID for this Level 0 ID
    //const RichSmartID sID = richSys->richSmartID( head.l0ID() );

    // Fill with active channels
    for ( RichDAQ::ShortType iRow = 0; iRow < dataSize(); ++iRow )
    {
      for ( RichDAQ::ShortType iCol = 0; iCol < dataSize(); ++iCol )
      {
        if ( isPixelActive(iRow,iCol) )
        {
          ids.push_back( RichSmartID( hpdID.rich(),
                                      hpdID.panel(),
                                      hpdID.hpdNumInCol(),
                                      hpdID.hpdCol(),
                                      iRow, iCol ) );
        }
      }
    }

  }

  RichDAQ::Level0ID RichNonZeroSuppData::level0ID() const
  {
    return Header(header()).l0ID();
  }

  RichDAQ::ShortType RichNonZeroSuppData::hitCount() const
  {
    return Header(header()).hitCount();
  }

  void RichNonZeroSuppData::fillMsgStream( MsgStream & os ) const
  {

    os << "======================================================================================================"
       << endreq;

    // Header
    os << Header( header() ) << endreq;

    // Raw print out of data block
    os << endreq;
    dumpAllBits( os );
    os << endreq;

    os << "======================================================================================================"
       << endreq;

  }

} // end V1 namespace
