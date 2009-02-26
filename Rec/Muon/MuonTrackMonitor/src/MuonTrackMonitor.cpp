// $Id: MuonTrackMonitor.cpp,v 1.1.1.1 2009-02-26 09:29:59 ggiacomo Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "MuonTrackMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTrackMonitor
//
// 2009-02-25 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonTrackMonitor );



MuonTrackMonitor::MuonTrackMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{

}

MuonTrackMonitor::~MuonTrackMonitor() {} 



StatusCode MuonTrackMonitor::initialize() {

  const StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}


StatusCode MuonTrackMonitor::execute() {

  setFilterPassed(true);  
  return StatusCode::SUCCESS;
}



StatusCode MuonTrackMonitor::finalize() {


  return   GaudiHistoAlg::finalize();
}


