// $Id: DummyLumiAlley.cpp,v 1.2 2008-08-01 08:13:21 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/L0DUReport.h"
#include "Event/HltSummary.h"

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
  , m_selection(*this)
{
  declareProperty("L0DULocation", m_l0Location = L0DUReportLocation::Default );
  m_selection.declareProperties();
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
  m_selection.registerSelection();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode DummyLumiAlley::execute() {

  debug() << " Entering the execute " << endreq;

  LHCb::L0DUReport *l0 = get<L0DUReport>(m_l0Location);

  m_selection.output()->setDecision(true);

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



