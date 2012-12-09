// $Id: DummyAnalysisAlg.cpp,v 1.1 2009-10-02 07:24:53 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "DummyAnalysisAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DummyAnalysisAlg
//
// 2009-10-02 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DummyAnalysisAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DummyAnalysisAlg::DummyAnalysisAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : DaVinciTupleAlgorithm ( name , pSvcLocator )
{ }

//=============================================================================
// Destructor
//=============================================================================
DummyAnalysisAlg::~DummyAnalysisAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DummyAnalysisAlg::initialize() 
{
  StatusCode sc = DaVinciTupleAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DummyAnalysisAlg::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // code goes here  

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted. 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DummyAnalysisAlg::finalize()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciTupleAlgorithm::finalize();
}
//=============================================================================
