// $Id: HltTrackMerge.cpp,v 1.2 2007-07-02 21:04:26 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// local
#include "HltTrackMerge.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackMerge
//
// 2007-03-22 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackMerge );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackMerge::HltTrackMerge( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackMerge::~HltTrackMerge() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackMerge::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  checkInput(m_inputTracks," input tracks ");
  checkInput(m_inputTracks2," input tracks [2]");
  checkInput(m_outputTracks," output tracks ");

  return StatusCode::SUCCESS;
}

//===========================================================================
// Main execution
//===========================================================================
StatusCode HltTrackMerge::execute() {

  debug() << "==> Execute" << endmsg;

  copy(*m_inputTracks, *m_outputTracks);
  copy(*m_inputTracks2,*m_outputTracks);

  int n = m_outputTracks->size();
  debug() << " merged tracks " << n <<endreq;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackMerge::finalize() {

  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
