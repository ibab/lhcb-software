// $Id: TrackToDST.cpp,v 1.1 2006-06-14 15:03:47 mneedham Exp $
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
 this->declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
 this->declareProperty("allStates", m_allStates = false);
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
 
    if (m_allStates == false){
      cleanStates(*iterT);
    }

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

} 

