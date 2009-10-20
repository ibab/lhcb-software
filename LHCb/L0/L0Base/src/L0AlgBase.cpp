// $Id: L0AlgBase.cpp,v 1.2 2009-10-20 13:28:21 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "L0Base/L0AlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0AlgBase
//
// 2009-10-20 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0AlgBase );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0AlgBase::L0AlgBase( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

  declareProperty("WriteBanks"  , m_writeBanks   = true);
  declareProperty("WriteOnTES"  , m_writeOnTES   = false);
  declareProperty("L0Context"   , m_l0context   = "");

}
//=============================================================================
// Destructor
//=============================================================================
L0AlgBase::~L0AlgBase() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0AlgBase::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0AlgBase::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0AlgBase::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
