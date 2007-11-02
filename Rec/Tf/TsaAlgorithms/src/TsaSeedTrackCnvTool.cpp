// $Id: TsaSeedTrackCnvTool.cpp,v 1.1 2007-11-02 14:57:58 albrecht Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/SystemOfUnits.h"

#include "Event/StateParameters.h"
// local
#include "TsaSeedTrackCnvTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TsaSeedTrackCnvTool
//
//   @author M.Needham
//   @date   30/04/2004
//
// 2007-10-31 : Johannes Albrecht - convert to tool
//-----------------------------------------------------------------------------

using namespace Tf::Tsa;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SeedTrackCnvTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SeedTrackCnvTool::SeedTrackCnvTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITsaSeedTrackCnvTool>(this);

  /*
    declareProperty("eX2",m_EX2 = 40*Gaudi::Units::mm2);
    declareProperty("eY2",m_EY2 = 40*Gaudi::Units::mm2);
    declareProperty("eTX2",m_ETx2 = 1e-4);
    declareProperty("eTY2",m_ETy2 = 1.0e-4); // 2e-7;
  */
  
  declareProperty("eX2",m_EX2 = 31*Gaudi::Units::mm2);
  declareProperty("eY2",m_EY2 = 12*Gaudi::Units::mm2);
  declareProperty("eTX2",m_ETx2 = 2.0e-3);
  declareProperty("eTY2",m_ETy2 = 2.0e-3); // 2e-7;
  declareProperty("increaseErrors", m_largeErrors = true);
  declareProperty("curvatureFactor", m_curvFactor = 42.0/Gaudi::Units::MeV);
  
  declareProperty("pFromPtKick", m_pFromPtKick = false);
  declareProperty("pFromCurvature", m_pFromCurvature = false);
  declareProperty("stateAtLastHit", m_stateAtLastHit = true);
  
}
//=============================================================================
// Destructor
//=============================================================================
SeedTrackCnvTool::~SeedTrackCnvTool() {} 

//=============================================================================

StatusCode SeedTrackCnvTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  debug() << " ==> Initialize " << endmsg;
  
  m_ptKickTool = tool<ITrackPtKick>("TrackPtKick");
  m_fastPTool = tool <IFastMomentumEstimate>("FastMomentumEstimate");
  
  return sc;
}


LHCb::Track* SeedTrackCnvTool::convert(const SeedTrack* aTrack) const
{
  // make a fit track...
  LHCb::Track* fitTrack = new LHCb::Track();


  std::vector<SeedPnt> pnts = aTrack->usedPnts();
  //std::cout << " starting a track " << pnts.size() << std::endl;
  for ( std::vector<SeedPnt>::const_iterator itP = pnts.begin(); pnts.end() != itP; ++itP ) {
    fitTrack->addToLhcbIDs((*itP).hit()->lhcbID());
    //std::cout << (*itP).hit()->ITChan() << std::endl;
  } //it

  double zRef = pnts.back().z();
  if( !m_stateAtLastHit ) zRef = StateParameters::ZMidT;
  
  addState(aTrack, fitTrack, zRef );

  // add history
  fitTrack->setHistory(LHCb::Track::TsaTrack);
  fitTrack->setType(LHCb::Track::Ttrack);
  fitTrack ->setPatRecStatus( LHCb::Track::PatRecIDs );
  fitTrack->addInfo(LHCb::Track::TsaLikelihood, aTrack->lik());

  return fitTrack;
}

void SeedTrackCnvTool::addState(const SeedTrack* aTrack, LHCb::Track* lTrack, const double z) const{

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
    if (m_pFromPtKick == false){
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

  // p estimate can come from ptKick, curvature or Fast momentum estimate tool. 
  if ( m_pFromPtKick ){
    StatusCode sc = m_ptKickTool->calculate(&aState);
    if( sc.isFailure() ) {
      Warning( "Pt Kick tool failed, but still adding State" );
    }
  }
  else if( m_pFromCurvature ){
    aState.setQOverP(estimateCurvature(aTrack->tx(), m_curvFactor));
    if (m_largeErrors) {
      aState.setErrQOverP2( m_EQdivP2*gsl_pow_2(stateVec(4)));
    }
    else {
      aState.setErrQOverP2(aTrack->xErr(5)*gsl_pow_2(m_curvFactor));
    }
  }
  else {
    //need to create a preliminary state to ensure that momentum estimate 
    //gets a state at mid T
    double zMidT =  StateParameters::ZMidT;
    LHCb::State aStateT = LHCb::State(stateVec,stateCov,zMidT,LHCb::State::AtT);
    // state all parameters but p
    aStateT.setX(aTrack->x(zMidT,0.));
    aStateT.setY(aTrack->y(zMidT,0.));
    aStateT.setTx(aTrack->xSlope(zMidT,0.));
    aStateT.setTy(aTrack->sy());

    double qOverP = 0;
    double sigmaQOverP = 0;
    StatusCode sc = m_fastPTool->calculate(&aStateT ,qOverP, sigmaQOverP , false );
    if( sc.isFailure() ) {
      Warning( "FastMomentumEstimate tool  failed, but still adding State" );
    }
    //momentum assigned is p at mid T
    aState.setQOverP( qOverP );
    aState.setErrQOverP2( sigmaQOverP );
  }
  
  // add to states
  lTrack->addToStates(aState);
  
}


