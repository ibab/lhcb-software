// $Id: Vertex.cpp,v 1.1 2002-03-27 20:05:30 gcorti Exp $
// Include files 

// STD and STL
#include <iostream> // from Gaudi
#include "GaudiKernel/StreamBuffer.h" 

// local
#include "Event/Vertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCVertex
//                                 derived from class ContainedObject
//
// 14/03/2002 : Gloria Corti
//----------------------------------------------------------------------------

//=============================================================================
// Retrieve how the vertex was made
//=============================================================================
Vertex::VertexType Vertex::type() const 
{
  return m_type;
}

//=============================================================================
// Retrieve how the vertex was made
//=============================================================================
Vertex::VertexType Vertex::type()
{
  return m_type;
}

//=============================================================================
// Update How the vertex was made
//=============================================================================
void Vertex::setType(const Vertex::VertexType& value) 
{
  m_type = value;
}


//=============================================================================
// Serialization for reading
//=============================================================================
StreamBuffer& Vertex::serialize( StreamBuffer& s ) {

  unsigned int temp = m_type;
  KeyedObject<int>::serialize(s);
  s << m_position
    << m_positionErr
    << (float)m_chi2
    << m_nDoF
    << temp
    << m_products(this);
  return s;

};


//=============================================================================
