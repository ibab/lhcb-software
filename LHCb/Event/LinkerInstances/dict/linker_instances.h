#include "Linker/LinkedTo.h"

#include "Event/MCParticle.h"
#include "Event/VeloCluster.h"
#include "Event/TrgTrack.h"
#include "Event/MuonCoord.h"
#include "Event/ITCluster.h"
#include "Event/OTCluster.h"
#include "Event/TrStoredTrack.h"

namespace {
  LinkedTo<MCParticle,VeloCluster> i1 (0,0,"");
  LinkedTo<MCParticle,TrgTrack>    i2 (0,0,"");
  LinkedTo<MCParticle,OTCluster> i3 (0,0,"");
  LinkedTo<MCParticle,ITCluster> i4 (0,0,"");
  LinkedTo<MCParticle,MuonCoord> i5 (0,0,"");
  LinkedTo<MCParticle,TrStoredTrack> i6 (0,0,"");
}
