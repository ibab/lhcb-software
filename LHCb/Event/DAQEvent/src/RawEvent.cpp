// $Id: RawEvent.cpp,v 1.3 2005-03-07 13:07:40 cattanem Exp $
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
    short bankSize = (buffer[i]>>16)&0xFFFF;
    // Check integrity
    if ( (buffer[i]&0xFFFF) != magic ){
      throw GaudiException( "Bad magic number", "RawBufferException",
                            StatusCode::FAILURE );
    }
    // Determine bank type
    int bankType  = buffer[i+1]&0xFF;

    // Add this bank address to Event Map
    (m_eventMap[bankType]).push_back(RawBank(buffer+i));
    // Go to the next bank
    i+=bankSize;
  }
}

//=============================================================================
