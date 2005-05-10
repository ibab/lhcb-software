// $Id: L1Event.cpp,v 1.6 2005-05-10 08:08:37 cattanem Exp $
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

  m_bufLen = l1Buffer.currentSize();
  m_buffer = l1Buffer.buffer();
  this->decode();
}

//=============================================================================
// Constructor from buffer pointer, for use online
//=============================================================================
L1Event::L1Event( l1_int* buffer, unsigned int length ) {

  m_bufLen = length;
  m_buffer = buffer;
  this->decode();
}

//=============================================================================
// Method to decode buffer
//=============================================================================
void L1Event::decode() 
{
  unsigned int i=0;
  while( i < m_bufLen ){
    int bankSize = int(m_buffer[i]&127);
    int bankType = int( (m_buffer[i]>>13) &7 );
    // Add this bank address to Event Map
    (m_eventMap[bankType]).push_back(L1Bank(m_buffer+i));
    // Go to the next bank
    i+=bankSize;
  }

}

//=============================================================================
