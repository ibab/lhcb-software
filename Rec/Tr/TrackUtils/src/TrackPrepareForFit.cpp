//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "gsl/gsl_math.h"

// track interfaces
#include "Event/State.h"

#include "TrackPrepareForFit.h"

using namespace LHCb;
using namespace Gaudi::Units;
using namespace Gaudi;

DECLARE_ALGORITHM_FACTORY( TrackPrepareForFit )

TrackPrepareForFit::TrackPrepareForFit(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  declareProperty( "inputLocation", m_inputLocation = TrackLocation::Velo);
  declareProperty( "ptVelo", m_ptVelo = 400.*MeV);
  declareProperty("fixP", m_fixP = true);
  declareProperty("curvValue", m_curvValue = 1.0/(7.0*GeV));
}

TrackPrepareForFit::~TrackPrepareForFit()
{
  // destructor
}

StatusCode TrackPrepareForFit::execute(){

  Tracks* inCont = get<Tracks>(m_inputLocation);
   
  // loop 
  unsigned int i = 0;
  int charge;
  Tracks::const_iterator iterT = inCont->begin();
  for (; iterT != inCont->end(); ++iterT, ++i){
    i % 2  == 0 ? charge = -1 : charge = 1;   
    prepare(*iterT, charge );
  } // iterT
   
  return StatusCode::SUCCESS;
}

void TrackPrepareForFit::prepare(Track* aTrack, const int charge) const{

 // do what we have to do...
 State& vState = aTrack->firstState();
 TrackVector& vec = vState.stateVector();
 double slope2 = GSL_MAX(vec(2)*vec(2) + vec(3)*vec(3), 1e-20);
 double curv  = m_curvValue;
 if ( !m_fixP) curv = charge * sqrt( slope2 ) / (m_ptVelo * sqrt( 1. + slope2 ));

 // set all the state a track has....
 const std::vector<State*>& tStates = aTrack->states();
 for (std::vector<State*>::const_iterator iterState = tStates.begin(); 
      iterState != tStates.end(); ++iterState){
   State* aState= const_cast<State*>(*iterState);
   aState->setQOverP(curv);
   aState->setErrQOverP2(1e-3);
 }
}
