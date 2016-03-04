//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// track interfaces
#include "TrackInterfaces/ITrackSelector.h"
#include "Event/Track.h"

#include "TrackContainerCleaner.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackContainerCleaner )

TrackContainerCleaner::TrackContainerCleaner(const std::string& name,
                                             ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
  , m_selector(nullptr)
{
  // constructor
 declareProperty("selectorName", m_selectorName = "TrackSelector");
 declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
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
  m_selector = tool<ITrackSelector>(m_selectorName, "Selector", this);

  return StatusCode::SUCCESS;
}

StatusCode TrackContainerCleaner::execute(){

  Tracks* trackCont = getIfExists<Tracks>(m_inputLocation);
  if( trackCont == nullptr){
    Warning( "Input container "+m_inputLocation+" does not exist", StatusCode::SUCCESS, 20 );
    return StatusCode::SUCCESS;
  }
    
  std::vector<Track*> tVec;
  
  // loop and select bad tracks
  for (Tracks::const_iterator iterT = trackCont->begin(); iterT != trackCont->end(); ++iterT){
    Track& aTrack = *(*iterT);
    if (m_selector->accept(aTrack) == false) tVec.push_back(*iterT); 
  } // iterT
   
  // remove from the container and delete the bad tracks
  std::size_t nRemoved = 0;
  std::vector<Track*>::reverse_iterator iterVec = tVec.rbegin(); 
  for (; iterVec != tVec.rend(); ++iterVec){
    trackCont->erase(*iterVec);
    ++nRemoved;
  } // iterVec

  counter("# input tracks") += trackCont->size();
  counter("# removed tracks") += nRemoved;
  
  return StatusCode::SUCCESS;
}
