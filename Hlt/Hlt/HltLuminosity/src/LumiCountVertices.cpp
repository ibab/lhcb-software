// $Id: LumiCountVertices.cpp,v 1.6 2009-02-18 13:11:13 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "Event/Vertex.h"
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

// local
#include "LumiCountVertices.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiCountVertices
//
// 2008-07-21 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiCountVertices );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiCountVertices::LumiCountVertices( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputSelection" ,      m_InputSelectionName);
  declareProperty( "CounterName"    ,      m_CounterName);
  declareProperty( "OutputContainer",      m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );

}
//=============================================================================
// Destructor
//=============================================================================
LumiCountVertices::~LumiCountVertices() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiCountVertices::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  info() << "InputSelection         " << m_InputSelectionName  << endmsg;
  info() << "CounterName            " << m_CounterName         << endmsg;
  info() << "OutputContainer        " << m_OutputContainerName << endmsg;


  // ------------------------------------------
  m_Counter = LHCb::LumiCounters::counterKeyToType(m_CounterName);
  if ( m_Counter == LHCb::LumiCounters::Unknown ) {
    info() << "LumiCounter not found with name: " << m_CounterName <<  endmsg;
  } else {
    info() << m_CounterName << " key value: " << m_Counter << endmsg;
  }
  // ------------------------------------------
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiCountVertices::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // load the track objects
  int nCounter =  0;
  if ( !exist<LHCb::RecVertices>(m_InputSelectionName) ){
    warning() << m_InputSelectionName << " not found" << endmsg ;
  } else {
    // get the container
    m_InputContainer = get<LHCb::RecVertices>(m_InputSelectionName);
  
    if ( !m_InputContainer ) { 
      err() << "Could not find location " 
	    <<  m_InputSelectionName << endreq;
      return StatusCode::FAILURE ;
    }
    nCounter = m_InputContainer->size() ;
    verbose() << "found " << nCounter << " vertices." << endreq ;
  }
  debug() << "There are " << nCounter << " vertices in " << m_InputSelectionName <<  endreq ;


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
StatusCode LumiCountVertices::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

