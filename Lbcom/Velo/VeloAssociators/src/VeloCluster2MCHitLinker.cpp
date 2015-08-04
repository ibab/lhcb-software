// Include files 

// event model
#include "Event/VeloCluster.h"
#include "Event/MCVeloFE.h"
#include "Event/MCHit.h"

// Linker
#include "Linker/LinkerWithKey.h"

// local
#include "VeloCluster2MCHitLinker.h"
#include "VeloTruthTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloCluster2MCHitLinker
//
// 2006-03-15 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloCluster2MCHitLinker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloCluster2MCHitLinker::VeloCluster2MCHitLinker( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_asctName ( LHCb::VeloClusterLocation::Default + "2MCHits" ),
    m_inputClusters ( LHCb::VeloClusterLocation::Default ),
    m_inputFEs ( LHCb::MCVeloFELocation::Default )
{ }
//=============================================================================
// Destructor
//=============================================================================
VeloCluster2MCHitLinker::~VeloCluster2MCHitLinker() { }
//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloCluster2MCHitLinker::execute() {

  debug() << "==> Execute" << endmsg;
  // get objects containers
  LHCb::VeloClusters* clusters;
  if(!exist<LHCb::VeloClusters>(m_inputClusters)){
    error()<< "There is no VeloClusters at: " << m_inputClusters <<endmsg;
    return ( StatusCode::FAILURE );
  }else{
    clusters=get<LHCb::VeloClusters>(m_inputClusters);
  }
  //
  LHCb::MCVeloFEs* mcFEs;
  if(!exist<LHCb::MCVeloFEs>(m_inputFEs)){
    error()<< "There is no MCVeloFEs at: " << m_inputFEs <<endmsg;
    return ( StatusCode::FAILURE );
  }else{
    mcFEs=get<LHCb::MCVeloFEs>(m_inputFEs);
  }
  //
  LinkerWithKey<LHCb::MCHit, LHCb::VeloCluster> myLink(evtSvc(), msgSvc(),
                                                       asctName());
  // link the VeloClusters to MCHits
  LHCb::VeloClusters::const_iterator cluIt;
  for(cluIt=clusters->begin(); cluIt!=clusters->end(); cluIt++){
    std::map<LHCb::MCHit*, double> relations;
    StatusCode sc=VeloTruthTool::associateToTruth((*cluIt), relations, mcFEs);    
    if(sc)
      {
        debug()<< "Afer the associator " <<endmsg;
        //
        std::map<LHCb::MCHit*, double>::iterator relIt;
        for(relIt=relations.begin(); relIt!=relations.end(); relIt++){
          SmartRef<LHCb::MCHit> aHit=(*relIt).first;
          debug()<< "Hit's mid point: " << aHit->midPoint() <<endmsg;
          // relation weight
          double weight=(*relIt).second;
          // linker table
          myLink.link((*cluIt), aHit, weight);
        }
      }else{
        debug()<< " ==> Empty linker table! " <<endmsg;
      }
  }// loop over clusters
  //
  return StatusCode::SUCCESS;
}
