// $Id: RawBuffer.cpp,v 1.2 2004-01-12 09:14:41 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/StreamBuffer.h" 

// local
#include "Event/RawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawBuffer
//
// 2003-11-10 : Marco Cattaneo
//-----------------------------------------------------------------------------


//=============================================================================
// Serialisation for writing
//=============================================================================
StreamBuffer& RawBuffer::serialize( StreamBuffer& s ) const {
  s << m_initialSize
    << m_allocatedSize
    << m_currentSize;
  
  long i = 0;
  while( i < this->currentSize() ) {
    s << m_buffer[i];
    i++;
  }
  s  << m_magic;
  return s;
};

//=============================================================================
// Serialization for reading
//=============================================================================
StreamBuffer& RawBuffer::serialize( StreamBuffer& s ) {
  s >> m_initialSize
    >> m_allocatedSize
    >> m_currentSize;

  if( 0 != m_buffer ) delete[] m_buffer;
  m_buffer = new raw_int[m_allocatedSize];
  long i = 0;
  while( i < this->currentSize() ) {
#ifdef WIN32 // VERY temporary hack for Windows!
	int hack;
	s >> hack;
	m_buffer[i] = hack;
#else
    s >> m_buffer[i];
#endif
    i++;
  }

#ifdef WIN32 // VERY temporary hack for Windows!
  int hack;
  s >> hack;
  m_magic = hack;
#else
  s >> m_magic;
#endif
  return s;
};

//=============================================================================
// Serialization for printing
//=============================================================================
std::ostream& RawBuffer::fillStream(std::ostream& s) const
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
  s << "   magic:\t" << m_magic << " } ";
  return s;
}
