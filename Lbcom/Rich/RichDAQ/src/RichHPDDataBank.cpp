
//-----------------------------------------------------------------------------
/** @file RichHPDDataBank.cpp
 *
 *  Implementation file for RICH DAQ helper class : RichHPDDataBank
 *
 *  CVS Log :-
 *  $Id: RichHPDDataBank.cpp,v 1.2 2005-01-13 13:09:34 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2005/01/07 12:35:59  jonrob
 *  Complete rewrite
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

// local
#include "RichHPDDataBank.h"

//-----------------------------------------------------------------------------


void RichHPDDataBank::dumpAllBits( MsgStream & os ) const
{
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
    os << "  " << static_cast<bool>( header()&(1<<iCol) );
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
      os << "  " << static_cast<bool>( data()[iRow]&(1<<iCol) );
    }
    os << endreq;
  }

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
