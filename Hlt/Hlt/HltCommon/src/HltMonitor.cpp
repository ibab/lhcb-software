// $Id: HltMonitor.cpp,v 1.3 2006-10-24 09:44:03 hernando Exp $
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

  const std::vector< std::string >& des = m_histoDescriptor.value();
  info() << " histo descriptor size " << des.size() << endreq;  
  for (std::vector<std::string >::const_iterator it = des.begin();
       it != des.end(); ++it) {
    const std::string& des = *it;
    std::string title = "";
    int nbins = 100;
    float x0 = 0.;
    float xf = 100.;
    info() << " descriptor " << des << endreq;
    bool ok =  ParserDescriptor::parseHisto1D(des,title,nbins,x0,xf);
    if (ok) {
      m_keys.push_back(title);
      book1D(title,x0,xf,nbins);
      info() << " booking histo  " << title 
             << "( "<< nbins << " , "<< x0 <<" , " << xf << ") ";  
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode HltMonitor::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  bool ok = HltMonitorAlgorithm::beginExecute();
  if (!ok) return sc;

  monitor(m_tracks,m_keys);

  monitor(m_vertices,m_keys);

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
