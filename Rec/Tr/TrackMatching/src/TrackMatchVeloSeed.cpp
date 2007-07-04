// $Id: TrackMatchVeloSeed.cpp,v 1.33 2007-07-04 09:37:54 mneedham Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from GSL
#include "gsl/gsl_math.h"

// from TrackEvent
#include "Event/State.h"
#include "Event/StateParameters.h"

// from DigiEvent
#include "Event/STCluster.h"
#include "TsaKernel/Line.h"

// local
#include "TrackMatchVeloSeed.h"
#include "TrackMatchingDataFunctor.h"
#include "AddTTClusterTool.h"
#include "VeloCandidate.h"

#include "gsl/gsl_cdf.h"

#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>


//-----------------------------------------------------------------------------
// Implementation file for class : TrackMatchVeloSeed
//
// 2001-05-16 : J. van Tilburg Jeroen.van.Tilburg@cern.ch
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackMatchVeloSeed );

using namespace Gaudi;
using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMatchVeloSeed::TrackMatchVeloSeed( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_extrapolatorVelo(0)
  , m_extrapolatorSeed(0)
  , m_chi2Calculator(0)
  , m_measProvider(0)
{
  declareProperty( "InputVeloTracks",  m_veloTracks = TrackLocation::Velo );
  declareProperty( "InputSeedTracks",  m_seedTracks = TrackLocation::Tsa  );

  declareProperty( "Chi2MatchingCut",  m_chi2MatchingCut =1000.0 );
  declareProperty( "AllCombinations",  m_allCombinations = false );
  declareProperty( "MomentumCut",      m_momentumCut = 1000.0 );
  declareProperty( "pt2Cut",      m_pt2Cut = 80.0*80.0 );
  declareProperty( "VeloXCut2",         m_veloXCut2 = gsl_pow_2(0.5) );
  declareProperty( "VeloYCut2",         m_veloYCut2 = gsl_pow_2(0.5) );
  declareProperty( "VeloTxCut2",        m_veloTxCut2 = gsl_pow_2(8.0e-4) );
  declareProperty( "VeloTyCut2",        m_veloTyCut2 = gsl_pow_2(8.0e-4) );
  declareProperty( "SeedXCut2",         m_seedXCut2 = gsl_pow_2(150.0) );
  declareProperty( "SeedYCut2",         m_seedYCut2 = gsl_pow_2(30.0) );
  declareProperty( "SeedTxCut2",        m_seedTxCut2 = gsl_pow_2(0.1) );
  declareProperty( "SeedTyCut2",        m_seedTyCut2 = gsl_pow_2(0.01) );
  declareProperty( "MatchAtZPosition", m_matchAtZPosition = 800.0 );
  declareProperty( "VariableZ",        m_variableZ = false );
  declareProperty( "VarZParameters",   m_varZParameters );
  declareProperty( "AddTTClusters",    m_addTTClusters = true );
  declareProperty( "AddMeasurements",  m_addMeasurements = false );
  declareProperty("Chi2SeedCut", m_chi2SeedCut = 25.);
  declareProperty("LikCut", m_likCut = -30.0);
  declareProperty("referenceT", m_refT = 7500.);
  declareProperty("maxStepSize" , m_maxStepSize = 1500.);
  declareProperty("chi2TCut", m_chi2TCut = 200);
  declareProperty("yTParams", m_yTParams = boost::assign::list_of(gsl_pow_2(1.6))
                                                         (gsl_pow_2(6.9e4)));
  declareProperty("tyTParams", m_tyTParams = boost::assign::list_of(gsl_pow_2(1.6e-3))
                                                           (gsl_pow_2(20)));

  declareProperty( "ExtrapolatorVelo",
                   m_extrapolatorVeloName = "TrackLinearExtrapolator" );
  declareProperty( "ExtrapolatorSeed",
                   m_extrapolatorSeedName = "TrackHerabExtrapolator" );
  
  declareProperty("TTClusterToolName", m_ttClusterToolName = "PatAddTTCoord" );

  declareProperty( "outputContainer",  m_outputLocation = TrackLocation::Match );

  declareProperty("nUseT", m_nUseT = 0u);
  declareProperty("nUseVelo", m_nUseVelo = 0u);

}
//=============================================================================
// Destructor
//=============================================================================
TrackMatchVeloSeed::~TrackMatchVeloSeed() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackMatchVeloSeed::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // Access the extrapolators tools
  m_extrapolatorVelo = tool<ITrackExtrapolator>( m_extrapolatorVeloName );
  m_extrapolatorSeed = tool<ITrackExtrapolator>( m_extrapolatorSeedName );
 
  // Access the chi2 calculator tool
  m_chi2Calculator = tool<ITrackChi2Calculator>( "TrackChi2Calculator", this );

  // Tool to add TT clusters to a track
  m_addTTClusterTool = tool<IAddTTClusterTool>( m_ttClusterToolName, "TTClusterTool",  this );

  // Access the measurement provider tool
  m_measProvider = tool<IMeasurementProvider>( "MeasurementProvider" );

  m_usedVelo.clear();
  m_usedT.clear();

  if (m_tyTParams.empty() || m_yTParams.empty()){
    return Error("Failed to initialize: Empty parameter vector !", StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackMatchVeloSeed::execute()
{
  // Retrieve velo and seed tracks from EvDS
  Tracks* veloTracks = get<Tracks>( m_veloTracks );
  Tracks* seedTracks = get<Tracks>( m_seedTracks );

  // make a vector of selected velo tracks
  VeloCandidates selectedVelo; selectedVelo.reserve(veloTracks->size());
  createVeloCandidates(veloTracks,selectedVelo);  

  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "retrieved succesfully " << veloTracks -> size()
            << " velo tracks selected " << selectedVelo.size() << endmsg;
    debug() << "retrieved succesfully " << seedTracks -> size()
            << " seed tracks" << endmsg;
  }
  
  // create and register the container for the matched tracks
  Tracks* matches = new Tracks();
  matches->reserve(veloTracks->size());
  put( matches, m_outputLocation);
  
  // the actual matching of the tracks
  StatusCode sc = matchTracks( selectedVelo, seedTracks, matches );
  if ( sc.isFailure() )
    return Error( "Unable to match velo tracks with Seed tracks" );

  // Store the new tracks
  sc = storeTracks( matches );
  if ( sc.isFailure() ) return Error( "Unable to store new tracks." );

  // clean up the Velo candidates 
  for (VeloCandidates::iterator iterVelo = selectedVelo.begin(); iterVelo != selectedVelo.end(); ++iterVelo){
    delete *iterVelo;  
  } // iterVelo

  // clean up the maps
  m_usedVelo.clear();
  m_usedT.clear();

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackMatchVeloSeed::finalize()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
// Match velo tracks with seed tracks
//=============================================================================
StatusCode TrackMatchVeloSeed::matchTracks( VeloCandidates& veloTracks,
                                            Tracks* seedTracks,
                                            Tracks* matches )
{
  // Match the two trackcontainers at the given z-position according
  // to the lowest chi2-distance and at most chi2=m_chi2MatchingCut.
  // Returns a vector of pointers to these tracks.

  StatusCode sc;
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Trying to match velo and seed tracks." << endmsg;
  
  TrackMatchVector matchVector;         // temporary list
  matchVector.reserve(2000);
  VeloCandidates::iterator iTrack2;
  for (Tracks::const_iterator iTrack1 = seedTracks->begin(); 
       iTrack1 != seedTracks->end(); 
       ++iTrack1 ) {

    if (goodSeed(*iTrack1) == false) continue;

    // Get the matching z-position
    double matchZ = (m_variableZ) ? determineZ( *iTrack1 ) : 
                    m_matchAtZPosition;
   
    State stateAtT = (*iTrack1)->closestState(m_refT);

    // Cut away tracks with p lower than m_momentumCut
    const double momentum = stateAtT.p() ;
    if ( momentum < m_momentumCut ) continue; 

    const double yT = stateAtT.y();
    const double tyT = stateAtT.ty();

    sc = extrapolateSeed(stateAtT, matchZ);
    if ( sc.isFailure() ) continue;
    const TrackVector& trackVector1 = stateAtT.stateVector();
    const TrackSymMatrix& trackCov1 = stateAtT.covariance();

    // Cut away badly reconstructed tracks
    if ( trackCov1(0,0)  > m_seedXCut2  ) continue;
    if ( trackCov1(1,1)  > m_seedYCut2  ) continue;
    if ( trackCov1(2,2)  > m_seedTxCut2 ) continue;
    if ( trackCov1(3,3)  > m_seedTyCut2 ) continue;
     
    //    const double cyRes = 1.0/(gsl_pow_2(1.6) + gsl_pow_2(6.9e4/momentum));
    // const double ctyRes = 1.0/((3.3*(gsl_pow_2(8.6e-4)) + gsl_pow_2(20/momentum)));

    const double cyRes = resT(momentum, m_yTParams);
    const double ctyRes = resT(momentum, m_tyTParams);

    const double dy = 10.0*sqrt(1.0/cyRes);
    const double yStart = yT - dy;   
    const double yStop = yT + dy;  

    iTrack2 = std::lower_bound(veloTracks.begin(),veloTracks.end(), 
                               yStart , VeloCandidate::compByY_LB());
    VeloCandidates::iterator stop = std::lower_bound(iTrack2,veloTracks.end(), 
                                                     yStop , VeloCandidate::compByY_LB()); 
    for ( ; iTrack2 != stop; ++iTrack2) {

      const TrackVector& trackVector2 = (*iTrack2)->trackVector();

      const double yV = (*iTrack2)->yAtT();
      const double chi2T = (gsl_pow_2(yV - yT)*cyRes) + (gsl_pow_2(tyT - trackVector2[3])*ctyRes);
      if (chi2T > m_chi2TCut ) continue;

      const double pt2 = (gsl_pow_2(trackVector2[2]*momentum) + 
                          gsl_pow_2(trackVector2[3]*momentum))/(*iTrack2)->norm2();
      if (pt2 < m_pt2Cut) continue;
  
      // Calculate the chi2 distance between 2 tracks
      const TrackSymMatrix& trackCov2 = (*iTrack2)->covariance();
      double chi2 = 0.0;
      sc = m_chi2Calculator->calculateChi2( trackVector1, trackCov1, 
                                            trackVector2, trackCov2,
                                            chi2 );
      if ( sc.isFailure() ) continue;

      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "TrackChi2Calculator: chi2 = " << chi2 << endmsg;

      // found a match -> add it to temporary matchList
      if ( chi2 < m_chi2MatchingCut ) {   
        TrackMatch* matchTrack = new TrackMatch((*iTrack2)->track(), 
                                                 *iTrack1, chi2);
        matchVector.push_back( matchTrack );
      }
    }
  }

  // Sort the list by chi2 and make new list with the best combinations 
  std::sort( matchVector.begin(), matchVector.end(),
                  TrackMatchingDataFunctor::Less_by_Chi2<const TrackMatch*>());
  
  for ( TrackMatches::const_iterator ipair = matchVector.begin() ;
        ipair != matchVector.end() ; ++ipair) {

    if (m_allCombinations || trackToTake(*ipair)){
      Track* aTrack = new Track;
      aTrack -> addToAncestors( (*ipair)->veloTrack() );
      aTrack -> addToAncestors( (*ipair)->seedTrack() );
      aTrack->addInfo(LHCb::Track::MatchChi2, (*ipair)->chi2());
      matches -> add( aTrack ) ;
    }
  } // loop matches

  // clean up !
  for (TrackMatches::const_iterator iterM = matchVector.begin();
       iterM != matchVector.end(); ++iterM) delete *iterM;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Found " << matches->size() << " matches between velo "
            << "and seed tracks" << endmsg; 

  return StatusCode::SUCCESS;
}

//=============================================================================
// Add TT clusters to matched tracks
//=============================================================================
StatusCode TrackMatchVeloSeed::addTTClusters( Track* track ) 
{
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "Finding TT clusters." << endmsg;

  // Make a new State from the velo state plus momentum from seed track
  const SmartRefVector<Track>& Ancestors = track->ancestors();
  const State& veloState  = Ancestors.front()->firstState();
  const State& seedState  = Ancestors.back()-> firstState();
  TrackVector stateVec    = veloState.stateVector();
  TrackSymMatrix stateCov = veloState.covariance();
  stateVec[4]             = seedState.qOverP();
  stateCov(4,4)           = gsl_pow_2( 0.015 * stateVec[4] );
  State state( stateVec, stateCov, veloState.z(), State::AtTT );

  // Add this state to the track
  track -> addToStates( state );

  // The actual tool to add the TT clusters
  StatusCode sc = m_addTTClusterTool -> addTTClusters( *track );
  if ( sc.isFailure() )
    return Error( "Failure in running the addTTClusterTool." );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Store the new tracks made from the seed- and velo track segments
//=============================================================================
StatusCode TrackMatchVeloSeed::storeTracks( Tracks* matchCont )
{
  // This part below will add the measurements of the Velo-track and
  // the seed-tracks to a new combined track. Also it will make the
  // current state the state closest to the seeding state, since it is

  // assumed that after this algorithm a re-fit will be done. The
  // 're-fit' expects the current state to be the seeding state.

  StatusCode sc;
  if ( msgLevel(MSG::DEBUG) ) debug() << "Registering the tracks ..." << endmsg;

  // Initialize the MeasurementProvider tool for the current event
  if ( m_addMeasurements ) m_measProvider -> load();

  // fill output container
  Tracks::iterator iterMatch = matchCont->begin();
  for ( ; iterMatch != matchCont->end(); ++iterMatch ) {

    Track* aTrack = (*iterMatch);
    const SmartRefVector<Track>& Ancestors = aTrack->ancestors();
    const Track* veloTrack = Ancestors.front();
    const Track* seedTrack = Ancestors.back();
       
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Creating a long track from:" << endmsg
                << " - velo track (key=" << veloTrack -> key()
                << ") : # states / LHCbIDs / Measurements = "
                << veloTrack -> nStates() << " / "
                << veloTrack -> nLHCbIDs() << " / "
                << veloTrack -> nMeasurements() << endmsg
                << " - seed track (key=" << seedTrack -> key()
                << ") : # states / LHCbIDs / Measurements = "
                << seedTrack -> nStates() << " / "
                << seedTrack -> nLHCbIDs() << " / "
                << seedTrack -> nMeasurements() << endmsg;
    
    // Copy velo hits (= LHCbIDs)
    aTrack -> setLhcbIDs( veloTrack -> lhcbIDs() );
    
    // If present copy also measurements
    if ( m_addMeasurements ) {
      std::vector<Measurement*>::const_iterator iVeloMeas;
      for ( iVeloMeas = veloTrack->measurements().begin(); 
            iVeloMeas != veloTrack->measurements().end(); ++iVeloMeas) {
        aTrack -> addToMeasurements( *(*iVeloMeas) );
      }
    }  

    // Search for tt hits
    if ( m_addTTClusters ) {  
      sc = addTTClusters( aTrack );
      if ( sc.isFailure() )
        return Error( "Unable to add TT Clusters to matched tracks" );
    }

    // Copy seed hits (= LHCbIDs) -- if track is not fitted
    if (seedTrack->fitStatus() == Track::FitStatusUnknown ){
      const std::vector<LHCbID>& sids = seedTrack -> lhcbIDs();
      std::vector<LHCbID>::const_iterator itsids = sids.begin();
      while ( itsids != sids.end() ) {
        aTrack -> addToLhcbIDs( (*itsids) );
        ++itsids;
      }
    }
    else {
      std::vector<Measurement*>::const_iterator iSeedMeas;
      for ( iSeedMeas = seedTrack->measurements().begin(); 
            iSeedMeas != seedTrack->measurements().end(); ++iSeedMeas) {
	LHCb::Measurement* meas = *iSeedMeas;
        if (meas != 0 ) aTrack -> addToLhcbIDs( meas->lhcbID());
      }
    }

    // If present copy also seed measurements
    if ( m_addMeasurements) {
      std::vector<Measurement*>::const_iterator iSeedMeas;
      for ( iSeedMeas = seedTrack->measurements().begin(); 
            iSeedMeas != seedTrack->measurements().end(); ++iSeedMeas) {
         aTrack -> addToMeasurements( *(*iSeedMeas) );
       }
    }

    // Add state at T
    State aState = seedTrack -> closestState( StateParameters::ZAtT );
    //State aState = tState;
    aState.setLocation(LHCb::State::AtT);
    TrackSymMatrix newC;
    aState.setCovariance( newC );
    aTrack -> addToStates( aState );

    // Add state at Velo
    if (veloTrack->hasStateAt(LHCb::State::EndVelo) == false) {
      return Warning("No State at Velo", StatusCode::FAILURE);
    }
    State vState = veloTrack->stateAt( LHCb::State::EndVelo );
    vState.setQOverP(aState.qOverP());
    aTrack -> addToStates( vState );

    // Now make all the velo measurements
    if ( m_addMeasurements) {
      const std::vector<LHCb::LHCbID>& ids = veloTrack->lhcbIDs();
      for (std::vector<LHCb::LHCbID>::const_iterator iter = ids.begin(); iter != ids.end(); ++iter){
        LHCb::Measurement* meas = m_measProvider->measurement(*iter);
	// extrapolate to refz and set the reference
        vState.setQOverP(aState.qOverP()); 
        m_extrapolatorVelo->propagate(vState,meas->z());
        meas->setRefVector( vState.stateVector() );
        aTrack->addToMeasurements( *meas );
      } // for
    }// if


    // Set various flags
    aTrack -> setType( Track::Long );
    aTrack -> setHistory( Track::TrackMatching );
    if ( m_addMeasurements ) aTrack -> setPatRecStatus( Track::PatRecMeas );
    else                     aTrack -> setPatRecStatus( Track::PatRecIDs  );

    // debugging the Track
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "  -> Track stored with key # " << aTrack -> key() << endmsg
                << "    * is Invalid    = "
                << aTrack -> checkFlag( Track::Invalid ) << endmsg
                << "    * is Unique     = "
                << !aTrack -> checkFlag( Track::Clone ) << endmsg
                << "    * is Long       = "
                << aTrack -> checkType( Track::Long ) << endmsg
                << "    * chi2, nDoF    = "
                << aTrack -> chi2() << " , " << aTrack -> nDoF() << endmsg
                << "    * # States       = " << aTrack -> states().size() 
                << endmsg
                << "    * # measurements = " << aTrack -> measurements().size()
                << endmsg;
  } // iterMatch

  if ( msgLevel(MSG::DEBUG) )
    debug() << matchCont->size() << " tracks registered in " 
            << TrackLocation::Match << "." << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Calculate the new z
//=============================================================================
double TrackMatchVeloSeed::determineZ( const Track* track )
{
  const State& seedState = track->closestState( StateParameters::ZAtT );
  double tX = seedState.tx();
  double zNew = m_matchAtZPosition ;
  if ( m_varZParameters.size() == 3 ) {
    zNew += m_varZParameters[0] +
            m_varZParameters[1] * tX +
            m_varZParameters[2] * gsl_pow_2( tX );
  }
  return zNew;
}

//=============================================================================


void TrackMatchVeloSeed::createVeloCandidates(LHCb::Tracks* tracks, VeloCandidates& candidates) {

  for (Tracks::iterator iterTrack = tracks->begin(); iterTrack != tracks->end(); ++iterTrack){

    Track* aTrack = *iterTrack;

    // Check on backward going velo tracks
    if ( aTrack->checkFlag( Track::Backward ) ) continue;

    // Remove uninteresting tracks
    const State& veloState = aTrack->closestState( m_matchAtZPosition);
    const TrackVector& vec = veloState.stateVector();
    if ( .350 < fabs( vec[2] ) ) continue;
    if ( .300 < fabs( vec[3] ) ) continue;
 
    // Cut away badly reconstructed tracks
    const TrackSymMatrix& trackCov = veloState.covariance();
    if ( trackCov(0,0)  > m_veloXCut2  )  continue;
    if ( trackCov(1,1)  > m_veloYCut2 )  continue;
    if ( trackCov(2,2)  > m_veloTxCut2 )  continue;
    if ( trackCov(3,3)  > m_veloTyCut2 )  continue;

    State tState = aTrack->closestState(m_matchAtZPosition);
    StatusCode sc = m_extrapolatorVelo->propagate(tState, m_matchAtZPosition);
    if (sc.isSuccess()){
      const double yAtT = vec[3] * m_refT;
      VeloCandidate* can = new VeloCandidate(aTrack,tState,yAtT );
      candidates.push_back(can);
    } // sc

    std::sort(candidates.begin(), candidates.end(), VeloCandidate::increasingY());

  } // iterTrack
}

bool TrackMatchVeloSeed::goodSeed(const LHCb::Track* aTrack) const{

  // remove tracks with bad chi
  if (aTrack->chi2PerDoF() > m_chi2SeedCut ) return false;

  // remove seeds with bad likelihood
  if (aTrack->info(Track::Likelihood, -9999.) < m_likCut ) return false;

  return true;
}

bool TrackMatchVeloSeed::trackToTake(const TrackMatch* aMatch) const{

  // check whether the velo and T part are used
  bool take = true;
  if (usedT(aMatch->seedTrack()) == true){
    take = false;
  }
  else {

    if (usedVelo(aMatch->veloTrack()) == true){
      take = false; 
    }
    else {
      // update used maps
      m_usedT[aMatch->seedTrack()] += 1;
      m_usedVelo[aMatch->veloTrack()] += 1;
    }
  }
  return take;
}

StatusCode TrackMatchVeloSeed::extrapolateSeed(LHCb::State& aState, const double zMatch) const{

 // F = I  
 TrackMatrix F = TrackMatrix( ROOT::Math::SMatrixIdentity() );
 TrackMatrix stepF; 

 // go to match z
 StatusCode sc = StatusCode::SUCCESS;
 double zStart = aState.z();
 int    nbStep = (int)( fabs( zMatch-zStart ) / m_maxStepSize ) + 1;
 double zStep  = (zMatch - zStart) / nbStep;
 TrackVector& vec = aState.stateVector(); 
 for ( int step= 0 ; nbStep > step && sc.isSuccess() ; ++step ) {
   const double zTarget = zStart + zStep;
   //   sc = m_extrapolatorSeed->propagate(aState , zTarget );
   sc = m_extrapolatorSeed->propagate(vec,zStart,zTarget,&stepF);
   Gaudi::TrackMatrix tempF( F );
   F = stepF * tempF;
   zStart = zTarget;
 }

 // update C and z 
 aState.setZ(zMatch);
 aState.setCovariance( ROOT::Math::Similarity<double,TrackMatrix::kRows,TrackMatrix::kCols>( F,aState.covariance() ) );

 return sc;
}



