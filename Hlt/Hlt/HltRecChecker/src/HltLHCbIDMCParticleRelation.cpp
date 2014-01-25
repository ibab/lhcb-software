// $Id: HltLHCbIDMCParticleRelation.cpp,v 1.5 2009-10-10 16:01:58 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// local
#include "ERelations.h"
#include "HltLHCbIDMCParticleRelation.h"
#include "Linker/LinkedFrom.h"

using namespace LHCb;


DECLARE_ALGORITHM_FACTORY( HltLHCbIDMCParticleRelation )

HltLHCbIDMCParticleRelation::HltLHCbIDMCParticleRelation(const std::string& name,ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  
  declareProperty("RelationLocation",
                  m_relationLocation = "Hlt/Link/MCParticleLHCbID");
}

HltLHCbIDMCParticleRelation::~HltLHCbIDMCParticleRelation() {}; 


StatusCode HltLHCbIDMCParticleRelation::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

StatusCode HltLHCbIDMCParticleRelation::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;  
  
  Hlt::IntRelation* relation = new Hlt::IntRelation();
  put(relation,m_relationLocation);
  
  VeloClusters* velos = get<VeloClusters>(VeloClusterLocation::Default);
  LinkedTo<LHCb::MCParticle,LHCb::VeloCluster> 
    veloLink( evtSvc(), msgSvc(), LHCb::VeloClusterLocation::Default );
  for (VeloClusters::iterator it = velos->begin(); it != velos->end(); ++it) {
    MCParticle* part = veloLink.first( *it );
    if (part != NULL) {
      LHCbID id((*it)->channelID());
      relation->relate(part->key(),id.lhcbID());
    }
  }

  STClusters* sts = get<STClusters>(STClusterLocation::ITClusters);
  LinkedTo<LHCb::MCParticle,LHCb::STCluster> 
    stLink( evtSvc(), msgSvc(), LHCb::STClusterLocation::ITClusters );
  for (STClusters::iterator it = sts->begin(); it != sts->end(); ++it) {
    MCParticle* part = stLink.first( *it );
    if (part != NULL) {
      LHCbID id((*it)->channelID());
      relation->relate(part->key(),id.lhcbID());
    }
  }

  OTTimes* ots = get<OTTimes>(OTTimeLocation::Default);
  LinkedTo<LHCb::MCParticle,LHCb::OTTime> 
    otLink( evtSvc(), msgSvc(), LHCb::OTTimeLocation::Default );
  for (OTTimes::iterator it = ots->begin(); it != ots->end(); ++it) {
    MCParticle* part = otLink.first( *it );
    if (part != NULL) {
      LHCbID id((*it)->channel());
      relation->relate(part->key(),id.lhcbID());
    }
  }

  // const std::vector<int>& keys = relation->keys();
  // debug() << " Keys " << keys.size() << " [1] " << keys[1] << endmsg;
  
  // relation->info();

  // LinkedFrom<DataObject,MCParticle,LHCbID> 
//     linkIDs(evtSvc(),msgSvc(),"Pat/LHCbID");
  
  return StatusCode::SUCCESS;
};

StatusCode HltLHCbIDMCParticleRelation::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
