// $Id: RawEvent.cpp,v 1.5 2005-05-10 08:08:37 cattanem Exp $
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

  m_bufLen = rawBuffer.currentSize();
  m_buffer = rawBuffer.buffer();
  this->decode();
}

//=============================================================================
// Constructor from buffer pointer, for use online
//=============================================================================
RawEvent::RawEvent( raw_int* buffer, unsigned int length ) {

  m_buffer = buffer;
  m_bufLen = length;
  this->decode();
}

//=============================================================================
// Method to decode buffer
//=============================================================================
void RawEvent::decode() 
{
  unsigned int i=0;
  while( i < m_bufLen ){
    short bankSize = (m_buffer[i]>>16)&0xFFFF;
    // Check integrity
    if ( (m_buffer[i]&0xFFFF) != RawBuffer::MagicPattern ){
      throw GaudiException( "Bad magic number", "RawBufferException",
                            StatusCode::FAILURE );
    }
    // Determine bank type
    int bankType  = m_buffer[i+1]&0xFF;

    // Add this bank address to Event Map
    (m_eventMap[bankType]).push_back(RawBank(m_buffer+i));
    // Go to the next bank
    i+=bankSize;
  }
}

//=============================================================================
