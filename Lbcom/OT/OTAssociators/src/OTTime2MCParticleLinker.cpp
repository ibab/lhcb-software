// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Linker
#include "Linker/LinkerTool.h"

// MCEvent
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
//#include "Event/MCOTTime.h"
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
static const  AlgFactory<OTTime2MCParticleLinker> s_factory ;
const IAlgFactory& OTTime2MCParticleLinkerFactory = s_factory ; 

OTTime2MCParticleLinker::OTTime2MCParticleLinker( const std::string& name,
						  ISvcLocator* pSvcLocator )
  : GaudiAlgorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", m_outputData  = "/Event/Link/Raw/OT/Times" );
}

OTTime2MCParticleLinker::~OTTime2MCParticleLinker() {
  // destructor
}

StatusCode OTTime2MCParticleLinker::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) { 
    return Error( "Failed to initialize", sc );
  }
    
  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCParticleLinker::execute() 
{
  // get OTTimes
  //  LHCb::MCOTTimes* timeCont = get<LHCb::MCOTTimes>( LHCb::MCOTTimeLocation::Default );
  LHCb::OTTimes* timeCont = get<LHCb::OTTimes>( LHCb::OTTimeLocation::Default );

  // Create a linker
  //LinkerWithKey<LHCb::MCParticle,LHCb::MCOTTime> myLink( evtSvc(), msgSvc(), outputData() );
  LinkerWithKey<LHCb::MCParticle,LHCb::OTTime> myLink( evtSvc(), msgSvc(), outputData() );
  
  // loop and link OTTimes to MC truth
  //LHCb::MCOTTimes::const_iterator iterTime;
  LHCb::OTTimes::const_iterator iterTime;
  for ( iterTime = timeCont->begin(); 
        iterTime != timeCont->end(); ++iterTime ){
    std::vector<const LHCb::MCParticle*> partVec;
    StatusCode sc=associateToTruth( *iterTime, partVec );
    if ( !sc.isSuccess() ) return Error( "Failed to associate to truth" , sc );
    std::vector<const LHCb::MCParticle*>::iterator iPart = partVec.begin();
    while ( iPart != partVec.end() ){
      myLink.link( *iterTime, *iPart );
      ++iPart;
    } // while iPart != partVec.end()
  } // for iterTime

  return StatusCode::SUCCESS;
}

// StatusCode OTTime2MCParticleLinker::associateToTruth( const LHCb::MCOTTime* aTime,
// 						      std::vector<const LHCb::MCParticle*>& partVec )
StatusCode OTTime2MCParticleLinker::associateToTruth( const LHCb::OTTime* aTime,
						      std::vector<const LHCb::MCParticle*>& partVec )
{
 
  //typedef LinkerTool<LHCb::MCOTTime, LHCb::MCHit> OTTime2MCHitAsct;
  typedef LinkerTool<LHCb::OTTime, LHCb::MCHit> OTTime2MCHitAsct;
  typedef OTTime2MCHitAsct::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;
  
  OTTime2MCHitAsct associator( evtSvc(), "/Event/Link/OTTimes2MCHits" );
  const Table* aTable = associator.direct();
  if( !aTable ) return Error( "Failed to find table", StatusCode::FAILURE );

  Range range = aTable->relations( aTime );
  if ( !range.empty() ) {
    iterator iterHit;
    for ( iterHit = range.begin(); iterHit != range.end(); ++iterHit ) {
      const LHCb::MCHit* aHit = iterHit->to();
      if ( 0 != aHit) {
        const LHCb::MCParticle* aParticle = aHit->mcParticle();
        if ( 0 != aParticle ) {
          partVec.push_back( aParticle );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}
