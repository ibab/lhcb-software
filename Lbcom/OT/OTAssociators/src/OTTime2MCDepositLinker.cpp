// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// Linker 
#include "Linker/LinkerTool.h"

// MCEvent
#include "Event/MCTruth.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCOTTime.h"

// Event
#include "Event/OTTime.h"

// local
#include "OTTime2MCDepositLinker.h"

/** @file OTTime2MCDepositLinker.cpp 
 *
 *  Implementation of the OTTime2MCDepositLinker class
 *  
 *  @author J. van Tilburg and Jacopo Nardulli
 *  @date   15/06/2004
 */

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTTime2MCDepositLinker )

OTTime2MCDepositLinker::OTTime2MCDepositLinker( const std::string& name,
						ISvcLocator* pSvcLocator )
  : GaudiAlgorithm (name,pSvcLocator) 
{
  // constructor
  declareProperty( "OutputData", m_outputData = LHCb::OTTimeLocation::Default + "2MCDeposits" );
  declareProperty( "acceptTime", m_acceptTime  = 7.8*Gaudi::Units::ns );
}

OTTime2MCDepositLinker::~OTTime2MCDepositLinker() {
  // destructor
}

StatusCode OTTime2MCDepositLinker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize", sc );
    
  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCDepositLinker::execute() 
{
  // Get OTTimes
  LHCb::OTTimes* timeCont = get<LHCb::OTTimes>( LHCb::OTTimeLocation::Default );

  // Get MCOTTimes
  LHCb::MCOTTimes* mcTime = get<LHCb::MCOTTimes>( LHCb::MCOTTimeLocation::Default );  
  
  StatusCode sc = setMCTruth( timeCont, mcTime );
  if ( !sc.isSuccess() ) return Error( "Failed to set the mc truth link", sc );
  
  LinkerWithKey<LHCb::MCOTDeposit,LHCb::OTTime> myLink( evtSvc(), msgSvc(), outputData() );
 
  // loop and link OTTimes to MC truth
  //LHCb::MCOTTimes::const_iterator iterTime;
  LHCb::OTTimes::const_iterator iterTime = timeCont->begin();
  for ( ; iterTime != timeCont->end(); ++iterTime ) {
    std::vector<LHCb::MCOTDeposit*> depVec;
    sc = associateToTruth( *iterTime, depVec );
    if ( !sc.isSuccess() ) return Error( "Failed to associate to truth" , sc );
    std::vector<LHCb::MCOTDeposit*>::iterator iDep = depVec.begin();
    while ( iDep != depVec.end() ) {
      myLink.link(*iterTime, *iDep );
      ++iDep;
    } // while iDep != depVec.end()
  } // loop iterTime

  return StatusCode::SUCCESS;
}

StatusCode OTTime2MCDepositLinker::associateToTruth( const LHCb::OTTime* aTime,
 						     std::vector<LHCb::MCOTDeposit*>& depVec ) 
{  
  // Link time to truth
  const LHCb::MCOTTime* mcTime = mcTruth<LHCb::MCOTTime>( aTime );

  if (mcTime) {
    // link to deposits
    SmartRefVector<LHCb::MCOTDeposit> depCont = mcTime->deposits();
    if ( depCont.empty() ){
      error() << " Deposits Size" << depCont.size() << endmsg;
      return StatusCode::FAILURE;
    } // if depCont.empty()
  
    unsigned tdcTime = aTime->tdcTime();
  
    SmartRefVector<LHCb::MCOTDeposit>::iterator iterDep;
    for ( iterDep = depCont.begin(); iterDep != depCont.end(); ++iterDep ) {
      if( ( (*iterDep)->tdcTime() ) < ( tdcTime + m_acceptTime ) ) {
	depVec.push_back( *iterDep );
      } // if ( (*iterDep)->tdcTime() ) < ( tdcTime + m_acceptTime )
    } // for iterDep
  }

  return StatusCode::SUCCESS;
}    

