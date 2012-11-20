
// Event
#include "Event/MCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCParticle
//
// 2012-11-20 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::MCParticle::fillStream(std::ostream& s) const
{
  s << "{ Momentum = "     << momentum()
    << " ParticleID = "    << particleID()
    << " OriginVertex = "  << originVertex()
    << " EndVertices : #=" << endVertices().size() << " [";
  for ( SmartRefVector<LHCb::MCVertex>::const_iterator iter = endVertices().begin();
        iter != endVertices().end(); ++iter )
  { s << " " << *iter; }
  s << " ] }";
  return s;
}

bool LHCb::MCParticle::hasOscillated() const
{
  bool hasOsc = false;
  for ( SmartRefVector<LHCb::MCVertex>::const_iterator v = endVertices().begin();
        v != endVertices().end(); ++v )
  {
    if ( (*v)->type() == LHCb::MCVertex::OscillatedAndDecay )
    {
      hasOsc = true; break;
    }
  }
  return hasOsc;
}
