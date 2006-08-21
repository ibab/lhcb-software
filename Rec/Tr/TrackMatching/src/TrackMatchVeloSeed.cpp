// $Id: TrackMatchVeloSeed.cpp,v 1.26 2006-08-21 15:55:03 mneedham Exp $
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

// local
#include "TrackMatchVeloSeed.h"
#include "TrackMatchingDataFunctor.h"
#include "AddTTClusterTool.h"

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
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_extrapolatorVelo(0)
  , m_extrapolatorSeed(0)
  , m_chi2Calculator(0)
  , m_measProvider(0)
{
  declareProperty( "InputVeloTracks",  m_veloTracks = TrackLocation::Velo );
  declareProperty( "InputSeedTracks",  m_seedTracks = TrackLocation::Seed );

  declareProperty( "Chi2MatchingCut",  m_chi2MatchingCut = 500.0 );
  declareProperty( "AllCombinations",  m_allCombinations = false );
  declareProperty( "MomentumCut",      m_momentumCut = 1000.0 );
  declareProperty( "ptCut",      m_ptCut = 50.0 );
  declareProperty( "VeloXCut",         m_veloXCut = 0.6 );
  declareProperty( "VeloYCut",         m_veloYCut = 0.6 );
  declareProperty( "VeloTxCut",        m_veloTxCut = 8.0e-4 );
  declareProperty( "VeloTyCut",        m_veloTyCut = 8.0e-4 );
  declareProperty( "SeedXCut",         m_seedXCut = 150.0 );
  declareProperty( "SeedYCut",         m_seedYCut = 30.0 );
  declareProperty( "SeedTxCut",        m_seedTxCut = 0.1 );
  declareProperty( "SeedTyCut",        m_seedTyCut = 0.01 );
  declareProperty( "MatchAtZPosition", m_matchAtZPosition = 830.0 );
  declareProperty( "VariableZ",        m_variableZ = false );
  declareProperty( "VarZParameters",   m_varZParameters );
  declareProperty( "AddTTClusters",    m_addTTClusters = true );
  declareProperty( "AddMeasurements",  m_addMeasurements = false );
  declareProperty("Chi2SeedCut", m_chi2SeedCut = 200.);

  declareProperty( "ExtrapolatorVelo",
                   m_extrapolatorVeloName = "TrackLinearExtrapolator" );
  declareProperty( "ExtrapolatorSeed",
                   m_extrapolatorSeedName = "TrackHerabExtrapolator" );

  declareProperty("TTClusterToolName", m_ttClusterToolName = "AddTTClusterTool" );

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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

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

  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "retrieved succesfully " << veloTracks -> size()
            << " velo tracks" << endmsg;
    debug() << "retrieved succesfully " << seedTracks -> size()
            << " seed tracks" << endmsg;
  }
  
  // create and register the container for the matched tracks
  Tracks* matches = new Tracks();
  matches->reserve(veloTracks->size());
  put( matches, TrackLocation::Match );
  
  // the actual matching of the tracks
  StatusCode sc = matchTracks( veloTracks, seedTracks, matches );
  if ( sc.isFailure() )
    return Error( "Unable to match velo tracks with Seed tracks" );

  // Store the new tracks
  sc = storeTracks( matches );
  if ( sc.isFailure() ) return Error( "Unable to store new tracks." );

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackMatchVeloSeed::finalize()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// Match velo tracks with seed tracks
//=============================================================================
StatusCode TrackMatchVeloSeed::matchTracks( Tracks* veloTracks,
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
  Tracks::const_iterator iTrack1 = seedTracks->begin();
  Tracks::const_iterator iTrack2 = veloTracks->begin();
  for ( iTrack1 = seedTracks->begin(); iTrack1 != seedTracks->end(); 
        ++iTrack1 ) {

    // Remove seed tracks with bad chi2/ndf
    int ndf = (*iTrack1) -> nDoF();
    double chi2ndf = (*iTrack1) -> chi2PerDoF();
    if ( ndf < 1 || chi2ndf > m_chi2SeedCut ) continue;

    // Get the matching z-position
    double matchZ = (m_variableZ) ? determineZ( *iTrack1 ) : 
      m_matchAtZPosition;

    // Extrapolate seedTrack to the actual m_matchAtZPosition
    TrackVector trackVector1;
    TrackSymMatrix trackCov1;
    sc = extrapolate( *iTrack1, m_extrapolatorSeed, 
                      matchZ, trackVector1, trackCov1 );
    if ( sc.isFailure() ) continue;

    // Cut away badly reconstructed tracks
    if ( !m_allCombinations ) {
      if ( sqrt(trackCov1(0,0) ) > m_seedXCut  ) continue;
      if ( sqrt(trackCov1(1,1) ) > m_seedYCut  ) continue;
      if ( sqrt(trackCov1(2,2) ) > m_seedTxCut ) continue;
      if ( sqrt(trackCov1(3,3) ) > m_seedTyCut ) continue;
    }

    for ( iTrack2 = veloTracks->begin(); iTrack2 != veloTracks->end(); 
          ++iTrack2) {

      // Check on backward going velo tracks
      if ( (*iTrack2) -> checkFlag( Track::Backward ) ) continue;

      // Remove uninteresting tracks
      const State& veloState = (*iTrack2) -> closestState( 0.0 );
      TrackVector vec = veloState.stateVector();
      if ( .350 < fabs( vec[2] ) ) continue;
      if ( .300 < fabs( vec[3] ) ) continue;

      // Extrapolate veloTrack to the actual m_matchAtZPosition
      TrackVector trackVector2;
      TrackSymMatrix trackCov2;
      sc = extrapolate( *iTrack2, m_extrapolatorVelo,
                        matchZ, trackVector2, trackCov2 );
      if ( sc.isFailure() ) continue;

      // Cut away badly reconstructed tracks
      if ( !m_allCombinations ) {
        if ( sqrt(trackCov2(0,0) ) > m_veloXCut  ) continue;
        if ( sqrt(trackCov2(1,1) ) > m_veloYCut  ) continue;
        if ( sqrt(trackCov2(2,2) ) > m_veloTxCut ) continue;
        if ( sqrt(trackCov2(3,3) ) > m_veloTyCut ) continue;
      }

      // Cut away tracks with pT-kick lower than m_momentumCut
      double momentum =  1.0 / fabs( trackVector1[4] ) ;
      if ( momentum < m_momentumCut ) continue; 

      // cut on pt
      double norm = sqrt(1.0 + trackVector2[2]* trackVector2[2]  + trackVector2[3]* trackVector2[3]);
      double px = trackVector2[2]*momentum/norm;
      double py = trackVector2[3]*momentum/norm;
      double pt = sqrt(px*px + py*py);
      if (pt < m_ptCut) continue;
  

      // Calculate the chi2 distance between 2 tracks
      double chi2 = 0.0;
      sc = m_chi2Calculator->calculateChi2( trackVector1, trackCov1, 
                                            trackVector2, trackCov2,
                                            chi2 );
      if ( sc.isFailure() ) continue;

      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "TrackChi2Calculator: chi2 = " << chi2 << endmsg;

      // found a match -> add it to temporary matchList
      if ( chi2 < m_chi2MatchingCut ) {
        TrackMatch* matchTrack = new TrackMatch( *iTrack2, *iTrack1, chi2 );
        matchVector.push_back( matchTrack );
      }
    }
  }

  // Sort the list by chi2 and make new list with the best combinations
  std::stable_sort( matchVector.begin(), matchVector.end(),
                  TrackMatchingDataFunctor::Less_by_Chi2<const TrackMatch*>());
  for ( TrackMatches::const_iterator ipair = matchVector.begin() ;
        ipair != matchVector.end() ; ++ipair) {
    bool found = false ;
    for ( Tracks::const_iterator iGoodMatch = matches->begin() ;
          iGoodMatch != matches->end() && !found ; ++iGoodMatch ) {

      const SmartRefVector<Track>& Ancestors = (*iGoodMatch)->ancestors();
      const Track* veloTrack = Ancestors.front();
      const Track* seedTrack = Ancestors.back();

      found = (*ipair)->veloTrack() == veloTrack ||
        (*ipair)->seedTrack() == seedTrack ;
    }
    if ( (!found || m_allCombinations) ) {
      Track* aTrack = new Track;
      aTrack -> addToAncestors( (*ipair)->veloTrack() );
      aTrack -> addToAncestors( (*ipair)->seedTrack() );
      aTrack->addInfo(LHCb::Track::MatchChi2, (*ipair)->chi2());
      matches -> add( aTrack ) ;
    }
  }

  // clean up !
  for (TrackMatches::const_iterator iterM = matchVector.begin();
       iterM != matchVector.end(); ++iterM) delete *iterM;
  matchVector.clear();

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
  //  std::vector<double> ttChi2s;
  // std::vector<STCluster*> ttClusters;
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

    // Copy seed hits (= LHCbIDs)
    const std::vector<LHCbID>& sids = seedTrack -> lhcbIDs();
    std::vector<LHCbID>::const_iterator itsids = sids.begin();
    while ( itsids != sids.end() ) {
      aTrack -> addToLhcbIDs( (*itsids) );
      ++itsids;
    }

    // If present copy also seed measurements
    if ( m_addMeasurements ) {
      std::vector<Measurement*>::const_iterator iSeedMeas;
      for ( iSeedMeas = seedTrack->measurements().begin(); 
            iSeedMeas != seedTrack->measurements().end(); ++iSeedMeas) {
        aTrack -> addToMeasurements( *(*iSeedMeas) );
       }
    }

    // Now make all the remaining Measurements "in one go"
    if ( m_addMeasurements && aTrack->nMeasurements() < aTrack->nLHCbIDs() ) {
      sc = m_measProvider -> load( *aTrack );
      if ( sc.isFailure() )
        return Error( "Unable to load measurements!", StatusCode::FAILURE );
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
    const State vState = veloTrack->stateAt( LHCb::State::EndVelo );
    //State aState2 = vState;
    aTrack -> addToStates( vState );

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
// Extrapolate a Track to a z-position starting with the closest State
//=============================================================================
StatusCode TrackMatchVeloSeed::extrapolate( Track* track,
                                            ITrackExtrapolator* extrapolator,
                                            double zpos,
                                            TrackVector& trackVector,
                                            TrackSymMatrix& trackCov )
{
  State tmpState;
  StatusCode sc = extrapolator -> propagate( *track, zpos, tmpState );
  if ( sc.isFailure() ) return sc;
  trackVector = tmpState.stateVector();
  trackCov    = tmpState.covariance();

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
