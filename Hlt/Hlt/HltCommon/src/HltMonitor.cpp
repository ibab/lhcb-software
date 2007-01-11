// $Id: HltMonitor.cpp,v 1.5 2007-01-11 14:07:13 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "HltBase/ParserDescriptor.h"
// local
#include "HltMonitor.h"


//-----------------------------------------------------------------------------
// Implementation file for class : HltMonitor
//
// 2006-09-01 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMonitor::HltMonitor( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : HltMonitorAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
HltMonitor::~HltMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMonitor::initialize() {
  StatusCode sc = HltMonitorAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // initializeHistosFromDescriptor();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode HltMonitor::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  bool ok = HltMonitorAlgorithm::beginExecute();
  if (!ok) return sc;


  // TODO
  // monitor(m_tracks,m_keys);
  // monitor(m_vertices,m_keys);

  debug() << "==> Execute" << endmsg;

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  return HltMonitorAlgorithm::finalize();
}

//=============================================================================
