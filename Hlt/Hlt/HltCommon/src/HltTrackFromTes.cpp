// $Id: HltTrackFromTes.cpp,v 1.2 2007-06-20 16:06:21 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "HltBase/ParserDescriptor.h"
// local
#include "HltTrackFromTes.h"
#include "HltBase/HltFunctions.h"

#include "L0Names.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackFromTes
//
// 2006-09-01 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackFromTes );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackFromTes::HltTrackFromTes( const std::string& name,
				      ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("TracksLocation", 
		  m_tracksLocation  = LHCb::TrackLocation::Default);
  
  declareProperty("Type",m_types);

  declareProperty("OrderByPt", m_orderByPt = false);

}
//=============================================================================
// Destructor
//=============================================================================
HltTrackFromTes::~HltTrackFromTes() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackFromTes::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first

  checkInput(m_outputTracks," no output tracks!");

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode HltTrackFromTes::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  bool ok = true;

  ok = retrieve(m_tracks,m_tracksLocation);
  if (!ok) return stop("no tracks in TES");

  m_outputTracks->clear();  
  for (Tracks::iterator it = m_tracks->begin(); 
       it != m_tracks->end(); it++) {
    Track& track = (**it);
    if (track.checkType(Track::Long)) m_outputTracks->push_back(&track);
  }
  if (m_orderByPt)
    std::sort(m_outputTracks->begin(),m_outputTracks->end(),
              Hlt::SortTrackByPt());
  debug() << " transfered tracks " << m_outputTracks->size() 
          << " from TES " << m_tracks->size() << endreq;

  if (m_outputTracks->empty()) return sc;
  
  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackFromTes::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  return HltAlgorithm::finalize();
}

//=============================================================================
