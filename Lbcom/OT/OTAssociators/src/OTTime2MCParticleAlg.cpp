// $Id: OTTime2MCParticleAlg.cpp,v 1.4 2005-03-22 10:49:29 cattanem Exp $

#include "Linker/LinkerTool.h"

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
  : GaudiAlgorithm (name,pSvcLocator) 
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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_hAsct = tool<OTTime2MCHitAsct::IAsct>( m_nameAsct );
  return StatusCode::SUCCESS;
}


StatusCode OTTime2MCParticleAlg::execute() 
{
  // get OTTimes
  OTTimes* timeCont = get<OTTimes>( OTTimeLocation::Default );

  // create an association table and register table in store
  OTTime2MCParticleAsct::Table* aTable = new OTTime2MCParticleAsct::Table();
  put( aTable, outputData() );

  // loop and link OTTimes to MC truth
  OTTimes::const_iterator iterTime;
  for ( iterTime = timeCont->begin(); 
        iterTime != timeCont->end(); ++iterTime){
    std::vector<const MCParticle*> partVector;
    StatusCode sc = associateToTruthLinker(*iterTime, partVector);
    if ( sc.isFailure() ) return sc; // error printed already by associateToTruth
    std::vector<const MCParticle*>::iterator iPart = partVector.begin();
    while (iPart != partVector.end()) {
      aTable->relate(*iterTime, *iPart);
      ++iPart;
    }
  } // loop iterTime

  return StatusCode::SUCCESS;
}


StatusCode 
OTTime2MCParticleAlg::associateToTruthLinker(const OTTime* aTime,
                      std::vector<const MCParticle*>& partVector){

  // Retrieve linker table
  LinkerTool<OTTime,MCHit> associator( evtSvc(), "OTTimes2MCHits" );
  const LinkerTool<OTTime,MCHit>::DirectType* aTable = associator.direct();
  if( 0 == aTable ) {
    info() << "OTTimes2MCHits Linker not found, try Relations" << endmsg;
    return associateToTruth( aTime, partVector );
  }

  LinkerTool<OTTime,MCHit>::DirectType::Range range = aTable->relations(aTime);
  if ( 0 != range.size() ) {
    LinkerTool<OTTime,MCHit>::DirectType::iterator iterHit;
    for (iterHit = range.begin(); iterHit != range.end(); ++iterHit) {
      const MCHit* aHit = iterHit->to();
      if (0 != aHit) {
        const MCParticle* aParticle = aHit->mcParticle();
        if (0 != aParticle) {
          partVector.push_back( aParticle );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode 
OTTime2MCParticleAlg::associateToTruth(const OTTime* aTime,
                                     std::vector<const MCParticle*>& partVector){

  // make truth link to MCParticle and retrieve table
  OTTime2MCHitAsct::DirectType* aTable = m_hAsct->direct();
  if (0 == aTable){
    return Error ("Failed to find table");
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
