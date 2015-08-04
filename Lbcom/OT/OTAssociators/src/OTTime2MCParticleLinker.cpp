// Linker
#include "Linker/LinkerTool.h"

// MCEvent
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/OTTime.h"

// local
#include "OTTime2MCParticleLinker.h"

/** @file OTTime2MCParticleLinker.cpp 
 *
 *  Implementation of the OTTime2MCParticleLinker class
 *  
 *  @author J. van Tilburg and Jacopo Nardulli
 *  @date   15/06/2004
 */

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTTime2MCParticleLinker )

OTTime2MCParticleLinker::OTTime2MCParticleLinker( const std::string& name,
						  ISvcLocator* pSvcLocator )
  : GaudiAlgorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", m_outputData = LHCb::OTTimeLocation::Default );
}

OTTime2MCParticleLinker::~OTTime2MCParticleLinker() {
  // destructor
}

StatusCode OTTime2MCParticleLinker::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize", sc );
      
  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCParticleLinker::execute() 
{
  // get OTTimes
  LHCb::OTTimes* timeCont = get<LHCb::OTTimes>( LHCb::OTTimeLocation::Default );

  // Create a linker
  LinkerWithKey<LHCb::MCParticle,LHCb::OTTime> myLink( evtSvc(), msgSvc(), outputData() );
  
  // loop and link OTTimes to MC truth
  LHCb::OTTimes::const_iterator iterTime = timeCont->begin();
  for ( ; iterTime != timeCont->end(); ++iterTime ) {
    std::vector<const LHCb::MCParticle*> partVec;
    StatusCode sc=associateToTruth( *iterTime, partVec );
    if ( !sc.isSuccess() ) return Error( "Failed to associate to truth" , sc );
    std::vector<const LHCb::MCParticle*>::iterator iPart = partVec.begin();
    while ( iPart != partVec.end() ) {
      myLink.link( *iterTime, *iPart );
      ++iPart;
    } // while iPart != partVec.end()
  } // for iterTime

  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCParticleLinker::associateToTruth( const LHCb::OTTime* aTime,
						      std::vector<const LHCb::MCParticle*>& partVec )
{
 
  typedef LinkerTool<LHCb::OTTime, LHCb::MCHit> OTTime2MCHitAsct;
  typedef OTTime2MCHitAsct::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;
  
  OTTime2MCHitAsct associator( evtSvc(), LHCb::OTTimeLocation::Default+"2MCHits" );
  const Table* aTable = associator.direct();
  if( !aTable ) return Error( "Failed to find table", StatusCode::FAILURE );

  Range range = aTable->relations( aTime );
  if ( !range.empty() ) {
    iterator iterHit;
    for ( iterHit = range.begin(); iterHit != range.end(); ++iterHit ) {
      const LHCb::MCHit* aHit = iterHit->to();
      if (aHit) {
        const LHCb::MCParticle* aParticle = aHit->mcParticle();
        if (aParticle) {
          partVec.push_back( aParticle );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}
