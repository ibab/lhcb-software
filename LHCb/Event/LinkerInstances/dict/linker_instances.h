#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"

#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/VeloCluster.h"
#include "Event/VLCluster.h"
#include "Event/VPCluster.h"
#include "Event/FTCluster.h"
#include "Event/STCluster.h"
#include "Kernel/OTChannelID.h"
#include "Event/MuonCoord.h"
#include "Event/MuonDigit.h"
#include "Event/CaloDigit.h"
#include "Event/Track.h"
#include "Event/Particle.h"
namespace {
  LinkedTo<LHCb::MCParticle,LHCb::VeloCluster> i1 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::VLCluster> i21 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::VPCluster> i31 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::FTCluster>   i41 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::STCluster>   i2 (0,0,"");
  LinkedTo<LHCb::MCParticle,ContainedObject>      i3 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::MuonCoord>   i4 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::MuonDigit>   i7 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::CaloDigit>   i8 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::Track>       i5 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::MCHit>       i6 (0,0,"");
  LinkedTo<LHCb::MCParticle,LHCb::Particle>    i10 (0,0,"");
  LinkedTo<LHCb::MCParticle>                   i9 (0,0,"");

  LinkedFrom<LHCb::VeloCluster,LHCb::MCParticle> i101 (0,0,"");
  LinkedFrom<LHCb::VLCluster,LHCb::MCParticle> i201 (0,0,"");
  LinkedFrom<LHCb::VPCluster,LHCb::MCParticle> i301 (0,0,"");
  LinkedFrom<LHCb::FTCluster,LHCb::MCParticle> i401 (0,0,"");
  LinkedFrom<LHCb::STCluster,LHCb::MCParticle>   i102 (0,0,"");
  LinkedFrom<ContainedObject,LHCb::MCParticle>      i103 (0,0,"");
  LinkedFrom<LHCb::MuonCoord,LHCb::MCParticle>   i104 (0,0,"");
  LinkedFrom<LHCb::MuonDigit,LHCb::MCParticle>   i107 (0,0,"");
  LinkedFrom<LHCb::CaloDigit,LHCb::MCParticle>   i108 (0,0,"");
  LinkedFrom<LHCb::Track,LHCb::MCParticle>       i105 (0,0,"");
  LinkedFrom<LHCb::MCHit,LHCb::MCParticle>       i106 (0,0,"");
  LinkedFrom<LHCb::Particle,LHCb::MCParticle>    i110 (0,0,"");
  LinkedFrom<LHCb::MCParticle>                   i109 (0,0,"");
}

