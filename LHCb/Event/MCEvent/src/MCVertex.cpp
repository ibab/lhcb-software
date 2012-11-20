
// Event
#include "Event/MCVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCVertex
//
// 2012-11-20 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::MCVertex::fillStream(std::ostream& s) const
{
  s << "{ Position = "  << position()
    << " Time = "       << time()
    << " Type = "       << type()
    << " Mother = "     << mother()
    << " Products : #=" << products().size() << " [";
  for ( SmartRefVector<LHCb::MCParticle>::const_iterator iter = products().begin();
        iter != products().end(); ++iter )
  { s << " " << *iter; }
  s << " ] }";
  return s;
}
