// $Id: DummyLumiAlley.cpp,v 1.1.1.1 2008-07-17 08:50:25 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "DummyLumiAlley.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : DummyLumiAlley
//
// 2006-01-16 : Jose Angel Hernando Morata
// 2008-07-16 : Jaap Panman copying from the above
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DummyLumiAlley );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DummyLumiAlley::DummyLumiAlley( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("L0DULocation", m_l0Location = L0DUReportLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
DummyLumiAlley::~DummyLumiAlley() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DummyLumiAlley::initialize() {
 // debug() << "==> Initialize" << endmsg;
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // default configuration
  registerSelection();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode DummyLumiAlley::execute() {

  m_l0 = get<L0DUReport>(m_l0Location);
  
  debug() << " Entering the execute " << endreq;

  setDecision(true);

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DummyLumiAlley::finalize() {
  debug() << " Entering the finalize " << endreq;
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



