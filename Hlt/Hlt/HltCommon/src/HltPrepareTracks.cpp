// $Id: HltPrepareTracks.cpp,v 1.5 2007-11-14 13:57:03 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltPrepareTracks.h"
#include "Event/HltEnums.h"
//#include "Event/HltSummaryFunctor.h"
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
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("TESInputTracksName", m_TESInputTracksName = "");
}
//=============================================================================
// Destructor
//=============================================================================
HltPrepareTracks::~HltPrepareTracks() {
  // delete _nobackwards;
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

  StatusCode sc =  HltAlgorithm::finalize();
  delete _nobackwards;
  return sc;
}

//=============================================================================
