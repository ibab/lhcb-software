
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppData.cpp
 *
 *  Implementation file for RICH DAQ helper class : RichNonZeroSuppData
 *
 *  $Id: RichNonZeroSuppData.cpp,v 1.10 2006-04-19 17:05:04 jonrob Exp $
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

    // Fill with active channels
    for ( RichDAQ::ShortType iRow = 0; iRow < RichDAQ::MaxDataSize; ++iRow )
    {
      for ( RichDAQ::ShortType iCol = 0; iCol < RichDAQ::MaxDataSize; ++iCol )
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

// =================================================================================================

namespace RichNonZeroSuppDataV2
{

  void RichNonZeroSuppData::fillRichSmartIDs( RichSmartID::Vector & ids,
                                              const LHCb::RichSmartID hpdID ) const
  {

    // Fill with active channels
    for ( RichDAQ::ShortType iRow = 0; iRow < RichDAQ::MaxDataSize; ++iRow )
    {
      for ( RichDAQ::ShortType iCol = 0; iCol < RichDAQ::MaxDataSize; ++iCol )
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

  void RichNonZeroSuppData::dumpAllBits( MsgStream & os ) const
  {

    os << "------------------------------------------------------------------------------------------------------"
       << endreq;

    // Bit numbers
    os << " c   |";
    for ( int iCol = 31; iCol >= 0; --iCol )
    {
      os << format("%3i",iCol);
    }
    os << endreq
       << "------------------------------------------------------------------------------------------------------"
       << endreq;

    // Header
    os << " h   |";
    for ( int iCol = 31; iCol >= 0; --iCol )
    {
      os << "  " << isBitOn( header(), iCol );
    }
    os << endreq
       << "------------------------------------------------------------------------------------------------------"
       << endreq;

    // Data bits
    for ( RichDAQ::ShortType iRow = 0; iRow < dataSize(); ++iRow )
    {
      os << format( " r%2i |", iRow );
      for ( int iCol = 31; iCol >= 0; --iCol )
      {
        os << "  " << isPixelActive( iRow, iCol );
      }
      os << endreq;
    }
    os << "------------------------------------------------------------------------------------------------------"
       << endreq;

  }

} // end V2 namespace
