// $Id: OTCluster2MCDepositAlg.cpp,v 1.5 2003-06-10 09:04:15 jvantilb Exp $

// Event
#include "Event/OTCluster.h"
#include "Event/OTDigit.h"
#include "Event/MCOTDigit.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCTruth.h"

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

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
  declareProperty( "acceptTime", m_acceptTime  = 7.8*ns );
}

OTCluster2MCDepositAlg::~OTCluster2MCDepositAlg() {
  // destructor
}

StatusCode OTCluster2MCDepositAlg::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endmsg;
 
  return StatusCode::SUCCESS;
}

StatusCode OTCluster2MCDepositAlg::execute() 
{

  typedef Relation1D<OTCluster, MCOTDeposit>    Table;

  // get OTClusters
  SmartDataPtr<OTClusters> clusterCont(eventSvc(),OTClusterLocation::Default);
  if (0 == clusterCont){ 
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find OTClusters" << endmsg;
    return StatusCode::FAILURE;
  }

  // get MCOTDeposits to be able to find other deposits killed by dead-time
  if (m_acceptTime > 0.0 ) {
    SmartDataPtr<MCOTDeposits> deposits(eventSvc(), 
                                        MCOTDepositLocation::Default);
    if (0 == deposits){ 
      MsgStream msg(msgSvc(), name());
      msg << MSG::WARNING << "Failed to find MCOTDeposits" << endmsg;
      return StatusCode::FAILURE;
    }
    m_deposits = deposits;
  }
  
  // create an association table 
  Table* aTable = new Table();

  // loop and link OTClusters to MC truth
  OTClusters::const_iterator iterClus;
  for(iterClus = clusterCont->begin(); 
      iterClus != clusterCont->end(); iterClus++){
    std::vector<MCOTDeposit*> depVector;
    associateToTruth(*iterClus, depVector);
    std::vector<MCOTDeposit*>::iterator iDep = depVector.begin();
    while (iDep != depVector.end()) {
      aTable->relate(*iterClus, *iDep);
      iDep++;
    }
  } // loop iterClus

  // register table in store
  StatusCode sc = eventSvc()->registerObject(outputData(), aTable);
  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << "     *** Could not register " << outputData()
        << endmsg;
    delete aTable;
    return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;
}

StatusCode OTCluster2MCDepositAlg::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode OTCluster2MCDepositAlg::associateToTruth(const OTCluster* aCluster,
                                        std::vector<MCOTDeposit*>& depVector) {
  // make link to truth  to MCOTDeposit
  StatusCode sc = StatusCode::SUCCESS;

  const OTDigit* aDigit = aCluster->digit();

  if (0 == aDigit) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "No digit associated with cluster!" << endmsg;
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
  MCOTDeposit* deposit = 0;
  const MCOTDigit* mcDigit = mcTruth<MCOTDigit>(aDigit);
  if (0 != mcDigit && timeNumber != -1) {

    // link to deposits
    SmartRefVector<MCOTDeposit> depCont = mcDigit->deposits();
    if ( 0 == depCont.size()) return StatusCode::FAILURE;
    deposit = depCont[timeNumber];
    if ( 0 == deposit) return StatusCode::FAILURE;
    depVector.push_back(deposit);
  } else {
    return StatusCode::FAILURE;
  }

  // find other deposits killed by dead-time, but within certain time window.
  if (m_acceptTime > 0.0 ) {
    bool keepAdding = true;
    while (keepAdding) {
      // Go to the next MCOTDeposit (they should be ordered)
      const int key = deposit->key();
      MCOTDeposit* nextDeposit = m_deposits->object(key+1);
      if (nextDeposit == 0) {
          keepAdding = false;
          continue;
      }
      // Check for same channel and within acceptTime cut
      if ( nextDeposit->channel() == deposit->channel() &&
           nextDeposit->tdcTime() < deposit->tdcTime() + m_acceptTime ) {
        depVector.push_back(nextDeposit);
        deposit = nextDeposit;
      } else {
        keepAdding = false;
      }
    }    
  }

  return sc;
}
