// $Id: OTCluster2MCHitAlg.cpp,v 1.10 2003-07-15 11:31:07 jvantilb Exp $

// Event
#include "Event/OTCluster.h"
#include "Event/OTDigit.h"
#include "Event/MCHit.h"

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
  declareProperty( "associatorName", m_nameAsct = "OTDigit2MCHitAsct" );
}

OTCluster2MCHitAlg::~OTCluster2MCHitAlg() 
{
  // destructor
}

StatusCode OTCluster2MCHitAlg::initialize() 
{
  StatusCode sc = toolSvc()->retrieveTool(m_nameAsct, m_hAsct);
  if ( sc.isFailure() || 0 == m_hAsct ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << "Unable to retrieve Associator tool" << endmsg;
    return sc;
  }
 
  return StatusCode::SUCCESS;
}


StatusCode OTCluster2MCHitAlg::execute() 
{
  // get OTClusters
  SmartDataPtr<OTClusters> clusterCont(eventSvc(),OTClusterLocation::Default);
  if (0 == clusterCont){ 
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find OTClusters" << endmsg;
    return StatusCode::FAILURE;
  }  

  // create an association table and register table in store
  OTCluster2MCHitAsct::Table* aTable = new OTCluster2MCHitAsct::Table();
  StatusCode sc = eventSvc()->registerObject(outputData(), aTable);
  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << "Could not register " << outputData() << endmsg;
    delete aTable;
    return StatusCode::FAILURE;
  }

  // loop and link OTClusters to MC truth
  OTClusters::const_iterator iterClus;
  for(iterClus = clusterCont->begin(); 
      iterClus != clusterCont->end(); ++iterClus){
    std::vector<MCHit*> hitVector;
    associateToTruth(*iterClus, hitVector);
    std::vector<MCHit*>::iterator iHit = hitVector.begin();
    while (iHit != hitVector.end()) {
      aTable->relate(*iterClus, *iHit);
      ++iHit;
    }
  } // loop iterClus
 
  return StatusCode::SUCCESS;
}

StatusCode OTCluster2MCHitAlg::finalize() 
{
  // Release tool
  if( m_hAsct ) toolSvc()->releaseTool( m_hAsct );

  return StatusCode::SUCCESS;
}


StatusCode OTCluster2MCHitAlg::associateToTruth(const OTCluster* aCluster,
                                                std::vector<MCHit*>& hitVector) 
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
  OTDigit2MCHitAsct::DirectType* aTable = m_hAsct->direct();
  if ( 0 == aTable ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find table" << endmsg;
    return StatusCode::FAILURE;
  }
 
  OTDigit2MCHitAsct::MCHits range = aTable->relations( aDigit );
  if ( !range.empty() ) {
    OTDigit2MCHitAsct::MCHitsIterator iterHit = range.begin();
    for ( iterHit = range.begin(); iterHit != range.end(); ++iterHit ) {      
      MCHit* aHit = iterHit->to();
      int hitNumber = iterHit->weight();
      if ( 0 != aHit && hitNumber == timeNumber ) {
        hitVector.push_back(aHit);
      }
    }
  }

  return StatusCode::SUCCESS;
}
