// $Id: RichRingRecTupleAlgBase.cpp,v 1.2 2009-06-05 17:21:35 jonrob Exp $
// Include files 

// local
#include "RichRingRec/RichRingRecTupleAlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingRecTupleAlgBase
//
// 2007-06-29 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;
DECLARE_ALGORITHM_FACTORY(RichRingRecTupleAlgBase)
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingRecTupleAlgBase::RichRingRecTupleAlgBase( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : Rich::Rec::TupleAlgBase ( name , pSvcLocator ),
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
StatusCode RichRingRecTupleAlgBase::initialize() 
{
  const StatusCode sc = TupleAlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize  RichRingRecTupleAlgBase " << endmsg;

   m_ringtool = tool<IRichRingRecToolBase>("Rich::Rec::TemplateRings::RichRingRecToolBase");
  
  return sc;
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

  return TupleAlgBase::finalize();   // must be called after all other actions
}

//=============================================================================
