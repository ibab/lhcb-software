// $Id: L1Event.cpp,v 1.3 2003-11-10 15:51:46 cattanem Exp $
// Include files 
#include "GaudiKernel/Kernel.h"
#include <stdio.h>

// from Gaudi

// local
#include "Event/L1Event.h"
#include "GaudiKernel/MsgStream.h"

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
    //int sourceID =int(((buffer[i]>>7)&63));
    int bankType  =int( (buffer[i]>>13));
    // Add this bank address to Event Map
    (m_eventMap[bankType]).push_back(L1Bank(buffer+i));
    // Go to the next bank
    i+=bankSize;
  }

}

//=============================================================================
