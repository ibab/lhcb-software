// $Id: Vertex.cpp,v 1.2 2002-05-15 15:38:48 gcorti Exp $
// Include files 

// STD and STL
#include <iostream> // from Gaudi
#include "GaudiKernel/StreamBuffer.h" 

// local
#include "Event/Vertex.h"
#include "Event/Particle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCVertex
//                                 derived from class ContainedObject
//
// 14/03/2002 : Gloria Corti
//----------------------------------------------------------------------------

//=============================================================================
// Copy constructor
//=============================================================================
Vertex::Vertex(const Vertex& vert)
  : KeyedObject<int>() {
  
  m_position = vert.m_position;
  m_positionErr = vert.m_positionErr;
  m_chi2 = vert.m_chi2;
  m_type = vert.m_type;
  m_desktop = vert.m_desktop;
  // clone the end particles
  SmartRefVector<Particle>::const_iterator ip = vert.m_products.begin();
  while( ip != vert.m_products.end() ) {
    m_products.push_back((*ip)->clone());
    ip++;
  }
  
}

//=============================================================================
// Clone 
//=============================================================================
Vertex* Vertex::clone() const
{
  return new Vertex(*this);
}

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
