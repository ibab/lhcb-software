// $Id: L1Event.cpp,v 1.1 2003-10-03 13:59:11 cattanem Exp $
// Include files 

// from Gaudi

// local
#include "Event/L1Event.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L1Event
//
// 2003-10-03 : Marco Cattaneo
//-----------------------------------------------------------------------------

//=============================================================================
// Constructor from L1Buffer
//=============================================================================
L1Event::L1Event( L1Buffer& l1Buffer ) {

  long l1Size = l1Buffer.currentSize();
  l1_int * buffer = l1Buffer.buffer();
  long i=0;
  while( i < l1Size ){
    int bankSize = int(buffer[i]&127);
    //    int source_ID =int(((buffer[i]>>7)&127));
    int class_ID  =int( (buffer[i]>>14));

    // Add this bank address to Event Map
    (m_eventMap[class_ID]).push_back(L1Bank(buffer+i));
    // Go to the next bank
    i+=bankSize;
  }

}

//=============================================================================
