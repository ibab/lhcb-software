// $Id: VeloCluster2MCHitLinker.cpp,v 1.1 2005-03-22 07:05:30 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// Event model
#include "Event/VeloCluster.h"
#include "Event/MCVeloFE.h"
#include "Event/MCVeloHit.h"

// Linker
#include "Linker/LinkerWithKey.h"

// local
#include "VeloCluster2MCHitLinker.h"
#include "VeloTruthTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloCluster2MCHitLinker
//
// 2005-03-21 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<VeloCluster2MCHitLinker>          s_factory ;
const        IAlgFactory& VeloCluster2MCHitLinkerFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloCluster2MCHitLinker::VeloCluster2MCHitLinker( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
VeloCluster2MCHitLinker::~VeloCluster2MCHitLinker() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloCluster2MCHitLinker::execute() {

  debug() << "==> Execute" << endmsg;

  VeloClusters* clusters = get<VeloClusters>( VeloClusterLocation::Default );
  MCVeloFEs*    mcFEs    = get<MCVeloFEs>( MCVeloFELocation::Default );

  LinkerWithKey<MCVeloHit,VeloCluster> myLink( evtSvc(), msgSvc(), "VeloClusters2MCHits" );
  
  // loop and link VeloClusters to MC truth
  VeloClusters::const_iterator iterClus;
  for(iterClus = clusters->begin(); iterClus != clusters->end(); iterClus++){
    std::map<MCVeloHit*,double> hitMap;
    StatusCode sc = VeloTruthTool::associateToTruth(*iterClus,hitMap,mcFEs);
    if (sc){
      std::map<MCVeloHit*,double>::const_iterator iterMap;
      for (iterMap = hitMap.begin(); iterMap !=  hitMap.end(); iterMap++){
        SmartRef<MCVeloHit> aHit = (*iterMap).first;
        double charge = (*iterMap).second;
        debug() << "VeloTruthTool output - hit " << " charge " << charge << endmsg;
        myLink.link( *iterClus, aHit, charge );
      }
    }
    else{
      debug() << "VeloTruthTool output - no hits found, e.g. noise / spillover" << endmsg;
    }
  } // loop clusters

  return StatusCode::SUCCESS;
};

//=============================================================================
