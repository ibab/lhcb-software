// Include files 

#include "Event/VeloCluster.h"
#include "Event/MCParticle.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "VeloAssociators/VeloCluster2MCParticleAsct.h"
#include "VeloCluster2MCParticleAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloCluster2MCParticleAlg
//
// 22/05/2002 : Chris Parkes
// 10/07/2003 : David Hutchcroft Modified to use VeloCluster2MCHit table
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<VeloCluster2MCParticleAlg>          s_factory ;
const        IAlgFactory& VeloCluster2MCParticleAlgFactory = s_factory ; 

VeloCluster2MCParticleAlg::VeloCluster2MCParticleAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", 
                   m_outputData = VeloCluster2MCParticleAsctLocation );
}

VeloCluster2MCParticleAlg::~VeloCluster2MCParticleAlg() {
  // destructor
}; 

StatusCode VeloCluster2MCParticleAlg::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc = toolSvc()->retrieveTool("VeloCluster2MCHitAsct", m_pV2MCHit);
  if ( sc.isFailure() || (0 == m_pV2MCHit) ) {
    log << MSG::ERROR << "Could not retrieve VeloCluster2MCHitAsct tool"
        << endmsg;
    return sc;
  }
 
  return StatusCode::SUCCESS;
};


StatusCode VeloCluster2MCParticleAlg::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "--- execute---" << endreq;

  // get VeloClusters
  SmartDataPtr<VeloClusters> clusterCont(eventSvc(),
                                         VeloClusterLocation::Default);
  if (0 == clusterCont){
    log << MSG::WARNING << "Failed to find VeloClusters" << endreq;
    return StatusCode::FAILURE;
  }

  // create an association table 
  VeloCluster2MCParticleAsct::Table* aTable = 
    new VeloCluster2MCParticleAsct::Table(); 
  // loop and link VeloClusters to MC truth
  VeloClusters::const_iterator iterClus;
  for(iterClus = clusterCont->begin(); 
      iterClus != clusterCont->end(); iterClus++){

    typedef AssociatorWeighted<VeloCluster,MCVeloHit,double> VCl2MCHit;
    typedef VCl2MCHit::DirectType Table;
    typedef Table::Range Range;
    typedef Table::iterator iterator;
    
    const Table* table = m_pV2MCHit->direct();
    if (0==table){
      log << MSG::ERROR << "Could not retrieve VeloCluster2MCHit table"
          << endmsg;
      delete aTable;
      return StatusCode::FAILURE;
    }
    
    Range range1 = table->relations(*iterClus);
    iterator relation;
    for (relation=range1.begin(); relation !=range1.end(); relation++){
      // loop over relations
      MCVeloHit * hit = relation->to ();
      MCParticle * aParticle = hit->mcParticle();
      double charge = relation->weight();
      log << MSG::DEBUG 
          << "Relation output - particle charge " 
          << charge << endreq;
      if(0==aParticle){
        log << MSG::WARNING
            << "MCHit did not have an MCParticle parent" << endreq;
      }else{
        aTable->relate(*iterClus,aParticle,charge);
      }
    }
  } // loop iterClus
  
  // register table in store
  StatusCode sc = eventSvc()->registerObject(outputData(), aTable);
  if( sc.isFailure() ) {
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << "     *** Could not register " << outputData()
        << endreq;
    delete aTable;
    return sc;
  }
 
  return StatusCode::SUCCESS;
}

StatusCode VeloCluster2MCParticleAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  if( 0 == m_pV2MCHit ) {
    toolSvc()->releaseTool( m_pV2MCHit );
    m_pV2MCHit = 0;
  }
  
  return StatusCode::SUCCESS;
}





