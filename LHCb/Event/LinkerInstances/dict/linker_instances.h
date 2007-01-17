#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"

#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"
#include "Event/MuonCoord.h"
#include "Event/MuonDigit.h"
#include "Event/CaloDigit.h"
#include "Event/Track.h"

namespace {
  LinkedTo<LHCb::MCParticle,LHCb::VeloCluster> i1 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::STCluster>   i2 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::OTTime>      i3 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::MuonCoord>   i4 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::MuonDigit>   i7 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::CaloDigit>   i8 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::Track>       i5 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::MCHit>       i6 (0,0,"");

  LinkedFrom<LHCb::VeloCluster,LHCb::MCParticle> i11 (0,0,"");
  LinkedFrom<LHCb::STCluster,LHCb::MCParticle>   i12 (0,0,"");
  LinkedFrom<LHCb::OTTime,LHCb::MCParticle>      i13 (0,0,"");
  LinkedFrom<LHCb::MuonCoord,LHCb::MCParticle>   i14 (0,0,"");
  LinkedFrom<LHCb::MuonDigit,LHCb::MCParticle>   i17 (0,0,"");
  LinkedFrom<LHCb::CaloDigit,LHCb::MCParticle>   i18 (0,0,"");
  LinkedFrom<LHCb::Track,LHCb::MCParticle>       i15 (0,0,"");
  LinkedFrom<LHCb::MCHit,LHCb::MCParticle>       i16 (0,0,"");
}

