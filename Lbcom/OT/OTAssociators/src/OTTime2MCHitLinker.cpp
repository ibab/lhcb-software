// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Linker
#include "Linker/LinkerTool.h"

// MCEvent
#include "Event/MCHit.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCOTTime.h"

// Event
//#include "Event/OTTime.h"

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
static const  AlgFactory<OTTime2MCHitLinker>   s_factory ;
const IAlgFactory& OTTime2MCHitLinkerFactory = s_factory ; 

OTTime2MCHitLinker::OTTime2MCHitLinker( const std::string& name,
					ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator ) 
{
  // constructor
  declareProperty( "OutputData", m_outputData  = "OTTime2MCHitLocation" );
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
  if ( sc.isFailure() ) 
    {
      return Error( "Failed to intialize" , sc );
    }
  
  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCHitLinker::execute() 
{
  // Get OTTimes
  //LHCb::OTTimes* timeCont = get<LHCb::OTTimes>( LHCb::OTTimeLocation::Default );
  LHCb::MCOTTimes* timeCont = get<LHCb::MCOTTimes>( LHCb::MCOTTimeLocation::Default );
    
  // Get MCHits; no spillover 
  LHCb::MCHits* mcHits = get<LHCb::MCHits>( "/Event/"+LHCb::MCHitLocation::OT );

  // Create a linker
  //LinkerWithKey<LHCb::MCHit,LHCb::OTTime> myLink( evtSvc(), msgSvc(), outputData() );
  LinkerWithKey<LHCb::MCHit,LHCb::MCOTTime> myLink( evtSvc(), msgSvc(), outputData() );

  // loop and link OTTimes to MC truth
  //LHCb::OTTimes::const_iterator iterTime;
  LHCb::MCOTTimes::const_iterator iterTime;
  for ( iterTime = timeCont->begin(); 
        iterTime != timeCont->end(); ++iterTime ){
    
    // Find and link all hits 
    std::vector<const LHCb::MCHit*> hitVec;
    associateToTruth( *iterTime, hitVec, mcHits );
    std::vector<const LHCb::MCHit*>::iterator iHit = hitVec.begin();
    while ( iHit != hitVec.end() ) {
      myLink.link( *iterTime , *iHit );
      ++iHit;
    } // while iHit != hitVec.end()
  } // for iterTime

  return StatusCode::SUCCESS;
}

// StatusCode OTTime2MCHitLinker::associateToTruth( const LHCb::OTTime* aTime,
// 						 std::vector<const LHCb::MCHit*>& hitVec, 
// 						 LHCb::MCHits* mcHits )
StatusCode OTTime2MCHitLinker::associateToTruth( const LHCb::MCOTTime* aTime,
						 std::vector<const LHCb::MCHit*>& hitVec, 
						 LHCb::MCHits* mcHits )
{
  // Make link to MCHit from OTTime
  //typedef LinkerTool<LHCb::OTTime, LHCb::MCOTDeposit> OTTime2MCDepositAsct;
  typedef LinkerTool<LHCb::MCOTTime, LHCb::MCOTDeposit> OTTime2MCDepositAsct;
  typedef OTTime2MCDepositAsct::DirectType Table;
  typedef Table::Range MCDeposit;
  typedef Table::iterator MCDepositIter;
  
  OTTime2MCDepositAsct associator( evtSvc(), LHCb::MCOTTimeLocation::Default );
  const Table* aTable = associator.direct();
  if( !aTable ) return Error( "Failed to find table", StatusCode::FAILURE );

  MCDeposit range = aTable->relations( aTime );
  if ( !range.empty() ) {
    MCDepositIter iterDep;
    for ( iterDep = range.begin() ; iterDep != range.end() ; ++iterDep ) {      
      const LHCb::MCOTDeposit* aDeposit = iterDep->to();
      if ( 0 != aDeposit ) {
	const LHCb::MCHit* aHit = aDeposit->mcHit();
	if ( 0 != aHit ) {
	  if ( m_spillOver || ( mcHits == aHit->parent() ) ) {
	    hitVec.push_back( aHit );
	  } // if  m_spillOver || ( m_mcHits == aHit->parent())
	} // if 0 != aHit
      } // if 0 != aDeposit
    } // for iterDep
  } // if !MCDeposit.empty()
  
  return StatusCode::SUCCESS;
}
