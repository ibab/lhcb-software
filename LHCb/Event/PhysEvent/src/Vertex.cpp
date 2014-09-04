// $Id: $

#include "Event/Vertex.h"

// Gaudi
#include "GaudiKernel/IRegistry.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Vertex
//
// 2012-04-26 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::Vertex::fillStream(std::ostream& s) const
{
  VertexBase::fillStream(s);
  s << "{ " 
    << "Technique         :	" << m_technique << std::endl
    << "OutgoingParticles : " << m_outgoingParticles;
  const std::string testLocation = 
    ( parent() && parent()->registry() ?
      parent()->registry()->identifier() : "" );
  if ( !testLocation.empty() ) { s << std::endl << "TES=" << testLocation; }
  return s << " }";
}
