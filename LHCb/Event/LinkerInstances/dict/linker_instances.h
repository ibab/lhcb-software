#include "Linker/LinkedTo.h"

#include "Event/MCParticle.h"
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"
#include "Event/MuonCoord.h
#include "Event/Track.h"

namespace {
  LinkedTo<MCParticle,VeloCluster> i1 (0,0,"");
  LinkedTo<MCParticle,STCluster>   i2 (0,0,"");
  LinkedTo<MCParticle,OTTime>      i3 (0,0,"");
  LinkedTo<MCParticle,MuonCoord>   i4 (0,0,"");
  LinkedTo<MCParticle,Track>       i5 (0,0,"");
}
