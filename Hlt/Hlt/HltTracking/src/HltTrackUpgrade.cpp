// $Id: HltTrackUpgrade.cpp,v 1.13 2008-12-29 12:37:16 graven Exp $
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
  , m_selections(*this)
{
  declareProperty("RecoName", m_recoName = "empty");  
  m_selections.declareProperties();
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

  m_selections.retrieveSelections();
  // ownership: see TrackUpgradeTool: it always assumes ownership,
  // and then transfers it to the TES...
  m_selections.registerSelection();

  m_tool = tool<HltTrackUpgradeTool>("HltTrackUpgradeTool",this);
  
  sc = m_tool->setReco(m_recoName);
  if (sc.isFailure()) return sc;
  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackUpgrade::execute() {

  //@TODO: adapt interface to be more neutral: input a vector<Track*>::const_iterator begin,end, and a back_inserter
  //       don't rely on tool doing pt ordering...
  std::vector<LHCb::Track*> in(m_selections.input<1>()->begin(),m_selections.input<1>()->end());
  assert(m_selections.output()->empty());
  std::vector<LHCb::Track*> out;
  StatusCode sc = m_tool->upgrade(in,out);
  m_selections.output()->insert(m_selections.output()->end(),out.begin(),out.end());
  if (m_debug) printInfo(" upgraded tracks ",*m_selections.output());
  
  return sc;
}

//=============================================================================

void HltTrackUpgrade::saveConfiguration() {

  HltAlgorithm::saveConfiguration();

  confregister("RecoName",m_recoName);
  info() << " HLT Reconstruction " << m_recoName << endreq;
}
