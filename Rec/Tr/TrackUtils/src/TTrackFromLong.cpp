//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// track interfaces
#include "Event/Track.h"

#include "TTrackFromLong.h"
#include "GaudiKernel/SystemOfUnits.h"
using namespace LHCb;
using namespace Gaudi::Units;

DECLARE_ALGORITHM_FACTORY( TTrackFromLong )

TTrackFromLong::TTrackFromLong(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
 this->declareProperty("inputLocation", m_inputLocation = TrackLocation::Forward);
 this->declareProperty("outputLocation", m_outputLocation = TrackLocation::Seed);
}

TTrackFromLong::~TTrackFromLong()
{
  // destructor
}


StatusCode TTrackFromLong::execute(){

  Tracks* trackCont = get<Tracks>(m_inputLocation);
  Tracks* newCont = new Tracks();
  newCont->reserve(trackCont->size());

  // loop 
  for (Tracks::const_iterator iterT = trackCont->begin(); iterT != trackCont->end(); ++iterT){
    Track* aTrack = convert(*iterT);
    if (aTrack->nLHCbIDs()>4){ newCont->insert(aTrack);} else {delete aTrack;} 
  } // iterT
   

  put(newCont,m_outputLocation);

  return StatusCode::SUCCESS;
}

Track* TTrackFromLong::convert(const Track* aTrack) const{
  
  Track* tSeed = new Track();
  tSeed->setType( LHCb::Track::Ttrack );
  tSeed->setHistory( LHCb::Track::PatSeeding);

  const State& lastState =  aTrack->closestState(9000.*mm);

  LHCb::State tState;
  tState.setLocation( LHCb::State::AtT );
  tState.setState(lastState.stateVector());
  tState.setZ(lastState.z());
  
  tState.setCovariance(lastState.covariance());
  tSeed->addToStates(tState);
  tSeed->setPatRecStatus(Track::PatRecIDs);
  

  const std::vector<LHCb::LHCbID>& ids =  aTrack->lhcbIDs();

  for (std::vector<LHCb::LHCbID>::const_iterator iter = ids.begin(); iter != ids.end(); ++iter){
    //  std::cout<<"TTrackFromLong -- in the lhcbids loop...."<<std::endl;
    
    if ((iter->isOT() == true)|| (iter->isIT() == true)) 

      tSeed->addToLhcbIDs(*iter);
  } // iter

  return tSeed;
}
