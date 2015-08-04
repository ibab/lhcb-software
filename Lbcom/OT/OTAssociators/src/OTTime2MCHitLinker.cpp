// Linker
#include "Linker/LinkerTool.h"

// MCEvent
#include "Event/MCHit.h"
#include "Event/MCOTDeposit.h"
#include "Event/OTTime.h"

// local
#include "OTTime2MCHitLinker.h"

/** @file OTTime2MCHitLinker.cpp 
 *
 *  Implementation of the OTTime2MCHitLinker class
 *  
 *  @author J. Nardulli and J. van Tilburg
 *  @date   15/06/2004
 */

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTTime2MCHitLinker )

OTTime2MCHitLinker::OTTime2MCHitLinker( const std::string& name,
					ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator ) 
{
  // constructor
  declareProperty( "OutputData", m_outputData = LHCb::OTTimeLocation::Default + "2MCHits" );
  declareProperty( "SpillOver", m_spillOver = false );
}

OTTime2MCHitLinker::~OTTime2MCHitLinker() 
{
  // destructor
}

StatusCode OTTime2MCHitLinker::initialize() 
{
  // Get associator tool
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return Error( "Failed to intialize" , sc );
  
  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCHitLinker::execute() 
{
  // Get OTTimes
   LHCb::OTTimes* timeCont = get<LHCb::OTTimes>( LHCb::OTTimeLocation::Default );
    
  // Get MCHits; no spillover 
  LHCb::MCHits* mcHits = get<LHCb::MCHits>( LHCb::MCHitLocation::OT );

  // Create a linker
  LinkerWithKey<LHCb::MCHit,LHCb::OTTime> myLink( evtSvc(), msgSvc(), outputData() );
  
  // loop and link OTTimes to MC truth
  LHCb::OTTimes::const_iterator iterTime = timeCont->begin();
  for ( ; iterTime != timeCont->end(); ++iterTime ) {
    // Find and link all hits 
    std::vector<const LHCb::MCHit*> hitVec;
    StatusCode sc = associateToTruth( *iterTime, hitVec, mcHits );
    if ( !sc.isSuccess() ) return Error( "Failed to associate to truth" , sc );
    std::vector<const LHCb::MCHit*>::iterator iHit = hitVec.begin();
    while ( iHit != hitVec.end() ) {
      myLink.link( *iterTime , *iHit );
      ++iHit;
    } // while iHit != hitVec.end()
  } // for iterTime

  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCHitLinker::associateToTruth( const LHCb::OTTime* aTime,
						 std::vector<const LHCb::MCHit*>& hitVec, 
						 LHCb::MCHits* mcHits )
{
  // Make link to MCHit from OTTime
  typedef LinkerTool<LHCb::OTTime, LHCb::MCOTDeposit> OTTime2MCDepositAsct;
  typedef OTTime2MCDepositAsct::DirectType Table;
  typedef Table::Range MCDeposit;
  typedef Table::iterator MCDepositIter;
  
  OTTime2MCDepositAsct associator( evtSvc(), LHCb::OTTimeLocation::Default + "2MCDeposits" );
  const Table* aTable = associator.direct();
  if( !aTable ) return Error( "Failed to find table", StatusCode::FAILURE );

  MCDeposit range = aTable->relations( aTime );
  if ( !range.empty() ) {
    MCDepositIter iterDep;
    for ( iterDep = range.begin() ; iterDep != range.end() ; ++iterDep ) {      
      const LHCb::MCOTDeposit* aDeposit = iterDep->to();
      if ( aDeposit ) {
	const LHCb::MCHit* aHit = aDeposit->mcHit();
	if ( aHit ) {
	  if ( m_spillOver || ( mcHits == aHit->parent() ) ) {
	    hitVec.push_back( aHit );
	  } // if  m_spillOver || ( m_mcHits == aHit->parent())
	} // if 0 != aHit
      } // if 0 != aDeposit
    } // for iterDep
  } // if !MCDeposit.empty()
  
  return StatusCode::SUCCESS;
}
