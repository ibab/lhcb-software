// Include files 

// local
#include "Event/ProcStatus.h"
#include "AddToProcStatus.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AddToProcStatus
//
// 2011-06-15 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AddToProcStatus )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AddToProcStatus::AddToProcStatus( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  //  addAlgorithmStatus (const std::string &algName, const std::string &subsystem, 
  //                    const std::string &reason, const int status, const bool eventAborted)
  declareProperty("Subsystem", m_subsystem = "", "Subsystem that has aborted");  
  declareProperty("Reason", m_reason = "", "Reason");
  declareProperty("Status", m_status = -3, "Status Code");
  declareProperty("Abort", m_abort = true, "Abort Event?");
}
//=============================================================================
// Destructor
//=============================================================================
AddToProcStatus::~AddToProcStatus() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode AddToProcStatus::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( ""==m_reason) {
    Error("Please provide a Reason");
    return StatusCode::FAILURE;
  }
  if ( ""==m_subsystem) {
    Error("Please provide a Subsystem");
    return StatusCode::FAILURE;
  }

  info() << "Whenever called will add the following to ProcStatus:" << endmsg;
  info() << "  Algorithm: " << name() << ", Subsystem: " << m_subsystem 
         << ", Status: " << m_status << ", Abort: " << m_abort << endmsg ;
  

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AddToProcStatus::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
    
  LHCb::ProcStatus* procStat =
    getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(LHCb::ProcStatusLocation::Default);
    
  // give some indication that we had to skip this event
  // (ProcStatus returns zero status for us in cases where we don't
  // explicitly add a status code)
  procStat->addAlgorithmStatus(name(), m_subsystem, m_reason, m_status , m_abort );
  return Warning("Processing aborted", StatusCode::SUCCESS, 0);
}

//=============================================================================
