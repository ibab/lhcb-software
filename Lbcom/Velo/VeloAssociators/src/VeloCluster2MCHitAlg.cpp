// Include files 

#include "Event/VeloCluster.h"
#include "Event/MCVeloFE.h"
#include "Event/MCVeloHit.h"

#include "VeloDet/DeVelo.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "VeloCluster2MCHitAlg.h"
#include "VeloAssociators/VeloCluster2MCHitAsct.h"
#include "VeloTruthTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloCluster2MCHitAlg
//
// 21/05/2002 : Chris Parkes
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<VeloCluster2MCHitAlg>          s_factory ;
const        IAlgFactory& VeloCluster2MCHitAlgFactory = s_factory ; 

VeloCluster2MCHitAlg::VeloCluster2MCHitAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", m_outputData= VeloCluster2MCHitAsctLocation );
}

VeloCluster2MCHitAlg::~VeloCluster2MCHitAlg() {
  // destructor
}; 

StatusCode VeloCluster2MCHitAlg::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  SmartDataPtr<DeVelo> velo( detDataService(), "/dd/Structure/LHCb/Velo" );
   
  if ( 0 == velo ) {
    log << MSG::ERROR << "Unable to retrieve Velo detector element." << endreq;
    return StatusCode::FAILURE;
   }

  m_velo = velo;
 
  return StatusCode::SUCCESS;
};


StatusCode VeloCluster2MCHitAlg::execute() {
   MsgStream log(msgSvc(), name());
   log << MSG::DEBUG << "--- execute---" << endreq;
  // get VeloClusters
  SmartDataPtr<VeloClusters> clusterCont(eventSvc(),VeloClusterLocation::Default);
  if (0 == clusterCont){ 
    log << MSG::WARNING << "Failed to find VeloClusters" << endreq;
    return StatusCode::FAILURE;
  }


  // get VeloFEs
  SmartDataPtr<MCVeloFEs> feCont(eventSvc(),  MCVeloFELocation::Default);
  if (0 == feCont){ 
    log << MSG::WARNING << "Failed to find MCVeloFEs" << endreq;
    return StatusCode::FAILURE;
  } 
 
  // create an association table 
  VeloCluster2MCHitAsct::Table* aTable = new VeloCluster2MCHitAsct::Table(); 

  // loop and link VeloClusters to MC truth
  VeloClusters::const_iterator iterClus;
  for(iterClus = clusterCont->begin(); 
      iterClus != clusterCont->end(); iterClus++){
      std::map<MCVeloHit*,double> hitMap;
      StatusCode sc = VeloTruthTool::associateToTruth(*iterClus,hitMap,feCont);
    if (sc){
      std::map<MCVeloHit*,double>::const_iterator iterMap;
      for (iterMap = hitMap.begin(); iterMap !=  hitMap.end(); iterMap++){
        SmartRef<MCVeloHit> aHit = (*iterMap).first;
        double charge = (*iterMap).second;
        log << MSG::DEBUG << "VeloTruthTool output - hit "  << " charge " << charge << endreq;
        aTable->relate(*iterClus,aHit,charge);
      }
    }
    else{
      log << MSG::DEBUG << "VeloTruthTool output - no hits found, e.g. noise / spillover" << endreq;
    }
  } // loop iterClus

  // register table in store
  StatusCode sc = eventSvc()->registerObject(outputData(), aTable);
  if( sc.isFailure() ) {
    MsgStream log(msgSvc(), name());
    log << MSG::FATAL << "     *** Could not register " << outputData()
        << endreq;
    delete aTable;
    return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;
};

StatusCode VeloCluster2MCHitAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}














