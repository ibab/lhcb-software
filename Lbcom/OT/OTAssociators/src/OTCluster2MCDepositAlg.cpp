// $Id: OTCluster2MCDepositAlg.cpp,v 1.2 2002-08-07 15:55:06 jvantilb Exp $

// Event
#include "Event/OTCluster.h"
#include "Event/OTDigit.h"
#include "Event/MCOTDigit.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCTruth.h"

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "OTCluster2MCDepositAlg.h"
#include "OTAssociators/OTCluster2MCDepositAsct.h"

/** @file OTCluster2MCDepositAlg.cpp 
 *
 *  Implementation of the OTCluster2MCDepositAlg class
 *  
 *  @author J. van Tilburg
 *  @date   14/05/2002
 */

// Declaration of the Algorithm Factory
static const  AlgFactory<OTCluster2MCDepositAlg>          s_factory ;
const        IAlgFactory& OTCluster2MCDepositAlgFactory = s_factory ; 

OTCluster2MCDepositAlg::OTCluster2MCDepositAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm (name,pSvcLocator) 
{

  // constructor
  declareProperty( "OutputData", m_outputData  = OTCluster2MCDepositLocation );
}

OTCluster2MCDepositAlg::~OTCluster2MCDepositAlg() {
  // destructor
}; 

StatusCode OTCluster2MCDepositAlg::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
 
  return StatusCode::SUCCESS;
};


StatusCode OTCluster2MCDepositAlg::execute() 
{

  typedef Relation1D<OTCluster, MCOTDeposit>    Table;

  // get OTClusters
  SmartDataPtr<OTClusters> clusterCont(eventSvc(),OTClusterLocation::Default);
  if (0 == clusterCont){ 
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING << "Failed to find OTClusters" << endreq;
    return StatusCode::FAILURE;
  }
  
  // create an association table 
  Table* aTable = new Table();

  // loop and link OTClusters to MC truth
  OTClusters::const_iterator iterClus;
  for(iterClus = clusterCont->begin(); 
      iterClus != clusterCont->end(); iterClus++){
    MCOTDeposit* aHit = 0;
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

StatusCode OTCluster2MCDepositAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode OTCluster2MCDepositAlg::associateToTruth(const OTCluster* aCluster,
                                                    MCOTDeposit*& aHit) {
  // make link to truth  to MCOTDeposit
  StatusCode sc = StatusCode::SUCCESS;

  //SmartRef<OTDigit> aDigit = aCluster->digit();
  const OTDigit* aDigit = aCluster->digit();

  if (0 == aDigit) {
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING << "No digit associated with cluster!" << endmsg;
    return StatusCode::FAILURE;
  }

  // get the position in the vector of times
  int clusterTime = (int) (aCluster->signalTime());
  std::vector<int> times = aDigit->tdcTimes();
  int timeNumber = -1;
  
  // When there are more than one tdc-times in a OTDigit
  // do something more elaborate.
  if (times.size() > 1 ) {  
    SmartDataPtr<OTClusters> clusterCont(eventSvc(),
                                         OTClusterLocation::Default);
    OTClusters::const_iterator iterClus;
    // find the time correction due to time of flight subtraction
    double timeCorrection = 0.0;    
    std::vector<int>::iterator iTime = times.begin();
    for ( iTime = times.begin(); iTime != times.end(); ++iTime ) {
      timeCorrection += (double) (*iTime);
    }
    for ( iterClus = clusterCont->begin(); clusterCont->end() != iterClus;
          ++iterClus ) {
      if ( (*iterClus)->digit() == aDigit ) {
        timeCorrection -= (*iterClus)->signalTime();
      }
    }    

    timeCorrection /= (double) (times.size());

    // compare times and take the closest one
    int thisNumber = -1;
    int thisTime;
    int diffTime = 10000;    
    for ( iTime = times.begin(); iTime != times.end(); ++iTime ) {
      thisNumber++;
      thisTime = abs( (int) (timeCorrection) + clusterTime - (*iTime));
      if (thisTime < diffTime) {
        timeNumber = thisNumber;
        diffTime = thisTime;
      }
    }
  } else { // Easy: just one time, so nothing special
    timeNumber = 0;
  }

  // link digit to truth
  const MCOTDigit* mcDigit = mcTruth<MCOTDigit>(aDigit); 
  if (0 != mcDigit && timeNumber != -1) {

    // link to deposits
    SmartRefVector<MCOTDeposit> depCont = mcDigit->deposits();
    if ( 0 == depCont.size()) return StatusCode::FAILURE;
    MCOTDeposit* deposit = depCont[timeNumber];
    if ( 0 == deposit) return StatusCode::FAILURE;
    aHit = deposit;
  } else {
    return StatusCode::FAILURE;
  }

  return sc;
}








