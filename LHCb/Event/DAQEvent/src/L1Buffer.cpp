// $Id: L1Buffer.cpp,v 1.3 2003-11-24 16:06:06 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/StreamBuffer.h" 

// local
#include "Event/L1Buffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L1Buffer
//
// 2003-11-10 : Marco Cattaneo
//-----------------------------------------------------------------------------


//=============================================================================
// Serialisation for writing
//=============================================================================
StreamBuffer& L1Buffer::serialize( StreamBuffer& s ) const {
  s << m_initialSize
    << m_allocatedSize
    << m_currentSize;
  
  long i = 0;
  while( i < this->currentSize() ) {
    s << m_buffer[i];
    i++;
  }
  return s;
};

//=============================================================================
// Serialization for reading
//=============================================================================
StreamBuffer& L1Buffer::serialize( StreamBuffer& s ) {
  s >> m_initialSize
    >> m_allocatedSize
    >> m_currentSize;

  if( 0 != m_buffer ) delete[] m_buffer;
  m_buffer = new l1_int[m_currentSize];
  long i = 0;
  while( i < this->currentSize() ) {
#ifdef WIN32 // VERY temporary hack for Windows!
	short hack;
	s >> hack;
	m_buffer[i] = hack;
#else
    s >> m_buffer[i];
#endif
    i++;
  }
  return s;
};

//=============================================================================
// Serialization for printing
//=============================================================================
std::ostream& L1Buffer::fillStream(std::ostream& s) const
{
  s << "{ "
    << " initialSize:\t" << m_initialSize << std::endl
    << "   allocatedSize:\t" << m_allocatedSize << std::endl
    << "   currentSize:\t" << m_currentSize << std::endl
    << "   buffer:" << std::endl;
  
  long i = 0;
  while( i < this->currentSize() ) {
    long j = 0;
    while( j < 8 ) {
      s << "\t" << m_buffer[i];
      j++;
      i++;
      if( i == this->currentSize() ) break;
    }
    s << std::endl;
  }
  return s;
}
