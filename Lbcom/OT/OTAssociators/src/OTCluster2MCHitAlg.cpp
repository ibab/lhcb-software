// $Id: OTCluster2MCHitAlg.cpp,v 1.6 2002-09-27 09:41:05 jvantilb Exp $

// Event
#include "Event/OTCluster.h"
#include "Event/OTDigit.h"
#include "Event/MCOTDigit.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCHit.h"
#include "Event/MCTruth.h"

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "OTCluster2MCHitAlg.h"
#include "OTAssociators/OTCluster2MCHitAsct.h"

/** @file OTCluster2MCHitAlg.cpp 
 *
 *  Implementation of the OTCluster2MCHitAlg class
 *  
 *  @author J. van Tilburg
 *  @date   14/05/2002
 */

// Declaration of the Algorithm Factory
static const  AlgFactory<OTCluster2MCHitAlg>          s_factory ;
const        IAlgFactory& OTCluster2MCHitAlgFactory = s_factory ; 

OTCluster2MCHitAlg::OTCluster2MCHitAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", m_outputData  = OTCluster2MCHitLocation );
  declareProperty( "SpillOver", m_spillOver  = false );
  declareProperty( "associatorName", m_nameAsct = "OTCluster2MCDepositAsct" );
}

OTCluster2MCHitAlg::~OTCluster2MCHitAlg() 
{
  // destructor
}; 

StatusCode OTCluster2MCHitAlg::initialize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialize" << endreq;

  StatusCode sc = toolSvc()->retrieveTool(m_nameAsct, m_hAsct);
  if( sc.isFailure() || 0 == m_hAsct) {
    log << MSG::FATAL << "Unable to retrieve Associator tool" << endreq;
    return sc;
  }
 
  return StatusCode::SUCCESS;
};


StatusCode OTCluster2MCHitAlg::execute() 
{

  typedef Relation1D<OTCluster, MCHit>    Table;

  // get OTClusters
  SmartDataPtr<OTClusters> clusterCont(eventSvc(),OTClusterLocation::Default);
  if (0 == clusterCont){ 
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING << "Failed to find OTClusters" << endreq;
    return StatusCode::FAILURE;
  }
  
  // get the MCParticles for the event, in case you do not 
  // want to make links to spillover
  SmartDataPtr<MCHits> mcHits(eventSvc(), MCHitLocation::OTHits);
  if( 0 == mcHits){
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << "Could not find MCHits in " 
        << MCHitLocation::OTHits << endreq;
    return StatusCode::FAILURE;
  }
  m_mcHits = mcHits;

  // create an association table 
  Table* aTable = new Table();

  // loop and link OTClusters to MC truth
  OTClusters::const_iterator iterClus;
  for(iterClus = clusterCont->begin(); 
      iterClus != clusterCont->end(); iterClus++){
    MCHit* aHit = 0;
    associateToTruth(*iterClus,aHit);
    if (0 != aHit ) aTable->relate(*iterClus,aHit);
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

StatusCode OTCluster2MCHitAlg::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode OTCluster2MCHitAlg::associateToTruth(const OTCluster* aCluster,
                                                MCHit*& aHit) 
{
  // make link to truth  to MCHit
  // retrieve table
  OTCluster2MCDepositAsct::DirectType* aTable = m_hAsct->direct();
  if (0 == aTable){
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING << "Failed to find table" << endreq;
    return StatusCode::FAILURE;
  }
 
  MCOTDeposit* aDeposit = 0;
  OTCluster2MCDepositAsct::MCDeposits range = aTable->relations(aCluster);
  if ( !range.empty() ) {
    OTCluster2MCDepositAsct::MCDepositsIterator iterDep = range.begin();
    aDeposit = iterDep->to();
    if (0 != aDeposit) {
      aHit = aDeposit->mcHit();
      if (!m_spillOver && 0 != aHit) {
        if (m_mcHits != aHit->parent()) aHit = 0;
      }
    }
  }
  
  return StatusCode::SUCCESS;
}
