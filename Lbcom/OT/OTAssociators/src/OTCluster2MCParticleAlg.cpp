// $Id: OTCluster2MCParticleAlg.cpp,v 1.9 2003-07-15 11:31:07 jvantilb Exp $

// Event
#include "Event/OTCluster.h"
#include "Event/OTDigit.h"
#include "Event/MCParticle.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "OTCluster2MCParticleAlg.h"
#include "OTAssociators/OTCluster2MCParticleAsct.h"

/** @file OTCluster2MCParticleAlg.cpp 
 *
 *  Implementation of the OTCluster2MCParticleAlg class
 *  
 *  @author J. van Tilburg
 *  @date   23/05/2002
 */

// Declaration of the Algorithm Factory
static const  AlgFactory<OTCluster2MCParticleAlg>          s_factory ;
const        IAlgFactory& OTCluster2MCParticleAlgFactory = s_factory ; 

OTCluster2MCParticleAlg::OTCluster2MCParticleAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", m_outputData  = OTCluster2MCParticleLocation );
  declareProperty( "associatorName", m_nameAsct = "OTDigit2MCParticleAsct" );
}

OTCluster2MCParticleAlg::~OTCluster2MCParticleAlg() {
  // destructor
}

StatusCode OTCluster2MCParticleAlg::initialize() 
{
  StatusCode sc = toolSvc()->retrieveTool(m_nameAsct, m_hAsct);
  if ( sc.isFailure() || 0 == m_hAsct) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << "Unable to retrieve Associator tool" << endmsg;
    return sc;
  }
 
  return StatusCode::SUCCESS;
}


StatusCode OTCluster2MCParticleAlg::execute()
{
  // get OTClusters
  SmartDataPtr<OTClusters> clusterCont(eventSvc(),OTClusterLocation::Default);
  if (0 == clusterCont){ 
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find OTClusters" << endmsg;
    return StatusCode::FAILURE;
  }

  // create an association table and register table in store
  OTCluster2MCParticleAsct::Table* aTable = 
    new OTCluster2MCParticleAsct::Table();
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
        iterClus != clusterCont->end(); ++iterClus){
    std::vector<MCParticle*> partVector;
    associateToTruth(*iterClus, partVector);
    std::vector<MCParticle*>::iterator iPart = partVector.begin();
    while (iPart != partVector.end()) {
      aTable->relate(*iterClus, *iPart);
      ++iPart;
    }
  } // loop iterClus

  return StatusCode::SUCCESS;
}

StatusCode OTCluster2MCParticleAlg::finalize()
{
  // Release tool
  if( m_hAsct ) toolSvc()->releaseTool( m_hAsct );

  return StatusCode::SUCCESS;
}

StatusCode
OTCluster2MCParticleAlg::associateToTruth(const OTCluster* aCluster,
                                          std::vector<MCParticle*>& partVector)
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

  // Use the associator from OTDigits to MCParticles
  OTDigit2MCParticleAsct::DirectType* aTable = m_hAsct->direct();
  if ( 0 == aTable ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find table" << endmsg;
    return StatusCode::FAILURE;
  }
 
  OTDigit2MCParticleAsct::MCParticles range = aTable->relations(aDigit);
  if ( !range.empty() ) {
    OTDigit2MCParticleAsct::MCParticlesIterator iterPart = range.begin();
    for ( iterPart = range.begin(); iterPart != range.end(); ++iterPart ) {
      MCParticle* aParticle = iterPart->to();
      int depNumber = iterPart->weight();
      if ( 0 != aParticle && depNumber == timeNumber ) {
        partVector.push_back(aParticle);
      }
    }
  }

  return StatusCode::SUCCESS;
}

