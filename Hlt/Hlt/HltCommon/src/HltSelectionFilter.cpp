// $Id: HltSelectionFilter.cpp,v 1.10 2008-12-20 17:59:36 graven Exp $
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
  // false: do not require all input selections to have passed...
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

  BOOST_FOREACH( const std::string& name, m_inputSelectionNames.value() ) {
    m_input.push_back(&retrieveSelection(name));
    counter( name );
    //TODO: register with mon service...
  }
  
  m_output = &(registerTSelection<Hlt::Selection>(m_outputSelectionName));
  
  saveConfiguration();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelectionFilter::execute() {
  
  BOOST_FOREACH( Hlt::Selection* sel, m_input ) {
    counter(sel->id().str()) += sel->decision();
    if ( sel->decision()) {
      debug() << " positive selection " << sel->id().str() << endreq;
      m_output->push_back(sel);
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
