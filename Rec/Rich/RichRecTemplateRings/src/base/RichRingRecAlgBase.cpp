// $Id: RichRingRecAlgBase.cpp,v 1.2 2009-06-05 17:21:35 jonrob Exp $
// Include files 

// local
#include "RichRingRec/RichRingRecAlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingRecAlgBase
//
// 2007-05-17 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;
DECLARE_ALGORITHM_FACTORY(RichRingRecAlgBase) 
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingRecAlgBase::RichRingRecAlgBase( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Rich::Rec::AlgBase ( name , pSvcLocator ),
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
StatusCode RichRingRecAlgBase::initialize() 
{

  const StatusCode sc = Rich::Rec::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==>  Initialize RichRingRecAlgBase  " << endmsg;
  
  
    


  // acquiretool does not seem to work here.

    m_ringtool = tool<IRichRingRecToolBase>("Rich::Rec::TemplateRings::RichRingRecToolBase");
  //    acquireTool("RichRingRecToolBase",m_ringtool);
          


  return sc;
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

  return Rich::Rec::AlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
