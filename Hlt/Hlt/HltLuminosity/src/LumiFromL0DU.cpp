// $Id: LumiFromL0DU.cpp,v 1.7 2009-02-18 13:11:13 panmanj Exp $
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
  declareProperty( "InputSelection"      , m_InputSelectionName  = "Trig/L0/L0DUReport");
  declareProperty( "CounterName"         , m_CounterName);
  declareProperty( "ValueName"           , m_ValueName);
  declareProperty( "OutputContainer"     , m_OutputContainerName =  LHCb::HltLumiSummaryLocation::Default );
  declareProperty( "L0DUFromRawToolType" , m_fromRawTool         = "L0DUFromRawTool" );

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

  info() << "InputSelection         " << m_InputSelectionName  << endmsg;
  info() << "CounterName            " << m_CounterName         << endmsg;
  info() << "ValueName              " << m_ValueName           << endmsg;
  info() << "OutputContainer        " << m_OutputContainerName << endmsg;


  // get the L0 decoding tool
  m_fromRaw = tool<IL0DUFromRawTool>( m_fromRawTool , m_fromRawTool  );

  // ------------------------------------------
  m_Counter = LHCb::LumiCounters::counterKeyToType(m_CounterName);
  if ( m_Counter == LHCb::LumiCounters::Unknown ) {
    warning() << "LumiCounter not found with name: " << m_CounterName <<  endmsg;
  } else {
    info() << "ExtraInfo key value: " << m_Counter << endmsg;
  }
  // ------------------------------------------
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiFromL0DU::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // decode the bank using the fromRaw Tool
  if ( !m_fromRaw->decodeBank() ) Warning("Unable to decode L0DU rawBank", StatusCode::SUCCESS).ignore();
  LHCb::L0DUReport fromRawReport = m_fromRaw->report();
  // get the value using its name from the fromRawReport
  double fromRawValue = fromRawReport.dataValue(m_ValueName);
  debug() << "found value from the raw L0DU report for " << m_ValueName << " " << fromRawValue << endreq ;
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

