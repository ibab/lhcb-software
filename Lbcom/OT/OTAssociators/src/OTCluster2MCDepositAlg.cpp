// $Id: OTCluster2MCDepositAlg.cpp,v 1.6 2003-07-15 11:31:06 jvantilb Exp $

// Event
#include "Event/OTCluster.h"
#include "Event/OTDigit.h"
#include "Event/MCOTDeposit.h"

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
  declareProperty( "associatorName", m_nameAsct = "OTDigit2MCDepositAsct" );
}

OTCluster2MCDepositAlg::~OTCluster2MCDepositAlg() {
  // destructor
}

StatusCode OTCluster2MCDepositAlg::initialize()
{
  StatusCode sc = toolSvc()->retrieveTool(m_nameAsct, m_hAsct);
  if ( sc.isFailure() || 0 == m_hAsct ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << "Unable to retrieve Associator tool" << endmsg;
    return sc;
  } 

  return StatusCode::SUCCESS;
}

StatusCode OTCluster2MCDepositAlg::execute() 
{
  // get OTClusters
  SmartDataPtr<OTClusters> clusterCont(eventSvc(),OTClusterLocation::Default);
  if (0 == clusterCont){ 
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find OTClusters" << endmsg;
    return StatusCode::FAILURE;
  }
  
  // create an association table and register table in store
  OTCluster2MCDepositAsct::Table* aTable = new OTCluster2MCDepositAsct::Table();
  StatusCode sc = eventSvc()->registerObject(outputData(), aTable);
  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << "Could not register " << outputData() << endmsg;
    delete aTable;
    return StatusCode::FAILURE;
  }

  // loop and link OTClusters to MC truth
  OTClusters::const_iterator iterClus;
  for ( iterClus = clusterCont->begin(); 
        iterClus != clusterCont->end(); iterClus++){
    std::vector<MCOTDeposit*> depVector;
    associateToTruth(*iterClus, depVector);
    std::vector<MCOTDeposit*>::iterator iDep = depVector.begin();
    while (iDep != depVector.end()) {
      aTable->relate(*iterClus, *iDep);
      ++iDep;
    }
  } // loop iterClus
 
  return StatusCode::SUCCESS;
}

StatusCode OTCluster2MCDepositAlg::finalize() 
{
  // Release tool
  if( m_hAsct ) toolSvc()->releaseTool( m_hAsct );

  return StatusCode::SUCCESS;
}

StatusCode 
OTCluster2MCDepositAlg::associateToTruth(const OTCluster* aCluster,
                                         std::vector<MCOTDeposit*>& depVector)
{
  // make link to OTDigit
  const OTDigit* aDigit = aCluster->digit();
  if (0 == aDigit) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "No digit associated with cluster!" << endmsg;
    return StatusCode::FAILURE;
  }
  
  // When there are more than one tdc-times in a OTDigit
  // do something more elaborate: get the position in the vector of times.
  int timeNumber = 0;
  if ( (aDigit->tdcTimes()).size() > 1 ) {
    // count how many clusters are on the same channel before this cluster
    SmartDataPtr<OTClusters> clusterCont(eventSvc(),
                                         OTClusterLocation::Default);
    int key = aCluster->key();
    OTCluster* prevClus = clusterCont->object(--key);
    while ( prevClus != 0 && prevClus->digit() == aDigit ) {    
      ++timeNumber;
      prevClus = clusterCont->object(--key);
    }  
  }

  // Use the associator from OTDigits to MCOTDeposits
  OTDigit2MCDepositAsct::DirectType* aTable = m_hAsct->direct();
  if ( 0 == aTable ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find table" << endmsg;
    return StatusCode::FAILURE;
  }
 
  OTDigit2MCDepositAsct::MCDeposits range = aTable->relations(aDigit);
  if ( !range.empty() ) {
    OTDigit2MCDepositAsct::MCDepositsIterator iterDep = range.begin();
    for ( iterDep = range.begin(); iterDep != range.end(); ++iterDep ) {      
      MCOTDeposit* aDeposit = iterDep->to();
      int depNumber = iterDep->weight();
      if ( 0 != aDeposit && depNumber == timeNumber ) {
        depVector.push_back(aDeposit);
      }
    }
  }

  return StatusCode::SUCCESS;
}
