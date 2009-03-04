// $Id: RichRingRecTupleAlgBase.cpp,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "RichRingRec/RichRingRecTupleAlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingRecTupleAlgBase
//
// 2007-06-29 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichRingRecTupleAlgBase );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingRecTupleAlgBase::RichRingRecTupleAlgBase( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : RichRecTupleAlgBase ( name , pSvcLocator ),
    m_ringtool(NULL)
{

}
//=============================================================================
// Destructor
//=============================================================================
RichRingRecTupleAlgBase::~RichRingRecTupleAlgBase() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichRingRecTupleAlgBase::initialize() {


  StatusCode sc = RichRecTupleAlgBase::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize  RichRingRecTupleAlgBase " << endmsg;

   m_ringtool = tool<IRichRingRecToolBase>("RichRingRecToolBase");
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichRingRecTupleAlgBase::execute() {

  debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichRingRecTupleAlgBase::finalize() {

  debug() << "==> Finalize" << endmsg;

  return RichRecTupleAlgBase::finalize();   // must be called after all other actions
}

//=============================================================================
