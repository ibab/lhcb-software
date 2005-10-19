
//Include files
#include "Relations/RelationsDict.h"
using namespace seal::reflect;

#include "Event/ITDigit.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/ITCluster.h"
#include "Event/OTTime.h"
#include "Event/MuonCoord.h"
#include "Event/TrStoredTrack.h"
#include "Event/L0CaloCandidate.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/VeloCluster.h"
#include "Event/MCVeloHit.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include "Event/FlavourTag.h"
#include "Event/Vertex.h"


namespace
{
  struct _InitDict
  {
    _InitDict()
    {
      GaudiDict::PairDict<int,int>();
      GaudiDict::VectorDict< std::pair<int,int> >();
      GaudiDict::Relation1DDict<ITDigit, MCParticle>();
      GaudiDict::RelationWeighted1DDict<ITDigit, MCHit, double>();
      GaudiDict::RelationWeighted1DDict<ITCluster, MCParticle, double>();
      GaudiDict::RelationWeighted1DDict<ITCluster, MCHit, double>();
      GaudiDict::Relation1DDict<OTTime, MCHit>();
      GaudiDict::Relation1DDict<MuonCoord, MCParticle>();
      GaudiDict::Relation1DDict<TrStoredTrack, float>();
      GaudiDict::RelationWeighted1DDict<TrStoredTrack, MCParticle, double>();
      GaudiDict::RelationWeighted1DDict<L0CaloCandidate, MCParticle, double>();
      GaudiDict::RelationWeighted1DDict<CaloDigit, MCParticle, float>();
      GaudiDict::RelationWeighted1DDict<CaloCluster, MCParticle, float>();
      GaudiDict::RelationWeighted2DDict<CaloCluster, TrStoredTrack, float>();
      GaudiDict::RelationWeighted2DDict<CaloHypo, TrStoredTrack, float>();
      GaudiDict::RelationWeighted2DDict<VeloCluster, MCParticle, double>();
      GaudiDict::RelationWeighted2DDict<VeloCluster, MCVeloHit, double>();
      GaudiDict::RelationWeighted1DDict<ProtoParticle, MCParticle, double>();
      GaudiDict::RelationWeighted1DDict<Particle, MCParticle, double>();
      GaudiDict::Relation1DDict<Particle, MCParticle>();
      GaudiDict::Relation1DDict<Particle, FlavourTag>();
      GaudiDict::Relation2DDict<Particle, FlavourTag>();
      GaudiDict::RelationWeighted1DDict<Particle, Vertex, float>();
      GaudiDict::RelationWeighted2DDict<Particle, Vertex, float>();
    }
  };

  static _InitDict __init;
}

void* __init_InitDict_EventAssocAssoc()
{
  return &__init;
}
