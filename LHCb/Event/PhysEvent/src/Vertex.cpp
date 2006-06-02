// $Id: Vertex.cpp,v 1.8 2006-06-02 16:38:38 jpalac Exp $
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
  : 
  KeyedObject<int>(),
  m_position( vert.m_position ),
  m_covMatrix( vert.m_covMatrix ),
  m_chi2( vert.m_chi2 ),
  m_nDoF( vert.m_nDoF ),
  m_technique ( vert.m_technique ),
  m_extraInfo( vert.m_extraInfo ),
  m_outgoingParticles( vert.m_outgoingParticles )  
{
}
//=============================================================================
// Assignment operator
//=============================================================================
LHCb::Vertex& LHCb::Vertex::operator=(const LHCb::Vertex& rhs) {

  // protect against self assignement
  if( this != &rhs ) {
    m_position          = rhs.m_position;
    m_covMatrix         = rhs.m_covMatrix;
    m_chi2              = rhs.m_chi2;
    m_nDoF              = rhs.m_nDoF;
    m_technique         = rhs.m_technique;
    m_extraInfo         = rhs.m_extraInfo;
    m_outgoingParticles = rhs.m_outgoingParticles;
    
  }
  return *this;
}

//=============================================================================
