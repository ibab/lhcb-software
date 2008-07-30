// $Id: HltSelectionFilter.cpp,v 1.7 2008-07-30 13:37:32 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "HltSelectionFilter.h"
#include "boost/foreach.hpp"

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
  : HltAlgorithm ( name , pSvcLocator, false )
{
  declareProperty("OutputSelection", m_outputSelectionName = name);
  declareProperty("InputSelections", m_inputSelectionNames);
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

  const std::vector<std::string>& values = m_inputSelectionNames.value();
  BOOST_FOREACH( const std::string& name, values ) {
    m_input.push_back(&retrieveSelection(name));
    m_scounters.push_back(0);
  }
  
  m_output = &(registerTSelection<Hlt::Selection>(m_outputSelectionName));
  
  saveConfiguration();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelectionFilter::execute() {
  
  size_t i = 0;
  BOOST_FOREACH( Hlt::Selection* sel, m_input ) {
    if ( sel->decision()) {
      debug() << " positive selection " << sel->id().str() << endreq;
      m_output->push_back(sel);
      ++m_scounters[i];
    }
    ++i;
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
                     m_input[i]->id().str());
  }
  return HltAlgorithm::finalize();  
}

//=============================================================================
