// $Id: OTTime2MCParticleAlg.cpp,v 1.1 2004-09-03 12:08:10 jnardull Exp $

// local
#include "OTTime2MCParticleAlg.h"
#include "OTAssociators/OTTime2MCParticleAsct.h"

/** @file OTTime2MCParticleAlg.cpp 
 *
 *  Implementation of the OTTime2MCParticleAlg class
 *  
 *  @author J. van Tilburg and Jacopo Nardulli
 *  @date   15/06/2004
 */

// Declaration of the Algorithm Factory
static const  AlgFactory<OTTime2MCParticleAlg>          s_factory ;
const        IAlgFactory& OTTime2MCParticleAlgFactory = s_factory ; 

OTTime2MCParticleAlg::OTTime2MCParticleAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Algorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", m_outputData  = OTTime2MCParticleLocation );
  declareProperty( "associatorName", m_nameAsct = "OTTime2MCHitAsct" );
}

OTTime2MCParticleAlg::~OTTime2MCParticleAlg() {
  // destructor
}

StatusCode OTTime2MCParticleAlg::initialize() 
{
  StatusCode sc = toolSvc()->retrieveTool(m_nameAsct, m_hAsct);
  if ( sc.isFailure() || 0 == m_hAsct ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << "Unable to retrieve Associator tool" << endmsg;
    return sc;
  } 
  return StatusCode::SUCCESS;
}


StatusCode OTTime2MCParticleAlg::execute() 
{
  // get OTTimes
  SmartDataPtr<OTTimes> timeCont(eventSvc(), OTTimeLocation::Default);
  if (0 == timeCont){ 
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find OTTimes" << endmsg;
    return StatusCode::FAILURE;
  }

  // create an association table and register table in store
  OTTime2MCParticleAsct::Table* aTable = new OTTime2MCParticleAsct::Table();
  StatusCode sc = eventSvc()->registerObject(outputData(), aTable);
  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::FATAL << "Could not register " << outputData() << endmsg;
    delete aTable;
    return StatusCode::FAILURE;
  }

  // loop and link OTTimes to MC truth
  OTTimes::const_iterator iterTime;
  for ( iterTime = timeCont->begin(); 
        iterTime != timeCont->end(); ++iterTime){
    std::vector<MCParticle*> partVector;
    associateToTruth(*iterTime, partVector);
    std::vector<MCParticle*>::iterator iPart = partVector.begin();
    while (iPart != partVector.end()) {
      aTable->relate(*iterTime, *iPart);
      ++iPart;
    }
  } // loop iterTime

  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCParticleAlg::finalize()
{
  // Release tool
  if( m_hAsct ) toolSvc()->releaseTool( m_hAsct );

  return StatusCode::SUCCESS;
}

StatusCode 
OTTime2MCParticleAlg::associateToTruth(const OTTime* aTime,
                                       std::vector<MCParticle*>& partVector){

  // make truth link to MCParticle and retrieve table
  OTTime2MCHitAsct::DirectType* aTable = m_hAsct->direct();
  if (0 == aTable){
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find table" << endmsg;
    return StatusCode::FAILURE;
  }
 
  OTTime2MCHitAsct::MCHits range = aTable->relations(aTime);
  if ( !range.empty() ) {
    OTTime2MCHitAsct::MCHitsIterator iterHit;
    for (iterHit = range.begin(); iterHit != range.end(); ++iterHit) {
      MCHit* aHit = iterHit->to();
      if (0 != aHit) {
        MCParticle* aParticle = aHit->mcParticle();
        if (0 != aParticle) {
          partVector.push_back( aParticle );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}
