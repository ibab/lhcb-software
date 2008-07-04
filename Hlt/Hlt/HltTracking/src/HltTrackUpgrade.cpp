// $Id: HltTrackUpgrade.cpp,v 1.11 2008-07-04 07:56:53 graven Exp $
// Include files
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

// local
#include "HltTrackUpgrade.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackUpgrade
//
// 2006-08-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackUpgrade );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackUpgrade::HltTrackUpgrade( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("RecoName", m_recoName = "empty");  
};
//=============================================================================
// Destructor
//=============================================================================
HltTrackUpgrade::~HltTrackUpgrade() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode HltTrackUpgrade::initialize() {

  StatusCode sc = HltAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_inputTracks  = &(retrieveTSelection<LHCb::Track>(m_inputSelectionName));  
  m_outputTracks = &(registerTSelection<LHCb::Track>());

  m_tool = tool<HltTrackUpgradeTool>("HltTrackUpgradeTool",this);
  
  sc = m_tool->setReco(m_recoName);
  if (sc.isFailure()) return sc;

  saveConfiguration();
  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackUpgrade::execute() {

  //@TODO: adapt interface to be more neutral: input a vector<Track*>::const_iterator begin,end, and a back_inserter
  //       don't rely on tool doing pt ordering...
  std::vector<LHCb::Track*> in(m_inputTracks->begin(),m_inputTracks->end());
  assert(m_outputTracks->empty());
  std::vector<LHCb::Track*> out;
  StatusCode sc = m_tool->upgrade(in,out);
  m_outputTracks->insert(m_outputTracks->end(),out.begin(),out.end());
  if (m_debug) printInfo(" upgraded tracks ",*m_outputTracks);
  
  return sc;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackUpgrade::finalize() {

  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

void HltTrackUpgrade::saveConfiguration() {

  HltAlgorithm::saveConfiguration();

  confregister("RecoName",m_recoName);
  info() << " HLT Reconstruction " << m_recoName << endreq;
}
