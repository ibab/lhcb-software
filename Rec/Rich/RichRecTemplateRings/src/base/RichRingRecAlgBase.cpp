// $Id: RichRingRecAlgBase.cpp,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRingRec/RichRingRecAlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingRecAlgBase
//
// 2007-05-17 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichRingRecAlgBase );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingRecAlgBase::RichRingRecAlgBase( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : RichRecAlgBase ( name , pSvcLocator ),
    m_ringtool(NULL)
{

}
//=============================================================================
// Destructor
//=============================================================================
RichRingRecAlgBase::~RichRingRecAlgBase() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichRingRecAlgBase::initialize() {
  StatusCode sc = RichRecAlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize RichRingRecAlgBase  " << endmsg;
  
  
    


  // acquiretool does not seem to work here.

    m_ringtool = tool<IRichRingRecToolBase>("RichRingRecToolBase");
  //    acquireTool("RichRingRecToolBase",m_ringtool);
          


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichRingRecAlgBase::execute() {

  debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichRingRecAlgBase::finalize() {

  debug() << "==> Finalize" << endmsg;

  return RichRecAlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
