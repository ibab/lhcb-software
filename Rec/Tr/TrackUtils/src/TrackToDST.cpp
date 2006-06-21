// $Id: TrackToDST.cpp,v 1.3 2006-06-21 06:35:41 mneedham Exp $
//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// track interfaces
#include "Event/Track.h"

#include "TrackToDST.h"
using namespace LHCb;

// Needed for the creation of TrackToDST objects.
static const AlgFactory<TrackToDST> s_factory;
const IAlgFactory& TrackToDSTFactory = s_factory;

TrackToDST::TrackToDST(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  this->declareProperty( "TracksInContainer",
                         m_inputLocation = TrackLocation::Default );
  this->declareProperty( "StoreAllStates", m_storeAllStates = false );
}

TrackToDST::~TrackToDST()
{
  // destructor
}


StatusCode TrackToDST::execute(){

  Tracks* inCont = get<Tracks>(m_inputLocation);

  // loop 
  for (Tracks::iterator iterT = inCont->begin(); 
       iterT != inCont->end(); ++iterT){
 
    // remove the necessary States on the Track
    if (m_storeAllStates == false){
      cleanStates(*iterT);
    }

  // remove all the Measurements on the Track
  const std::vector<Measurement*> allmeas = (*iterT) -> measurements();
  for ( std::vector<Measurement*>::const_iterator it = allmeas.begin();
        it != allmeas.end(); ++it)
    (*iterT) -> removeFromMeasurements( *it );
  // set the appropriate flag!
  (*iterT) -> setPatRecStatus( Track::PatRecIDs );

  } // iterT
   
  return StatusCode::SUCCESS;
};

void TrackToDST::cleanStates(Track* aTrack) const{

  // find the first state on the track
  State& fState = aTrack->firstState();
  State* tState = fState.clone();

  // clear the vector and add the new states...
  aTrack->clearStates();
  aTrack->addToStates(*tState);

  // this line is necessary because the Track event 
  // model breaks all Gaudi conventions on memory 
  // garbage collection for a garbage model
  delete tState; 

} 

