
/** @file RichNonZeroSuppData.cpp
 *
 *  Implementation file for RICH DAQ helper class : RichNonZeroSuppData
 *
 *  CVS Log :-
 *  $Id: RichNonZeroSuppData.cpp,v 1.1 2005-01-07 12:35:59 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */

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

  void RichNonZeroSuppData::fillRAWBank( RichDAQ::RAWBank & rawData ) const
  {

    // fill with header word
    rawData.push_back( header() );

    // ... then data words
    for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData )
    {
      rawData.push_back( m_data[iData] );
    }

  }

  void RichNonZeroSuppData::fillMsgStream( MsgStream & os ) const
  {

    os << "======================================================================================================"
       << endreq;

    // Header
    os << RichNonZeroSuppDataV0::RichNonZeroSuppData::Header( header() ) << endreq;

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
                                      sID.PDRow(),
                                      sID.PDCol(),
                                      iRow, iCol ) );
        }
      }
    }

  }

  void RichNonZeroSuppData::fillRAWBank( RichDAQ::RAWBank & rawData ) const
  {

    // fill with header word
    rawData.push_back( header() );

    // ... then data words
    for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData )
    {
      rawData.push_back( m_data[iData] );
    }

  }

  void RichNonZeroSuppData::fillMsgStream( MsgStream & os ) const
  {

    os << "======================================================================================================"
       << endreq;

    // Header
    os << RichNonZeroSuppDataV1::RichNonZeroSuppData::Header( header() ) << endreq;

    // Raw print out of data block
    os << endreq;
    dumpAllBits( os );
    os << endreq;

    os << "======================================================================================================"
       << endreq;

  }

} // end V1 namespace
