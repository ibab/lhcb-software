// $Id: TrackBuildCloneTable.cpp,v 1.1 2007-08-27 14:52:33 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// track interfaces
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/TrackUnitsConverters.h"

// from Event/LinkerEvent
#include "Linker/LinkerWithKey.h"
 
#include "LHCbMath/MatrixUtils.h"

#include "TrackBuildCloneTable.h"

#include "TrackInterfaces/ITrackExtrapolator.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackBuildCloneTable );

TrackBuildCloneTable::TrackBuildCloneTable(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
 
 declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
 declareProperty("outputLocation", m_outputLocation = TrackLocation::Default+"Clones");
 declareProperty("klCut", m_klCut = 5000);
 declareProperty("zState", m_zState = 0.0);
 declareProperty("maxDz", m_maxDz = 2.0*Gaudi::Units::m);

}

TrackBuildCloneTable::~TrackBuildCloneTable()
{
  // destructor
}

StatusCode TrackBuildCloneTable::initialize() {
  
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }

  m_extrapolator = tool<ITrackExtrapolator>("TrackHerabExtrapolator");
  return StatusCode::SUCCESS;
}

StatusCode TrackBuildCloneTable::execute(){

  const Tracks* inCont = get<Tracks>(m_inputLocation);

  LinkerWithKey<Track,Track> myLink( eventSvc(), msgSvc(), m_outputLocation);

  // working objects ---> invert the covariance matrix only once...
  CloneTracks tracks; tracks.reserve(inCont->size());
  for (Tracks::const_iterator iterT = inCont->begin(); 
       iterT != inCont->end(); ++iterT){
    State tmpState = (*iterT)->closestState(m_zState);
    
    // only take ones that are close in z
    if (fabs(tmpState.z() - m_zState) > m_maxDz ) continue ;

    //extrapolate
    StatusCode sc = m_extrapolator->propagate(tmpState, m_zState);
    if (sc.isFailure() ) continue;

    // build object
    CloneTrack aTrack; aTrack.track = *iterT;
    aTrack.state = tmpState;
    Gaudi::TrackSymMatrix G1 = aTrack.state.covariance();
    invert(G1);
    aTrack.invCov = G1;

    tracks.push_back(aTrack);
  } // inCont
 
  // loop over the working objects 
  for (CloneTracks::const_iterator iterC1 = tracks.begin(); 
       iterC1 != tracks.end(); ++iterC1){
    for (CloneTracks::const_iterator iterC2 = tracks.begin(); 
      iterC2 != tracks.end(); ++iterC2){
      if (iterC1->track != iterC2->track) {
        double dist = kullbeckLieblerDist(*iterC1,*iterC2);
        if (dist < m_klCut){
          myLink.link(iterC1->track,iterC2->track, dist );
	}
      }
    } // iterC2
  } // iterC1

  return StatusCode::SUCCESS;
};

double TrackBuildCloneTable::kullbeckLieblerDist(const CloneTrack& track1,
                                                 const CloneTrack& track2) const{

  const Gaudi::TrackVector diffVec = track1.state.stateVector()  
                               - track2.state.stateVector(); 
  const Gaudi::TrackSymMatrix diffCov = track1.state.covariance() 
                               - track2.state.covariance();

  const Gaudi::TrackSymMatrix invDiff = track2.invCov - track1.invCov;
  const Gaudi::TrackSymMatrix invSum = track2.invCov + track1.invCov;
  const Gaudi::TrackMatrix diff = diffCov * invDiff;
  
  // trace
  return Gaudi::Math::trace(diff) + Similarity(invSum,diffVec);
}

bool TrackBuildCloneTable::invert(Gaudi::TrackSymMatrix& m) const{

  TrackUnitsConverters::convertToG3( m );
  bool OK = m.Invert();
  TrackUnitsConverters::convertToG4( m );
  return OK;
} 
