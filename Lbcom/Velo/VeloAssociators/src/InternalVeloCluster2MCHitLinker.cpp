// Include files 

// Event model
#include "VeloEvent/InternalVeloCluster.h"
#include "Event/MCVeloFE.h"
#include "Event/MCHit.h"

// Linker
#include "Linker/LinkerWithKey.h"

// local
#include "InternalVeloCluster2MCHitLinker.h"
#include "VeloTruthTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloCluster2MCHitLinker
//
// 2006-02-07 : Tomasz Szumlak
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( InternalVeloCluster2MCHitLinker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
InternalVeloCluster2MCHitLinker::InternalVeloCluster2MCHitLinker(
 const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_asctName ( LHCb::InternalVeloClusterLocation::Default + "2MCHits" ),
    m_inputClusters ( LHCb::InternalVeloClusterLocation::Default ),
    m_inputFEs ( LHCb::MCVeloFELocation::Default )
{ }
//=============================================================================
// Destructor
//=============================================================================
InternalVeloCluster2MCHitLinker::~InternalVeloCluster2MCHitLinker() {}
//=============================================================================
// Main execution
//=============================================================================
StatusCode InternalVeloCluster2MCHitLinker::execute() {
  
  bool isDebug = msgLevel(MSG::DEBUG);
  //
  if(isDebug) debug() << "==> Execute" << endmsg;
  //
  LHCb::InternalVeloClusters* clusters=
        get<LHCb::InternalVeloClusters>(m_inputClusters);
  LHCb::MCVeloFEs* mcFEs=get<LHCb::MCVeloFEs>(m_inputFEs);
  //
  LinkerWithKey<LHCb::MCHit,LHCb::InternalVeloCluster> myLink(
                                evtSvc(), msgSvc(), asctName());  
  //
  if(isDebug) debug()<< " ==> relations table has been created " <<endmsg;
  // link the InternalVeloClusters to MCHits
  LHCb::InternalVeloClusters::const_iterator cluIt;
  for(cluIt=clusters->begin(); cluIt!=clusters->end(); cluIt++){
    std::map<LHCb::MCHit*,double> relations;
    StatusCode sc=VeloTruthTool::associateToTruth((*cluIt), relations, mcFEs);
    if(sc){
      //
      if(isDebug) debug()<< " ==> after the associator" <<endmsg;
      std::map<LHCb::MCHit*,double>::const_iterator relIt;
      for (relIt=relations.begin(); relIt!=relations.end(); relIt++){
        SmartRef<LHCb::MCHit> hit=(*relIt).first;
        // create relation weight
        double weight=(*relIt).second;
        // create the table
        myLink.link((*cluIt), hit, weight);
      }
    }else{
      if(isDebug) debug()<< " ==> Empty table - no associations made! " <<endmsg;
    }
  } // loop clusters
  //
  return (StatusCode::SUCCESS);
}
//=============================================================================
