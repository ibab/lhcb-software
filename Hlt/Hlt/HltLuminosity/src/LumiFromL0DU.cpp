// $Id: LumiFromL0DU.cpp,v 1.10 2010-03-28 12:48:59 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/L0DUReport.h"
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
  declareProperty( "OutputContainer"     , m_OutputContainerName =  LHCb::HltLumiSummaryLocation::Default );
  declareProperty( "CounterMap"          , m_CounterValueProperty);

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

  if ( msgLevel(MSG::DEBUG) ) { 
      debug() << "==> Initialize" << endmsg;

      debug() << "InputSelection: " << m_InputSelectionName  << " "
             << "OutputContainer: " << m_OutputContainerName << endmsg;
  }

  //TODO: properly hook this into a property -- either use updateHandler, or custom property type...
  for (auto& i : m_CounterValueProperty) {
            int counter = LHCb::LumiCounters::counterKeyToType(i.first);
            if (msgLevel(MSG::DEBUG)) { 
                debug() << " adding " << i.second << " as " << i.first << " ("<< counter<< ")"<< endmsg;
            }
            if ( counter == LHCb::LumiCounters::Unknown ) {
                error() << "LumiCounter " << i.first <<  " not known " <<endmsg;
                return StatusCode::FAILURE;
            }
            m_CounterValue.emplace_back( counter,i.second );
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiFromL0DU::execute() {
  setFilterPassed(true);
  // get/create ouput
  LHCb::HltLumiSummary* sums = getOrCreate<HltLumiSummary,HltLumiSummary>(m_OutputContainerName);
  // get input
  const LHCb::L0DUReport* report = getIfExists<LHCb::L0DUReport>( m_InputSelectionName );
  for (auto& i : m_CounterValue ) {
      // get the value using its name from the fromRawReport
      // add counter (must be integer, so we force it to be so...
      sums->addInfo( i.first, report ? (int)report->dataValue(i.second) : -1 );
  }
  return StatusCode::SUCCESS;
}
