// $Id: TsaSeedTrackCnv.cpp,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "GaudiKernel/AlgFactory.h"

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
#include "GaudiKernel/PhysicalConstants.h"

// Needed for the creation of TsaSeedTrackCnv objects.
static const AlgFactory<TsaSeedTrackCnv> s_factory;
const IAlgFactory& TsaSeedTrackCnvFactory = s_factory;

TsaSeedTrackCnv::TsaSeedTrackCnv(const std::string& name,
                       ISvcLocator* pSvcLocator):
 TsaBaseAlg(name, pSvcLocator) 
{
  // constructor
  declareProperty("eX2",m_EX2 = 40*Gaudi::Units::mm2);
  declareProperty("eY2",m_EY2 = 40*Gaudi::Units::mm2); 
  declareProperty("eTX2",m_ETx2 = 1e-4);
  declareProperty("eTY2",m_ETy2 = 1.0e-4); // 2e-7;
  declareProperty("eQdivP2",m_EQdivP2 = 0.01);

  declareProperty("LikCut", m_likCut = -999.);
  declareProperty("curvatureFactor", m_curvFactor = 37.0);
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
      output->insert(fitTrack);
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
  return fitTrack;
}

void TsaSeedTrackCnv::addState(const SeedTrack* aTrack, LHCb::Track* lTrack, const double z) const{

  // first  state info
  Gaudi::TrackVector stateVec = Gaudi::TrackVector();
  stateVec(0) = aTrack->x(z,0.);
  stateVec(1) = aTrack->y(z,0.);
  stateVec(2) = aTrack->xSlope(z,0.);
  stateVec(3) = aTrack->sy();
  stateVec(4) = Tsa::estimateCurvature(aTrack->tx(), m_curvFactor);

  // cov matrix 
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
  stateCov(0,0) = m_EX2;
  stateCov(1,1) = m_EY2;
  stateCov(2,2) = m_ETx2;
  stateCov(3,3) = m_ETy2;
  stateCov(4,4) = m_EQdivP2*gsl_pow_2(stateVec(4));

  LHCb::State aState = LHCb::State(stateVec,stateCov,z,LHCb::State::AtT); 
  lTrack->addToStates(aState);
}


