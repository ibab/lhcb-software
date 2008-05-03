// $Id: HltTrackUpgrade.cpp,v 1.5 2008-05-03 15:24:44 graven Exp $
// Include files
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

// local
#include "HltTrackUpgrade.h"
#include "HltBase/HltConfigurationHelper.h"

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
  m_doInitSelections = false;
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
  m_outputTracks = &(registerTSelection<LHCb::Track>(m_outputSelectionName));

  m_tool = tool<HltTrackUpgradeTool>("HltTrackUpgradeTool",this);
  if (!m_tool) 
    fatal() << " not able to retrieve upgrade track tool " << endreq;
  
  m_tool->setReco(m_recoName);

  saveConfiguration();
  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackUpgrade::execute() {

  StatusCode sc = m_tool->upgrade(*m_inputTracks,*m_outputTracks);
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
  info() << " HLT Recostruction " << m_recoName << endreq;
}
