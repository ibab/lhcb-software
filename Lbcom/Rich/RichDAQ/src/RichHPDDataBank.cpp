
//-----------------------------------------------------------------------------
/** @file RichHPDDataBank.cpp
 *
 *  Implementation file for RICH DAQ helper class : RichHPDDataBank
 *
 *  CVS Log :-
 *  $Id: RichHPDDataBank.cpp,v 1.4 2005-03-21 14:11:42 jonrob Exp $
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
