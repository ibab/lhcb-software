// $Id: HltEvent.cpp,v 1.1 2003-11-03 09:45:57 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/GaudiException.h" 

// local
#include "Event/HltEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltEvent
//
// 2003-10-03 : Marco Cattaneo
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor from HltBuffer
//=============================================================================
HltEvent::HltEvent( HltBuffer& hltBuffer ) {

  // Loop over HltBuffer and get info for HltEvent:
  long hltSize = hltBuffer.currentSize();
  hlt_int * buffer = hltBuffer.buffer();
  hlt_int magic = hltBuffer.magic();
  long i=0;
  while( i < hltSize ){
    long bankSize = buffer[i];
    // Check integrity
    if ( buffer[i+1] != magic ){
      throw GaudiException( "Bad magic number", "HltBufferException",
                            StatusCode::FAILURE );
    }
    // Determine bank type
    // int sourceID =(buffer[i+2]>>16);
    int bankType  = buffer[i+2]&65535;

    // Add this bank address to Event Map
    (m_eventMap[bankType]).push_back(HltBank(buffer+i));
    // Go to the next bank
    i+=bankSize;
  }
}

//=============================================================================
