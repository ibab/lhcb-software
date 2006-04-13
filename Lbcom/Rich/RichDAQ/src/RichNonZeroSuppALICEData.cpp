
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppALICEData.cpp
 *
 *  Implementation file for RICH DAQ helper class : RichNonZeroSuppALICEData
 *
 *  $Id: RichNonZeroSuppALICEData.cpp,v 1.1 2006-04-13 12:37:10 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

// local
#include "RichNonZeroSuppALICEData.h"

// namespaces
using namespace LHCb; ///< LHCb general namespace

// =================================================================================================

namespace RichNonZeroSuppALICEDataV1
{

  void RichNonZeroSuppALICEData::fillRichSmartIDs( RichSmartID::Vector & ids,
                                                   const LHCb::RichSmartID hpdID ) const
  {

    // Fill with active channels
    for ( RichDAQ::ShortType iRow = 0; iRow < RichDAQ::MaxDataSizeALICE; ++iRow )
    {
      for ( RichDAQ::ShortType iCol = 0; iCol < RichDAQ::MaxDataSize; ++iCol )
      {
        if ( isPixelActive(iRow,iCol) )
        {
          ids.push_back( RichSmartID( hpdID.rich(),
                                      hpdID.panel(),
                                      hpdID.hpdNumInCol(),
                                      hpdID.hpdCol(),
                                      iRow/8,  // pixel row
                                      iCol,    // pixel col
                                      iRow%8 ) // pixel subrow
                         );
        }
      }
    }

  }

  RichDAQ::Level0ID RichNonZeroSuppALICEData::level0ID() const
  {
    return Header(header()).l0ID();
  }

  RichDAQ::ShortType RichNonZeroSuppALICEData::hitCount() const
  {
    return Header(header()).hitCount();
  }

  void RichNonZeroSuppALICEData::fillMsgStream( MsgStream & os ) const
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
