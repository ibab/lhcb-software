// $Id: HltSelectionFilter.cpp,v 1.2 2008-01-31 15:16:07 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "HltSelectionFilter.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltSelectionFilter
//
// 2006-01-16 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelectionFilter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelectionFilter::HltSelectionFilter( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  m_doInitSelections = false;
  m_algoType = "HltSelectionFilter";
}
//=============================================================================
// Destructor
//=============================================================================
HltSelectionFilter::~HltSelectionFilter() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelectionFilter::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_considerInputs = false;

  const std::vector<std::string>& values = 
    m_extraInputSelectionsNames.value();
  for (std::vector<std::string>::const_iterator it = values.begin();
       it != values.end(); ++it){
    const std::string& selname = (*it);
    retrieveSelection(selname);
    m_scounters.push_back(0);
  }
  
  m_outputSelections = 
    &(registerTSelection<Hlt::Selection>(m_outputSelectionName));
  
  saveConfiguration();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelectionFilter::execute() {
  
  size_t i = 0;
  for (Hlt::SelectionIterator it = m_inputSelections.begin();
       it != m_inputSelections.end(); ++it, ++i) {
    Hlt::Selection& sel = *(*it);
    if (sel.decision()) {
      debug() << " positive selection " << m_inputSelectionsNames[i] << endreq;
      m_outputSelections->push_back(&sel);
      m_scounters[i] += 1; 
    }
  }  

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelectionFilter::finalize() {

  StatusCode sc =  HltAlgorithm::finalize();  
  for (size_t i = 0; i < m_scounters.size(); ++i){
    std::string title = m_inputSelectionsNames[i];
    infoSubsetEvents(m_scounters[i],m_counterEntries,title);
  }
  return sc;
}

//=============================================================================
