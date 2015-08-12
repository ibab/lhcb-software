// Include files 

// event model
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"
#include "Event/LumiMethods.h"

// local
#include "FilterOnLumiSummary.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterOnLumiSummary
//
// 2010-01-29 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterOnLumiSummary )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterOnLumiSummary::FilterOnLumiSummary( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_Counter(0),
    m_Value(0)
{
  declareProperty( "InputDataContainer",  m_DataName = LHCb::HltLumiSummaryLocation::Default );
  declareProperty( "CounterName"       ,  m_CounterName = "Method" );
  declareProperty( "ValueName"         ,  m_ValueName = "" );
}
//=============================================================================
// Destructor
//=============================================================================
FilterOnLumiSummary::~FilterOnLumiSummary() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterOnLumiSummary::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "==> Initialize" << endmsg;
    debug() << "CounterName:       " << m_CounterName         << " "
            << "ValueName:         " << m_ValueName           << endmsg;
  }

  // ------------------------------------------
  m_Counter = LHCb::LumiCounters::counterKeyToType(m_CounterName);
  if ( m_Counter == LHCb::LumiCounters::Unknown ) {
    warning() << "LumiCounter not found with name: " << m_CounterName <<  endmsg;
  } else {
    if ( msgLevel(MSG::DEBUG) ) debug() << "ExtraInfo key value: " << m_Counter << endmsg;
  }
  // ------------------------------------------
  m_Value = LHCb::LumiMethods::methodKeyToType(m_ValueName);
  if ( m_Value == LHCb::LumiMethods::Unknown ) {
    warning() << "LumiMethod not found with name: " << m_ValueName <<  endmsg;
  } else {
    if ( msgLevel(MSG::DEBUG) ) debug() << "ExtraInfo method key value: " << m_Value << endmsg;
  }
  // ------------------------------------------

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterOnLumiSummary::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get data container
  if( !exist<LHCb::HltLumiSummary>(m_DataName) ){
    setFilterPassed(false);
    // not sure that this is an error - physics events do not have the lumi bank
    return Warning("LumiSummary cannot be loaded", StatusCode::SUCCESS);
  }
  if ( msgLevel(MSG::DEBUG) ) debug() << "LumiSummary loaded!!" << endmsg;
  LHCb::HltLumiSummary* the_hltLumiSummary = get<LHCb::HltLumiSummary>(m_DataName);

  // look at the specified value
  int ivalue = the_hltLumiSummary->info(m_Counter,-1);
  setFilterPassed(ivalue == m_Value);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterOnLumiSummary::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

