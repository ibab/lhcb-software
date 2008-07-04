// $Id: HltSelectionFilter.cpp,v 1.6 2008-07-04 08:07:41 graven Exp $
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
  declareProperty("InputSelections", m_extraInputSelectionsNames);
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

  const std::vector<std::string>& values = m_extraInputSelectionsNames.value();
  for (std::vector<std::string>::const_iterator it = values.begin();
       it != values.end(); ++it) {
    retrieveSelection(*it);
    m_scounters.push_back(0);
  }
  
  m_outputSelections = &(registerTSelection<Hlt::Selection>());
  
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
    if ( (*it)->decision()) {
      debug() << " positive selection " << (*it)->id().str() << endreq;
      m_outputSelections->push_back(*it);
      ++m_scounters[i];
    }
  }  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelectionFilter::finalize() {

  for (size_t i = 0; i < m_scounters.size(); ++i) {
    infoSubsetEvents(m_scounters[i],
                     m_counterEntries,
                     m_inputSelections[i]->id().str());
  }
  return HltAlgorithm::finalize();  
}

//=============================================================================
