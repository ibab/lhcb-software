// $Id: HLTEvent.cpp,v 1.1 2003-10-03 13:59:11 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/GaudiException.h" 

// local
#include "Event/HLTEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HLTEvent
//
// 2003-10-03 : Marco Cattaneo
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor from HLTBuffer
//=============================================================================
HLTEvent::HLTEvent( HLTBuffer& hltBuffer ) {

  // Loop over HLTBuffer and get info for HLTEvent:
  long hltSize = hltBuffer.currentSize();
  hlt_int * buffer = hltBuffer.buffer();
  hlt_int magic = hltBuffer.magic();
  long i=0;
  while( i < hltSize ){
    long bankSize = buffer[i];
    // Check integrity
    if ( buffer[i+1] != magic ){
      throw GaudiException( "Bad magic number", "HLTBufferException",
                            StatusCode::FAILURE );
    }
    // Determine class_ID
    // int source_ID =(buffer[i+2]>>16);
    int class_ID  = buffer[i+2]&65535;

    // Add this bank address to Event Map
    (m_eventMap[class_ID]).push_back(HLTBank(buffer+i));
    // Go to the next bank
    i+=bankSize;
  }
}

//=============================================================================
