//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// track interfaces
#include "Event/State.h"

#include "TrackPrepareVelo.h"

using namespace LHCb;
using namespace Gaudi::Units;
using namespace Gaudi;

DECLARE_ALGORITHM_FACTORY( TrackPrepareVelo )

TrackPrepareVelo::TrackPrepareVelo(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
  , m_ignoreBest(false)
{
  // constructor
  declareProperty( "inputLocation", m_inputLocation = TrackLocation::Velo);
  declareProperty( "outputLocation", m_outputLocation = "/Event/Rec/Track/PreparedVelo");
  declareProperty( "bestLocation", m_bestLocation = TrackLocation::Default);
  declareProperty( "ptVelo", m_ptVelo = 400.*MeV);
  declareProperty( "reverseCharge", m_reverseCharge = false );

}

TrackPrepareVelo::~TrackPrepareVelo()
{
  // destructor
}

StatusCode TrackPrepareVelo::initialize(){
  m_ignoreBest = ( m_bestLocation == "" ) ? true : false;
  return StatusCode::SUCCESS;
}

StatusCode TrackPrepareVelo::execute(){

  Tracks* inCont = get<Tracks>(m_inputLocation);
  Tracks* bestCont = 0 ;
  if ( !m_ignoreBest ) bestCont = get<Tracks>(m_bestLocation);
  Tracks* outCont = new Tracks();

  // loop
  unsigned int i = 0;
  Tracks::const_iterator iterT = inCont->begin();
  for (; iterT != inCont->end(); ++iterT, ++i){
    bool takeTrack = false;
    if (m_ignoreBest == false) takeTrack = used(*iterT,bestCont);
    if ( takeTrack == false){
      Track* aTrack = (*iterT)->clone();
      int charge = 0;
      i % 2  == 0 ? charge = -1 : charge = 1;
      if( m_reverseCharge ) charge *= -1; // flip charge assignment if requested
      prepare(aTrack, charge );
      outCont->insert(aTrack);
    }
  } // iterT

  put(outCont,m_outputLocation);

  return StatusCode::SUCCESS;
}

bool TrackPrepareVelo::used(const Track* aTrack, const Tracks* bestCont) const
{
  // if the track has already been flagged as a clone then do not use it
  if ( aTrack -> checkFlag(Track::Clone) ) return true;
  // check if velo track is used.
  bool found = false;
  Tracks::const_iterator iterT =  bestCont->begin();
  while ((iterT != bestCont->end())&&(found == false)){
    const SmartRefVector<LHCb::Track>& parents = (*iterT)->ancestors();
    for ( SmartRefVector<LHCb::Track>::const_iterator iterP = parents.begin();
          iterP != parents.end(); ++iterP) {
      const Track* testTrack = *iterP;
      if ( testTrack == aTrack) found = true;
    } // iterP
    ++iterT;
  } // iterT

  return found;
}

void TrackPrepareVelo::prepare(Track* aTrack, const int charge) const{
  // set q/p and error in all of the existing states
  const std::vector< LHCb::State * > states = aTrack->states();
  std::vector< LHCb::State * >::const_iterator iState;
  for ( iState = states.begin() ; iState != states.end() ; ++iState ){
    TrackVector& vec = (*iState)->stateVector();
    double slope2 = std::max(vec(2)*vec(2) + vec(3)*vec(3), 1e-20);
    double curv = charge * sqrt( slope2 ) / (m_ptVelo * sqrt( 1. + slope2 ));
    (*iState)->setQOverP(curv);
    (*iState)->setErrQOverP2(1e-6);
  }
}
