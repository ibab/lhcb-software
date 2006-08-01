// $Id: TsaToTrackCnv.cpp,v 1.2 2006-08-01 09:10:38 cattanem Exp $
//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Tsa...
#include "TsaKernel/Track.h"
#include "TsaKernel/TsaConstants.h"
#include "TsaToTrackCnv.h"

//  Event
#include "Event/State.h"
#include "Event/Track.h"

#include "TsaKernel/STCluster.h"

//gsl
#include "gsl/gsl_math.h"

DECLARE_ALGORITHM_FACTORY( TsaToTrackCnv );


TsaToTrackCnv::TsaToTrackCnv(const std::string& name,
                       ISvcLocator* pSvcLocator):
 GaudiAlgorithm(name, pSvcLocator) 
{
  // constructor
  declareProperty("eX2",m_EX2 = 40*Gaudi::Units::mm2);
  declareProperty("eY2",m_EY2 = 40*Gaudi::Units::mm2); 
  declareProperty("eTX2",m_ETx2 = 1e-4);
  declareProperty("eTY2",m_ETy2 = 1.0e-4); // 2e-7;
  declareProperty("eQdivP2",m_EQdivP2 = 0.01);

}

TsaToTrackCnv::~TsaToTrackCnv()
{
  // destructor
}


StatusCode TsaToTrackCnv::execute(){
 
  // get the tracks
  TsaTracks* trackCont = get<TsaTracks>(Tsa::TrackLocation::Default);

  // output container
  LHCb::Tracks* output = new LHCb::Tracks();
  output->reserve(trackCont->size()); 

  for (TsaTracks::const_iterator iterTrack = trackCont->begin(); 
       iterTrack != trackCont->end(); ++iterTrack) {
    LHCb::Track* fitTrack = this->convert(*iterTrack);
    output->insert(fitTrack);
  }
 

  put(output,LHCb::TrackLocation::Tsa);

  return StatusCode::SUCCESS;
}


LHCb::Track* TsaToTrackCnv::convert(const Tsa::Track* aTrack) const{

  // make a fit track...
  LHCb::Track* fitTrack = new LHCb::Track();

  addState(aTrack, fitTrack);

  // add history
  fitTrack->setHistory(LHCb::Track::TsaTrack);
  fitTrack->setType(LHCb::Track::Ttrack);

  // add measurements 
  std::vector<Tsa::Cluster*>::const_iterator iterC = aTrack->clusters().begin();  
  for (;iterC != aTrack->clusters().end(); ++iterC){
    fitTrack->addToLhcbIDs((*iterC)->id());
  } // iterC

  return fitTrack;
}

void TsaToTrackCnv::addState(const Tsa::Track* aTrack, LHCb::Track* lTrack) const{

  // first  state info
  Gaudi::TrackVector stateVec = aTrack->stateVector();
 
  // cov matrix 
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  stateCov(0,0) = m_EX2;
  stateCov(1,1) = m_EY2;
  stateCov(2,2) = m_ETx2;
  stateCov(3,3) = m_ETy2;
  stateCov(4,4) = m_EQdivP2*gsl_pow_2(stateVec(4));

  LHCb::State aState = LHCb::State(stateVec,stateCov,aTrack->z(),LHCb::State::AtT); 
  lTrack->addToStates(aState);
}


