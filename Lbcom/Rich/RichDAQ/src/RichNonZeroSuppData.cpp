
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppData.cpp
 *
 *  Implementation file for RICH DAQ helper class : RichNonZeroSuppData
 *
 *  $Id: RichNonZeroSuppData.cpp,v 1.3 2005-02-09 09:16:26 cattanem Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

// local
#include "RichNonZeroSuppData.h"

namespace RichNonZeroSuppDataV0 {

  void RichNonZeroSuppData::fillRichSmartIDs( RichSmartID::Collection & ids,
                                              const IRichHPDIDTool * /* hpdTool */ ) const
  {
    // header word object
    const Header head ( header() );

    // Fill with active channels
    for ( RichDAQ::ShortType iRow = 0; iRow < dataSize(); ++iRow ) {
      for ( RichDAQ::ShortType iCol = 0; iCol < dataSize(); ++iCol ) {
        if ( isPixelActive(iRow,iCol) ) {
          ids.push_back( RichSmartID( head.hpdID().rich(),
                                      head.hpdID().panel(),
                                      head.hpdID().pdRow(),
                                      head.hpdID().pdCol(),
                                      iRow, iCol ) );
        }
      }
    }

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

} // end V0 namespace

// =================================================================================================

namespace RichNonZeroSuppDataV1 {

  void RichNonZeroSuppData::fillRichSmartIDs( RichSmartID::Collection & ids,
                                              const IRichHPDIDTool * hpdTool ) const
  {
    // header word object
    const Header head ( header() );

    // Get HPD software ID for this HPD
    const RichSmartID sID = hpdTool->richSmartID( head.hpdID() );

    // Fill with active channels
    for ( RichDAQ::ShortType iRow = 0; iRow < dataSize(); ++iRow ) {
      for ( RichDAQ::ShortType iCol = 0; iCol < dataSize(); ++iCol ) {
        if ( isPixelActive(iRow,iCol) ) {
          ids.push_back( RichSmartID( sID.rich(),
                                      sID.panel(),
                                      sID.pdRow(),
                                      sID.pdCol(),
                                      iRow, iCol ) );
        }
      }
    }

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
