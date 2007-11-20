// $Id: HltPrepareTracks.cpp,v 1.6 2007-11-20 10:16:01 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltPrepareTracks.h"
#include "Event/HltEnums.h"
#include "HltBase/HltSequences.h"
#include "HltBase/HltFunctions.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltPrepareTracks
//
// 2006-07-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltPrepareTracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltPrepareTracks::HltPrepareTracks( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator ), _nobackwards(0)
{
  declareProperty("TESInputTracksName", m_TESInputTracksName = "");
}
//=============================================================================
// Destructor
//=============================================================================
HltPrepareTracks::~HltPrepareTracks() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltPrepareTracks::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorith
  
  // crreate a filter against the backward tracks
  Hlt::CheckFlag back(Track::Backward);
  _nobackwards = (!back).clone();

  checkInput(m_outputTracks," output tracks ");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltPrepareTracks::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  Tracks* itracks = get<Tracks>(m_TESInputTracksName);
  // select the input tracks that are non-backwards
  m_outputTracks->clear();
  Hlt::select(*itracks,*_nobackwards,*m_outputTracks);
  debug() << " non backward tracks " << m_outputTracks->size() << endreq;

  int ncan = m_outputTracks->size();
  candidateFound(ncan);

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltPrepareTracks::finalize() {
  debug() << "==> Finalize" << endmsg;
  delete _nobackwards; _nobackwards = 0;
  return HltAlgorithm::finalize();
}

//=============================================================================
