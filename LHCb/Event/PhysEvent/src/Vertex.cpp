// $Id: Vertex.cpp,v 1.4 2002-08-16 07:37:37 graven Exp $
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
  : KeyedObject<int>()
  , m_position( vert.position() )
  , m_positionErr( vert.positionErr() )
  , m_chi2( vert.chi2() )
  , m_type( vert.type() )

{
  // clone the end particles
  SmartRefVector<Particle>::const_iterator ip = vert.products().begin();
  while( ip != vert.products().end() ) {
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
// Assignment operator
//=============================================================================
Vertex& Vertex::operator=(const Vertex& orig) {

  // protect against self assignement
  if( this != &orig ) {
    m_position = orig.position();
    m_positionErr = orig.positionErr();
    m_chi2 = orig.chi2();
    m_type = orig.type();
    m_desktop = 0;
    // Keep the same reference to particle products
    m_products = orig.products();
  }
  return *this;
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

  float l_chi2;
  unsigned l_type;
  KeyedObject<int>::serialize(s);
  s >> m_position
    >> m_positionErr
    >> l_chi2
    >> m_nDoF
    >> l_type
    >> m_desktop
    >> m_products(this);
  m_chi2 = l_chi2;
  m_type = VertexType(l_type);
  return s;

};


//=============================================================================
