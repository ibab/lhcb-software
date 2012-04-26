// $Id: $

#include "Event/Vertex.h"

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
    << "OutgoingParticles : " << m_outgoingParticles
    << " }";
  return s;
}
