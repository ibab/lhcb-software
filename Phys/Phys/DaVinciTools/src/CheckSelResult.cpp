// $Id$
// Include files 

// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/RecHeader.h"

// local
#include "CheckSelResult.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckSelResult
//
// 2004-07-14 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( CheckSelResult )

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckSelResult::CheckSelResult( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_algorithms ()
{
  m_algorithms.clear();
  declareProperty( "Algorithms", m_algorithms, "List of algorithms" );
  declareProperty( "Andmode", m_ANDmode = false, "require all algos to pass" );
  declareProperty( "Notmode", m_NOTmode = false, "revert decision");
}
//=============================================================================
// Destructor
//=============================================================================
CheckSelResult::~CheckSelResult() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode CheckSelResult::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc; 

  if (msgLevel(MSG::VERBOSE)) verbose() << "==> Initialize" << endmsg;
  if (m_algorithms.empty()){
    warning() << "No algorithms defined. Use Algorithms option." << endmsg;;    
  }
  if ((m_ANDmode) && (m_algorithms.size()>1)){
    warning() << "You have selected the 'AND' mode: "
              << "ALL algorithms are required to pass!" << endmsg;
  } else if (msgLevel(MSG::DEBUG)) debug() << "You have selected the default 'OR' mode." << endmsg;
  if (m_NOTmode) warning() << "You have selected NOT mode: will invert decision" << endmsg ;

  m_readTool = tool<ICheckSelResults>("CheckSelResultsTool",this);

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheckSelResult::execute() 
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "==> Execute" << endmsg;
  bool pass = m_readTool->isSelected(m_algorithms, m_ANDmode) ;
  if (m_NOTmode) pass = !pass ;
  if (msgLevel(MSG::DEBUG)) debug() << "Result is " << pass << endmsg ;

  setFilterPassed(pass);
  
  counter("Passed") += pass;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CheckSelResult::finalize() {

  const StatEntity& stat = counter("Passed");
  
  info() << "Filtered "  << stat.flag() << "/" << stat.nEntries() << " events "
         << " (Eff = " << stat.flagMean() 
         << " +/- " << stat.flagRMS() << " )" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
