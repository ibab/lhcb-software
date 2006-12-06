// $Id: TsaSeedTrackCnv.cpp,v 1.8 2006-12-06 14:35:01 mneedham Exp $
//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Tsa...
#include "TsaKernel/SeedTrack.h"
#include "TsaKernel/SeedPnt.h"

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
  /*
  declareProperty("eX2",m_EX2 = 40*Gaudi::Units::mm2);
  declareProperty("eY2",m_EY2 = 40*Gaudi::Units::mm2); 
  declareProperty("eTX2",m_ETx2 = 1e-4);
  declareProperty("eTY2",m_ETy2 = 1.0e-4); // 2e-7;
  declareProperty("eQdivP2",m_EQdivP2 = 0.1);
  */

  declareProperty("eX2",m_EX2 = 31*Gaudi::Units::mm2);
  declareProperty("eY2",m_EY2 = 12*Gaudi::Units::mm2); 
  declareProperty("eTX2",m_ETx2 = 2.0e-3);
  declareProperty("eTY2",m_ETy2 = 2.0e-3); // 2e-7;
  declareProperty("eQdivP2",m_EQdivP2 = 0.01);

  declareProperty("LikCut", m_likCut = -30.);
  declareProperty("curvatureFactor", m_curvFactor = 42.0/Gaudi::Units::MeV);
  declareProperty("pFromCurvature", m_pFromCurvature = false);
  declareProperty("increaseErrors", m_largeErrors = true);

  declareProperty("inputLocation", m_inputLocation = SeedTrackLocation::Default);
  declareProperty("outputLocation", m_outputLocation = LHCb::TrackLocation::Tsa );
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
  SeedTracks* trackCont = get<SeedTracks>(m_inputLocation);

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
 

  put(output,m_outputLocation);

  return StatusCode::SUCCESS;
}


LHCb::Track* TsaSeedTrackCnv::convert(const SeedTrack* aTrack) const{

  // make a fit track...
  LHCb::Track* fitTrack = new LHCb::Track();


  std::vector<SeedPnt> pnts = aTrack->usedPnts();
  //std::cout << " starting a track " << pnts.size() << std::endl; 
  for ( std::vector<SeedPnt>::const_iterator itP = pnts.begin(); pnts.end() != itP; ++itP ) {
    fitTrack->addToLhcbIDs((*itP).hit()->clus()->id());
    //std::cout << (*itP).hit()->ITChan() << std::endl;
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

  // initial errors - either large or from seed
  if (m_largeErrors == true){
    stateCov(0,0) = m_EX2;
    stateCov(1,1) = m_EY2;
    stateCov(2,2) = m_ETx2;
    stateCov(3,3) = m_ETy2;
  }
  else {
    // x
    stateCov(0,0) = aTrack->xErr(0);
    stateCov(2,2) = aTrack->xErr(3); 
    stateCov(0,1) = aTrack->xErr(1);
    if (m_pFromCurvature == true){
      stateCov(0,4) = aTrack->xErr(2)*m_curvFactor;
      stateCov(2,4) = aTrack->xErr(4)*m_curvFactor;
    }
    // y  
    stateCov(1,1) = aTrack->yErr(0);
    stateCov(3,3) = aTrack->yErr(2); 
    stateCov(1,3) = aTrack->yErr(1);
  }


  LHCb::State aState = LHCb::State(stateVec,stateCov,z,LHCb::State::AtT); 

  // state all parameters but p
  aState.setX(aTrack->x(z,0.));
  aState.setY(aTrack->y(z,0.));
  aState.setTx(aTrack->xSlope(z,0.));
  aState.setTy(aTrack->sy());
 
  // p estimate can come from the curvature or the pt kick
  if (m_pFromCurvature == true) {
    aState.setQOverP(Tsa::estimateCurvature(aTrack->tx(), m_curvFactor));
    if (m_largeErrors == true) {
      aState.setErrQOverP2( m_EQdivP2*gsl_pow_2(stateVec(4)));
    }
    else {
      aState.setErrQOverP2(aTrack->xErr(5)*gsl_pow_2(m_curvFactor));
    }
  }
  else {
    m_ptKickTool->calculate(&aState);
  }

  // add to states
  lTrack->addToStates(aState);

}


