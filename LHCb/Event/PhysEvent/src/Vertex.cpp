// $Id: Vertex.cpp,v 1.6 2004-12-10 15:16:11 cattanem Exp $
// Include files 

// STD and STL
#include <iostream> // from Gaudi

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
  , m_nDoF( vert.nDoF() )
  , m_type( vert.type() )
  , m_products( vert.products() )  
{
}

//=============================================================================
// Clone 
//=============================================================================
Vertex* Vertex::clone() const
{
//    Vertex* newVtx = new Vertex(*this);
//    // clone the end particles
//    SmartRefVector<Particle>::const_iterator ip = vert.products().begin();
//    while( ip != vert.products().end() ) {
//      m_products.push_back((*ip)->clone());
//      ip++;
//    }  
//    return newVtx;
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
    m_nDoF = orig.nDoF();
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
