// $Id: Vertex.cpp,v 1.7 2006-01-17 10:13:29 pkoppenb Exp $
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
LHCb::Vertex::Vertex(const LHCb::Vertex& vert)
  : KeyedObject<int>()
  , m_position( vert.position() )
  , m_covMatrix( vert.covMatrix() )
  , m_chi2( vert.chi2() )
  , m_nDoF( vert.nDoF() )
  , m_technique ( vert.technique() )
  , m_extraInfo( vert.extraInfo() )
  , m_outgoingParticles( vert.outgoingParticles() )  
{
}

//=============================================================================
// Clone 
//=============================================================================
LHCb::Vertex* LHCb::Vertex::clone() const
{
//    Vertex* newVtx = new Vertex(*this);
//    // clone the end particles
//    SmartRefVector<Particle>::const_iterator ip = vert.outgoingParticles().begin();
//    while( ip != vert.outgoingParticles().end() ) {
//      m_outgoingParticles.push_back((*ip)->clone());
//      ip++;
//    }  
//    return newVtx;
  return new LHCb::Vertex(*this);
}

