// $Id: RawEvent.cpp,v 1.2 2004-10-26 13:10:15 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/GaudiException.h" 

// local
#include "Event/RawEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawEvent
//
// 2003-10-03 : Marco Cattaneo
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor from RawBuffer
//=============================================================================
RawEvent::RawEvent( RawBuffer& rawBuffer ) {

  // Loop over RawBuffer and get info for RawEvent:
  long rawSize = rawBuffer.currentSize();
  raw_int * buffer = rawBuffer.buffer();
  raw_int magic = rawBuffer.magic();
  long i=0;
  while( i < rawSize ){
    long bankSize = buffer[i];
    // Check integrity
    if ( buffer[i+1] != magic ){
      throw GaudiException( "Bad magic number", "RawBufferException",
                            StatusCode::FAILURE );
    }
    // Determine bank type
    int bankType  = buffer[i+2]&0xFF;

    // Add this bank address to Event Map
    (m_eventMap[bankType]).push_back(RawBank(buffer+i));
    // Go to the next bank
    i+=bankSize;
  }
}

//=============================================================================
