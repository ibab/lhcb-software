// $Id: HltTrackFromTes.cpp,v 1.4 2007-11-20 10:26:27 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
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
  LHCb::Tracks* tracks;
  bool ok = retrieve(tracks,m_tracksLocation);
  if (!ok) return stop("no tracks in TES");

  m_outputTracks->clear();  
  for (Tracks::iterator it = tracks->begin(); 
       it != tracks->end(); it++) {
    if ( (*it)->checkType(Track::Long)) m_outputTracks->push_back(*it);
  }
  if (m_orderByPt)
    std::sort(m_outputTracks->begin(),m_outputTracks->end(),
              Hlt::SortTrackByPt());
  debug() << " transfered tracks " << m_outputTracks->size() 
          << " from TES " << tracks->size() << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackFromTes::finalize() {
  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();
}

//=============================================================================
