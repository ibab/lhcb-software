// $Id: TrackCloneCleaner.cpp,v 1.1 2007-08-27 14:52:33 mneedham Exp $
//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces

#include "Event/Track.h"
// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include "TrackCloneCleaner.h"

// from Event/LinkerEvent
#include "Linker/LinkedFrom.h"

using namespace LHCb;
using namespace boost::lambda;

DECLARE_ALGORITHM_FACTORY( TrackCloneCleaner );

TrackCloneCleaner::TrackCloneCleaner(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor

 declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
 declareProperty("outputLocation", m_outputLocation = TrackLocation::Default+"2");

 // linker location
 m_linkerLocation = m_inputLocation+"Clones";
 if ( "/Event/" == m_inputLocation.substr(0,7) ) {
   m_linkerLocation = "Link/" + m_inputLocation.substr(7);
 }
 declareProperty("linkerLocation", m_linkerLocation);
}

TrackCloneCleaner::~TrackCloneCleaner()
{
  // destructor
}

StatusCode TrackCloneCleaner::execute(){

  Tracks* trackCont = get<Tracks>(m_inputLocation);
  Tracks* output = new Tracks(); output->reserve(trackCont->size());  
  put(output,m_outputLocation);

  // copy the tracks into a temporary vector
  WorkingTracks tempTracks;
  
   // loop and make working tracks
  for (Tracks::const_iterator iterT = trackCont->begin(); 
       iterT != trackCont->end(); ++iterT){
    WorkingTrack tmp; 
    tmp.track = *iterT ;
    tmp.clone = false;
    tempTracks.push_back(tmp); 
  } // iterT
   
  // sort
  std::stable_sort( tempTracks.begin(),tempTracks.end(), bind(&WorkingTrack::chi2,_1)  < 
                                                         bind(&WorkingTrack::chi2,_2) );
  std::stable_sort( tempTracks.begin(),tempTracks.end(), bind(&WorkingTrack::nMeas,_1)  > 
                                                         bind(&WorkingTrack::nMeas,_2));

  LinkedFrom<LHCb::Track,LHCb::Track> linker(evtSvc(), msgSvc(),m_linkerLocation);
  if (linker.notFound() == true){
    return Warning("Linker not found !");
  }

  std::vector<Track*> tVec;
  for (WorkingTracks::iterator iterW = tempTracks.begin(); 
       iterW != tempTracks.end(); ++iterW){

    if (iterW->clone == true) continue;
    
    // make a new track
    Track* newTrack = iterW->track->clone();
    output->insert(newTrack);

    // pick up the clones
    Track* cloneTrack = linker.first( iterW->track );
    while (cloneTrack != 0){
      //  tVec.push_back(cloneTrack);
      tagClone(tempTracks,cloneTrack);      
      cloneTrack = linker.next(); 
    } // clone track 
  
  } // iterW


  /*
  tempTracks.clear();

  // remove from the container and delete the bad tracks
  std::vector<Track*>::reverse_iterator iterVec = tVec.rbegin(); 
  for (; iterVec != tVec.rend(); ++iterVec){
    std::cout << "erase " << std::endl;
    trackCont->erase(*iterVec);
  } // iterVec
  */
  return StatusCode::SUCCESS;
};

void TrackCloneCleaner::tagClone(WorkingTracks& tracks, 
                                 const Track* aTrack) const{
  WorkingTracks::iterator iter = std::find_if(tracks.begin(),tracks.end(),
						    bind(&WorkingTrack::sameTrack, _1, aTrack));  if (iter != tracks.end()) iter->clone = true;
}
