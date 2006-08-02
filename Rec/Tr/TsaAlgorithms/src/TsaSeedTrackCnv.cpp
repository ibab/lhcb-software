// $Id: TsaSeedTrackCnv.cpp,v 1.4 2006-08-02 14:47:38 mneedham Exp $
//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Tsa...
#include "SeedTrack.h"
#include "SeedPnt.h"

#include "TsaKernel/TsaConstants.h"
#include "TsaKernel/OTCluster.h"
#include "TsaKernel/TsaFun.h"

#include "TsaSeedTrackCnv.h"


//  Event
#include "Event/State.h"
#include "Event/Track.h"


//gsl
#include "gsl/gsl_math.h"


#include "TrackInterfaces/ITrackPtKick.h"

DECLARE_ALGORITHM_FACTORY( TsaSeedTrackCnv );


TsaSeedTrackCnv::TsaSeedTrackCnv(const std::string& name,
                       ISvcLocator* pSvcLocator):
 TsaBaseAlg(name, pSvcLocator) 
{
  // constructor
  declareProperty("eX2",m_EX2 = 40*Gaudi::Units::mm2);
  declareProperty("eY2",m_EY2 = 40*Gaudi::Units::mm2); 
  declareProperty("eTX2",m_ETx2 = 1e-4);
  declareProperty("eTY2",m_ETy2 = 1.0e-4); // 2e-7;
  declareProperty("eQdivP2",m_EQdivP2 = 0.1);

  declareProperty("LikCut", m_likCut = -25.);
  declareProperty("curvatureFactor", m_curvFactor = 42.0/Gaudi::Units::MeV);
  declareProperty("pFromCurvature", m_pFromCurvature = false);
}


StatusCode TsaSeedTrackCnv::initialize()
{
  // Initialization
  
  StatusCode sc = TsaBaseAlg::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }
   
  m_ptKickTool = tool<ITrackPtKick>("TrackPtKick");
  
  return StatusCode::SUCCESS;
}


TsaSeedTrackCnv::~TsaSeedTrackCnv()
{
  // destructor
}


StatusCode TsaSeedTrackCnv::execute(){
 
  // get the tracks
  SeedTracks* trackCont = get<SeedTracks>(SeedTrackLocation::Default);

  // output container
  LHCb::Tracks* output = new LHCb::Tracks();
  output->reserve(trackCont->size()); 
  for (SeedTracks::const_iterator iterTrack = trackCont->begin(); 
       iterTrack != trackCont->end(); ++iterTrack) {

    if (((*iterTrack)->select() != 0)&&((*iterTrack)->lik() > m_likCut)){
      LHCb::Track* fitTrack = this->convert(*iterTrack);
      output->insert(fitTrack, (*iterTrack)->key());
    }
  }
 

  put(output,LHCb::TrackLocation::Tsa);

  return StatusCode::SUCCESS;
}


LHCb::Track* TsaSeedTrackCnv::convert(const SeedTrack* aTrack) const{

  // make a fit track...
  LHCb::Track* fitTrack = new LHCb::Track();

  std::vector<SeedPnt> pnts = aTrack->pnts();
  for ( std::vector<SeedPnt>::const_iterator itP = pnts.begin(); pnts.end() != itP; ++itP ) {
    fitTrack->addToLhcbIDs((*itP).hit()->clus()->id());
  } //it

  addState(aTrack, fitTrack, pnts.back().z());

  // add history
  fitTrack->setHistory(LHCb::Track::TsaTrack);
  fitTrack->setType(LHCb::Track::Ttrack);
  fitTrack ->setPatRecStatus( LHCb::Track::PatRecIDs );
  fitTrack->addInfo(LHCb::Track::Likelihood, aTrack->lik());

  return fitTrack;
}

void TsaSeedTrackCnv::addState(const SeedTrack* aTrack, LHCb::Track* lTrack, const double z) const{

  // first  state info
  Gaudi::TrackVector stateVec = Gaudi::TrackVector();
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();

  // initial errors
  stateCov(0,0) = m_EX2;
  stateCov(1,1) = m_EY2;
  stateCov(2,2) = m_ETx2;
  stateCov(3,3) = m_ETy2;


  LHCb::State aState = LHCb::State(stateVec,stateCov,z,LHCb::State::AtT); 

  // state all parameters but p
  aState.setX(aTrack->x(z,0.));
  aState.setY(aTrack->y(z,0.));
  aState.setTx(aTrack->xSlope(z,0.));
  aState.setTy(aTrack->sy());
 
  // p can come from the curvature or the pt kick
  if (m_pFromCurvature == true) {
    aState.setQOverP(Tsa::estimateCurvature(aTrack->tx(), m_curvFactor));
    aState.setErrQOverP2( m_EQdivP2*gsl_pow_2(stateVec(4)));
  }
  else {
    m_ptKickTool->calculate(&aState);
  }

  // add to states
  lTrack->addToStates(aState);

}


