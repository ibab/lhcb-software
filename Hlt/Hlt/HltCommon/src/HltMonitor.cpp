// $Id: HltMonitor.cpp,v 1.1 2006-09-22 11:36:09 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

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
  : HltAlgorithm ( name , pSvcLocator )
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
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
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
      m_titles.push_back(title);
      book1D(title,x0,xf,nbins);
      info() << " booking histogram from descriptor " << title << endreq;
    }
  }

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode HltMonitor::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  bool ok = HltAlgorithm::beginExecute();
  if (!ok) return sc;

  monitor(m_inputTracks);

  monitor(m_patInputTracks);

  monitor(m_inputVertices);

  HltAlgorithm::endExecute();

  debug() << "==> Execute" << endmsg;

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
