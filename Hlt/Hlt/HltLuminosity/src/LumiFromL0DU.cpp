// $Id: LumiFromL0DU.cpp,v 1.9 2009-11-13 13:08:52 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

// local
#include "LumiFromL0DU.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiFromL0DU
//
// 2008-07-21 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiFromL0DU );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiFromL0DU::LumiFromL0DU( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputSelection"      , m_InputSelectionName =  LHCb::L0DUReportLocation::Default );
  declareProperty( "CounterName"         , m_CounterName);
  declareProperty( "ValueName"           , m_ValueName);
  declareProperty( "OutputContainer"     , m_OutputContainerName =  LHCb::HltLumiSummaryLocation::Default );

}
//=============================================================================
// Destructor
//=============================================================================
LumiFromL0DU::~LumiFromL0DU() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiFromL0DU::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  debug() << "InputSelection:    " << m_InputSelectionName  << " "
         << "CounterName:       " << m_CounterName         << " "
         << "OutputContainer:   " << m_OutputContainerName << " "
         << "ValueName:         " << m_ValueName           << endmsg;


  // ------------------------------------------
  m_Counter = LHCb::LumiCounters::counterKeyToType(m_CounterName);
  if ( m_Counter == LHCb::LumiCounters::Unknown ) {
    warning() << "LumiCounter not found with name: " << m_CounterName <<  endmsg;
  } else {
    debug() << "ExtraInfo key value: " << m_Counter << endmsg;
  }
  // ------------------------------------------
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiFromL0DU::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  const LHCb::L0DUReport*  report = get<LHCb::L0DUReport>( m_InputSelectionName );
  // get the value using its name from the fromRawReport
  double fromRawValue = report->dataValue(m_ValueName);
  if (msgLevel(MSG::DEBUG)) debug() << "found value from the L0DU report for " << m_ValueName << " " << fromRawValue << endreq ;
  int nCounter = (int) fromRawValue;

  // get container
  LHCb::HltLumiSummary* sums = getOrCreate<HltLumiSummary,HltLumiSummary>(m_OutputContainerName);
  // add counter
  sums->addInfo( m_Counter, nCounter);

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiFromL0DU::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

