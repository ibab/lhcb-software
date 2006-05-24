// $Id: TrackContainerCleaner.cpp,v 1.1 2006-05-24 15:04:38 mneedham Exp $
//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "TrackInterfaces/ITrackSelector.h"
#include "Event/Track.h"

#include "TrackContainerCleaner.h"

using namespace LHCb;

// Needed for the creation of TrackContainerCleaner objects.
static const AlgFactory<TrackContainerCleaner> s_factory;
const IAlgFactory& TrackContainerCleanerFactory = s_factory;

TrackContainerCleaner::TrackContainerCleaner(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
 this->declareProperty("selectorName", m_selectorName = "TrackSelector");
 this->declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
}

TrackContainerCleaner::~TrackContainerCleaner()
{
  // destructor
}

StatusCode TrackContainerCleaner::initialize()
{
  // Initializes TsaInitialization at the begin of program execution.

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }

  // da selector
  m_selector = tool<ITrackSelector>(m_selectorName);

  return StatusCode::SUCCESS;
}

StatusCode TrackContainerCleaner::execute(){

  Tracks* trackCont = get<Tracks>(m_inputLocation);

  std::vector<Track*> tVec;
  
  // loop and select bad tracks
  for (Tracks::const_iterator iterT = trackCont->begin(); iterT != trackCont->end(); ++iterT){
    Track& aTrack = *(*iterT);
    if (m_selector->accept(aTrack) == false) tVec.push_back(*iterT); 
  } // iterT
   
  // remove from the container and delete the bad tracks
  std::vector<Track*>::reverse_iterator iterVec = tVec.rbegin(); 
  for (; iterVec != tVec.rend(); ++iterVec){
    trackCont->erase(*iterVec);
  } // iterVec

  return StatusCode::SUCCESS;
};
