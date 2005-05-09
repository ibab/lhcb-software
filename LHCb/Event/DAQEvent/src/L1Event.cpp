// $Id: L1Event.cpp,v 1.5 2005-05-09 11:50:45 cattanem Exp $
// Include files 

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
    int bankType = int( (buffer[i]>>13) &7 );
    // Add this bank address to Event Map
    (m_eventMap[bankType]).push_back(L1Bank(buffer+i));
    // Go to the next bank
    i+=bankSize;
  }

}

//=============================================================================
// Constructor from buffer pointer, for use online
//=============================================================================
L1Event::L1Event( l1_int* buffer, unsigned int length ) {

  unsigned int i=0;
  while( i < length ){
    int bankSize = int(buffer[i]&127);
    int bankType = int( (buffer[i]>>13) &7 );
    // Add this bank address to Event Map
    (m_eventMap[bankType]).push_back(L1Bank(buffer+i));
    // Go to the next bank
    i+=bankSize;
  }

}

//=============================================================================
