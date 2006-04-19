
//-----------------------------------------------------------------------------
/** @file RichHPDDataBank.cpp
 *
 *  Implementation file for RICH DAQ helper class : RichHPDDataBank
 *
 *  CVS Log :-
 *  $Id: RichHPDDataBank.cpp,v 1.6 2006-04-19 17:05:04 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

// local
#include "RichHPDDataBank.h"

// namespaces
using namespace LHCb; ///< LHCb general namespace

//-----------------------------------------------------------------------------

void RichHPDDataBank::dumpAllBits( MsgStream & os ) const
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
      os << "  " << isBitOn( data()[iRow], iCol );
    }
    os << endreq;
  }
  os << "------------------------------------------------------------------------------------------------------"
     << endreq;

}

void RichHPDDataBank::fillRAWBank( RichDAQ::RAWBank & rawData ) const
{
  // fill with header word
  rawData.push_back( header() );
  // ... then data words
  for ( RichDAQ::ShortType iData = 0; iData < dataSize(); ++iData )
  {
    rawData.push_back( m_data[iData] );
  }
}
