// Include files 

#include "Event/VeloCluster.h"
#include "Event/MCVeloFE.h"
#include "Event/MCParticle.h"

#include "VeloDet/DeVelo.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "VeloCluster2MCParticleAlg.h"
#include "VeloTruthTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloCluster2MCParticleAlg
//
// 22/05/2002 : Chris Parkes
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<VeloCluster2MCParticleAlg>          s_factory ;
const        IAlgFactory& VeloCluster2MCParticleAlgFactory = s_factory ; 

VeloCluster2MCParticleAlg::VeloCluster2MCParticleAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm (name,pSvcLocator) 
{

  // constructer
  declareProperty( "OutputData", m_outputData  = "MC/VeloClusters2MCParticles" );
}

VeloCluster2MCParticleAlg::~VeloCluster2MCParticleAlg() {
  // destructer
}; 

StatusCode VeloCluster2MCParticleAlg::initialize() {

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


StatusCode VeloCluster2MCParticleAlg::execute() {
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
  VeloCluster2MCParticleAsct::Table* aTable = new VeloCluster2MCParticleAsct::Table(); 

  // loop and link VeloClusters to MC truth
  VeloClusters::const_iterator iterClus;
  for(iterClus = clusterCont->begin(); 
      iterClus != clusterCont->end(); iterClus++){
    double purity = 0;
    MCParticle* aParticle = 0;
    VeloTruthTool::associateToTruth(*iterClus,aParticle,purity,feCont);
    log << MSG::DEBUG << "VeloTruthTool output - particle " << aParticle << " purity " << purity << endreq;
    aTable->relate(*iterClus,aParticle,purity);
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

StatusCode VeloCluster2MCParticleAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}














