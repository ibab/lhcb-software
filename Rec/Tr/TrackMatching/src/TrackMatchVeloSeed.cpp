// $Id: TrackMatchVeloSeed.cpp,v 1.14 2006-05-09 08:48:12 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// for std::auto_ptr
//#include <memory>
#include <boost/shared_ptr.hpp>


// from GSL
#include "gsl/gsl_math.h"

//from STDet
#include "STDet/DeSTSector.h"

// from STEvent
#include "Event/STCluster.h"

// from TrackFitEvent
#include "Event/STMeasurement.h"
#include "Event/StateTraj.h"

// local
#include "TrackMatchVeloSeed.h"
#include "TTCandidate.h"
#include "TrackMatchingDataFunctor.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrajPoca.h"
#include "Kernel/Trajectory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackMatchVeloSeed
//
// 2001-05-16 : J. van Tilburg jtilburg@nikhef.nl
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TrackMatchVeloSeed>          s_factory ;
const        IAlgFactory& TrackMatchVeloSeedFactory = s_factory ; 


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
  , m_ttTracker(0)
{
  declareProperty( "InputVeloTracks",  m_veloTracks = TrackLocation::Velo );
  declareProperty( "InputSeedTracks",  m_seedTracks = TrackLocation::Seed );

  declareProperty( "Chi2MatchingCut",  m_chi2MatchingCut = 500.0 );
  declareProperty( "AllCombinations",  m_allCombinations = false );
  declareProperty( "MomentumCut",      m_momentumCut = 1000.0 );
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
  declareProperty( "ErrorX2",          m_errorX2   =   4.0 );
  declareProperty( "ErrorY2",          m_errorY2   = 400.0 );
  declareProperty( "ErrorTx2",         m_errorTx2 = 6.e-5 );
  declareProperty( "ErrorTy2",         m_errorTy2 = 1.e-4 );
  declareProperty( "ErrorP",           m_errorP =  0.15 );
  declareProperty( "AddTTClusters",    m_addTTClusters = true );
  declareProperty( "TTClusterCut",     m_ttClusterCut = 10.0 );
  declareProperty( "MinTTHits",        m_minTTHits = 3 );
  declareProperty( "InterStationCut",  m_interStationCut = 2.0 );
  declareProperty( "IntraStationCut",  m_intraStationCut = 1.0 );
  declareProperty( "SpreadWeight",     m_spreadWeight = 7.0 );

  declareProperty( "ExtrapolatorVelo",
                   m_extrapolatorVeloName = "TrackLinearExtrapolator" );
  declareProperty( "ExtrapolatorSeed",
                   m_extrapolatorSeedName = "TrackHerabExtrapolator" );

  declareProperty( "TTGeometryPath",
                   m_ttTrackerPath = DeSTDetLocation::location("TT") );
  declareProperty( "TTClusterPositionTool",
                   m_ttPositionToolName = "STOfflinePosition" );
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

  debug() << "==> Initialize" << endmsg;

  // Access the extrapolators tools
  m_extrapolatorVelo = tool<ITrackExtrapolator>( m_extrapolatorVeloName );

  m_extrapolatorSeed = tool<ITrackExtrapolator>( m_extrapolatorSeedName );

  // Access the chi2 calculator tool
  m_chi2Calculator = tool<ITrackChi2Calculator>( "TrackChi2Calculator" );

  // Access the measurement provider tool
  m_measProvider = tool<IMeasurementProvider>( "MeasurementProvider" );

  // Get TT silicon tracker geometry
  m_ttTracker = getDet<DeTTDetector>( m_ttTrackerPath );

  // Retrieve the STClusterPosition tool for TT
  m_ttPositionTool = tool<ISTClusterPosition>( m_ttPositionToolName );

  // Magnetic field
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );

  // Poca tool to calculate distance of closest approach
  m_poca = tool<ITrajPoca>( "TrajPoca" );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackMatchVeloSeed::execute() {

  debug() << "==> Execute" << endmsg;

  // Retrieve velo tracks from EvDS
  Tracks* veloTracks = get<Tracks>( m_veloTracks );
  debug() << "retrieved succesfully " << veloTracks -> size()
          << " velo tracks" << endmsg;

  // Retrieve seed tracks from EvDS
  Tracks* seedTracks = get<Tracks>( m_seedTracks );
  debug() << "retrieved succesfully " << seedTracks -> size()
          << " seed tracks" << endmsg;

  // create and register the container for the matched tracks
  TrackMatches* matches = new TrackMatches();
  put( matches, TrackMatchLocation::Default );
  debug() << "Output container " << TrackMatchLocation::Default
          << " registered" << endreq;
  
  // the actual matching of the tracks
  debug() << "Trying to match velo with seed tracks ..." << endmsg;
  StatusCode sc = matchTracks( veloTracks, seedTracks, matches );
  if ( sc.isFailure() )
    return Error( "Unable to match velo tracks with Seed tracks" );

  // Add TT Clusters
  if ( m_addTTClusters ) {  
    sc = addTTClusters( matches );
    if ( sc.isFailure() )
      return Error( "Unable to add TT Clusters to matched tracks" );
  }

  // Store the new tracks
  sc = storeTracks( matches );
  if ( sc.isFailure() )
    return Error( "Unable to store new tracks." );

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackMatchVeloSeed::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// Match velo tracks with seed tracks
//=============================================================================
StatusCode TrackMatchVeloSeed::matchTracks( Tracks* veloTracks,
                                            Tracks* seedTracks,
                                            TrackMatches*& matches )
{
  // Match the two trackcontainers at the given z-position according
  // to the lowest chi2-distance and at most chi2=m_chi2MatchingCut.
  // Returns a vector of pointers to these tracks.

  StatusCode sc;

  TrackMatchVector matchVector;         // temporary list
  Tracks::const_iterator iTrack1 = seedTracks->begin();
  Tracks::const_iterator iTrack2 = veloTracks->begin();
  for ( iTrack1 = seedTracks->begin(); iTrack1 != seedTracks->end(); 
        ++iTrack1 ) {

    // Remove seed tracks with bad chi2/ndf
    int ndf = (*iTrack1) -> nDoF();
    double chi2ndf = (*iTrack1) -> chi2PerDoF();
    if ( ndf < 1 || chi2ndf > 100.0 ) continue;

    // Extrapolate seedTrack to the actual m_matchAtZPosition
    TrackVector trackVector1;
    TrackSymMatrix trackCov1;
    sc = extrapolate( *iTrack1, m_extrapolatorSeed, 
                      m_matchAtZPosition, trackVector1, trackCov1 );
    if ( sc.isFailure() ) continue;

    // Cut away badly reconstructed tracks
    if ( !m_allCombinations ) {
      if ( sqrt(trackCov1(0,0) ) > m_seedXCut )  continue;
      if ( sqrt(trackCov1(1,1) ) > m_seedYCut )  continue;
      if ( sqrt(trackCov1(2,2) ) > m_seedTxCut ) continue;
      if ( sqrt(trackCov1(3,3) ) > m_seedTyCut ) continue;
    }

    for ( iTrack2 = veloTracks->begin(); iTrack2 != veloTracks->end(); 
          ++iTrack2) {

      // Check on backward going velo tracks
      if ( (*iTrack2) -> checkFlag( Track::Backward ) ) continue;

      // Remove uninteresting tracks
      State& veloState = (*iTrack2) -> closestState( 0.0 );
      TrackVector vec = veloState.stateVector();
      if ( .350 < fabs( vec[2] ) ) continue;
      if ( .300 < fabs( vec[3] ) ) continue;

      // Extrapolate veloTrack to the actual m_matchAtZPosition
      TrackVector trackVector2;
      TrackSymMatrix trackCov2;
      sc = extrapolate( *iTrack2, m_extrapolatorVelo,
                        m_matchAtZPosition, trackVector2, trackCov2 );
      if ( sc.isFailure() ) continue;

      // Cut away badly reconstructed tracks
      if ( !m_allCombinations ) {
        if ( sqrt(trackCov2(0,0) ) > m_veloXCut )  continue;
        if ( sqrt(trackCov2(1,1) ) > m_veloYCut )  continue;
        if ( sqrt(trackCov2(2,2) ) > m_veloTxCut ) continue;
        if ( sqrt(trackCov2(3,3) ) > m_veloTyCut ) continue;
      }

      // Cut away tracks with pT-kick lower than m_momentumCut
      double momentum =  1.0 / fabs( trackVector1[4] ) ;
      if ( momentum < m_momentumCut ) continue; 

      // If asked extrapolate to a parametrized z-position:
      if ( m_variableZ ) {
        double newZ = m_matchAtZPosition;
        sc = determineZ( trackVector1[2], newZ);
        if ( sc.isFailure() ) continue;
        sc = extrapolate( *iTrack1, m_extrapolatorSeed, 
                          newZ, trackVector1, trackCov1 );
        if ( sc.isFailure() ) continue;
        sc = extrapolate( *iTrack2, m_extrapolatorVelo,
                          newZ, trackVector2, trackCov2 );
        if ( sc.isFailure() ) continue;
      }

      // Calculate the chi2 distance between 2 tracks
      double chi2 = 0.0;

      debug() << "Calling TrackChi2Calculator ..." << endreq;
      
      sc = m_chi2Calculator->calculateChi2( trackVector1, trackCov1, 
                                            trackVector2, trackCov2,
                                            chi2 );
      if ( sc.isFailure() ) continue;

      debug() << "-> chi2 = " << chi2 << endreq;

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

  int numCombinations = 0;
  for ( TrackMatches::const_iterator ipair = matchVector.begin() ;
        ipair != matchVector.end() ; ++ipair) {
    bool found = false ;
    for ( TrackMatches::const_iterator igoodpair = matches->begin() ;
          igoodpair != matches->end() && !found ; ++igoodpair) 
      found = (*ipair)->veloTrack() == (*igoodpair)->veloTrack() ||
              (*ipair)->seedTrack() == (*igoodpair)->seedTrack() ;
    if ( (!found || m_allCombinations) && numCombinations < 500 ) {
      matches -> add(*ipair) ;
      ++numCombinations;
    } else {
      delete *ipair;    
    }
  }
  matchVector.clear();

  debug() << "Found " << matches->size() << " matches between velo "
          << "and seed tracks" << endmsg; 

  return StatusCode::SUCCESS;
}

//=============================================================================
// Add TT clusters to matched tracks
//=============================================================================
StatusCode TrackMatchVeloSeed::addTTClusters( TrackMatches*& matchCont )
{
  // This function will add TT clusters to the matched tracks
  // First a velo track is extrapolated using the momentum of the
  // seed track. Then the closest TT clusters are found.

  // Get the TT Clusters
  STClusters* clusters = get<STClusters>( STClusterLocation::TTClusters );

  // Loop over the clusters and make a trajectory for each cluster
  // TODO: maybe put this functionality in STMeasurement and use projectors
  typedef std::pair<STCluster*, boost::shared_ptr<LHCb::Trajectory> >
    STClusterTrajectory;
  typedef std::vector<STClusterTrajectory> STClusterTrajectories;
  STClusterTrajectories clusterTrajectories;
  STClusters::iterator iClus = clusters -> begin();
  for ( ; iClus != clusters -> end(); ++iClus ) {

    // Get the cluster and STChannelID
    STCluster* stCluster = (*iClus) ;
    STChannelID stChan = stCluster -> channelID() ; 
    ISTClusterPosition::Info measVal =
      m_ttPositionTool -> estimate( stCluster );

    // Create a trajectory for this cluster and add combination to the vector
    STClusterTrajectory thisClusTraj;    
    thisClusTraj.first  = stCluster ;
    // explicitly transfer ownership from the auto_ptr returned by 'trajectory'
    // to the shared_ptr thisClusTraj.second
    thisClusTraj.second.reset( m_ttTracker -> trajectory( LHCbID( stChan ),
                                       measVal.fractionalPosition).release() );
    clusterTrajectories.push_back( thisClusTraj ) ;
  }

  // Loop over matched tracks
  debug() << "Looping over the matches tracks ..." << endmsg;
  TrackMatches::const_iterator iterMatch = matchCont->begin();  
  for ( ; iterMatch != matchCont->end(); ++iterMatch) {

    // Make a new State from the velo state plus momentum from seed track
    const Track* veloTrack = (*iterMatch) -> veloTrack();
    const Track* seedTrack = (*iterMatch) -> seedTrack();
    const State& veloState = veloTrack -> closestState(0.);
    const State& seedState = seedTrack->closestState(9900.);

    TrackVector stateVec = veloState.stateVector();
    TrackSymMatrix stateCov = veloState.covariance();
    stateVec[4]          = seedState.qOverP();
    stateCov(4,4)        = gsl_pow_2( 0.015 * stateVec[4] );

    State* state = new State();
    state -> setZ( veloState.z() );
    state -> setState( stateVec );
    state -> setCovariance( stateCov );

    // The vector of pointers to TT candidate hit-clusters
    TTCandidates candidates;

    // loop over layers in TT and find which sector is hit
    const DeSTDetector::Layers& ttLayers = m_ttTracker->layers();
    const unsigned int numTTLayers = ttLayers.size();
    DeSTDetector::Layers::const_iterator iLayer = ttLayers.begin();
    for ( ; iLayer != ttLayers.end(); ++iLayer) {

      // For this TT layer: extrapolate the new State to the z of the layer
      DeSTLayer* aLayer = *iLayer;
      double zLayer = (aLayer->globalCentre()).z() ;
      StatusCode sc =
        m_extrapolatorSeed -> propagate( *state, zLayer );
      if ( sc.isFailure() ) { 
        debug() << "extrapolation of state to z=" << zLayer
                << " failed" << endmsg;
        continue;
      }

      // calculate the layer number (0-3)
      unsigned int layerNum = iLayer - ttLayers.begin();

      // Get the state trajectory
      XYZVector bfield(0,0,0);
      m_pIMF -> fieldVector( state->position(), bfield );
      StateTraj stateTraj = StateTraj( state->stateVector(), zLayer, bfield );
  
      STClusterTrajectories::iterator iClusTraj = clusterTrajectories.begin();
      for ( ; iClusTraj != clusterTrajectories.end(); ++iClusTraj ) {
        Trajectory* measTraj = ((*iClusTraj).second).get();

        // Determine the distance between track state and measurement
        double s1, s2;
        XYZVector distance3D;
        s1 = 0.0;
        s2 = measTraj->arclength( stateTraj.position(s1) );
        m_poca -> minimize( stateTraj, s1, *measTraj, s2, distance3D, 50*mm );
        int signDist = ( distance3D.x() > 0.0 ) ? 1 : -1 ;
        double distance = signDist * distance3D.R();

        if ( fabs( distance ) < m_ttClusterCut ) {
          bool clusterUsed = false;
          unsigned int iCand = 0;
          unsigned int numCandidates = candidates.size();
          while ( iCand < numCandidates ) {
            TTCandidate* cand = candidates[iCand];
            if ( cand->lastLayer() < layerNum ) {
              // TODO: Remove geometry assumptions
              bool interStation = layerNum == 2 || 
                (layerNum == 3 && cand->lastLayer() != 2 );
              double delta = fabs(cand->lastDistance() - distance);
              if ( (interStation && delta < m_interStationCut) ||
                   (!interStation && delta < m_intraStationCut) ) {
                // if cluster compatible: make new set of clusters
                // using the old one and flag the old as dead.
                TTCandidate* newCand = new TTCandidate(cand,*iClus,
                                                       distance,layerNum);
                candidates.push_back( newCand );
                cand->setDead(true);
                clusterUsed = true;
              } 
            }
            ++iCand;
          } 
          if ( !clusterUsed ) {
            TTCandidate* newCand = new TTCandidate( *iClus, distance, 
                                                    layerNum);
            candidates.push_back( newCand );
          }
        }
      } // loop iClusTraj

      // delete all dead candidates and those with too few hits
      TTCandidates::iterator iCand = candidates.begin();
      while ( iCand != candidates.end() ) {
        bool tooFewHits = int( (*iCand)->numTTClusters() - m_minTTHits + 
                               numTTLayers - layerNum ) < 1;
        if ( (*iCand)->isDead() || tooFewHits ) {
          delete *iCand;
          iCand = candidates.erase( iCand );
        } else ++iCand;
      }
    } // loop iLayer
    delete state;

    // find the best TT candidate
    double bestQuality = 2.*m_ttClusterCut;
    std::vector<STCluster*> bestCand;
    std::vector<double> bestDistances;
    TTCandidates::const_iterator iCand;
    for ( iCand = candidates.begin(); iCand != candidates.end(); ++iCand ) {
      // the quality is defined as: |<distance>| + m_spreadWeight*spread 
      double quality = fabs((*iCand)->averageDistance()) + 
        m_spreadWeight * (*iCand)->spread();
      if ( quality < bestQuality ) {
        bestQuality = quality;
        bestCand = (*iCand)->ttClusters();
        bestDistances = (*iCand)->distances();
      }
      delete *iCand;
    }
    candidates.clear();

    // add the TT clusters of the best candidate
    if ( !bestCand.empty() ) {
      (*iterMatch)->setTTClusters( bestCand );
      (*iterMatch)->setTTChi2s( bestDistances );
    }    

  } // loop TrackMatches

  return StatusCode::SUCCESS;
}

//=============================================================================
// Store the new tracks made from the seed- and velo track segments
//=============================================================================
StatusCode TrackMatchVeloSeed::storeTracks( TrackMatches*& matchCont )
{
  // This part below will add the measurements of the Velo-track and
  // the seed-tracks to a new combined track. Also it will make the
  // current state the state closest to the seeding state, since it is

  // assumed that after this algorithm a re-fit will be done. The
  // 're-fit' expects the current state to be the seeding state.

  StatusCode sc = StatusCode::SUCCESS;

  Tracks* trackCont = new Tracks();

  // Initialize the MeasurementProvider tool for the current event
  m_measProvider -> load();

  // fill output container
  TrackMatches::const_iterator iterMatch;
  for ( iterMatch = matchCont->begin() ;
        iterMatch != matchCont->end(); ++iterMatch ) {

    const Track* veloTrack = (*iterMatch) -> veloTrack();
    const Track* seedTrack = (*iterMatch) -> seedTrack();
    Track* aTrack = new Track();
    
    debug() << "Creating a long track from:" << endreq
            << " - velo track (key=" << veloTrack -> key()
            << ") : # states / LHCbIDs / Measurements = "
            << veloTrack -> nStates() << " / "
            << veloTrack -> nLHCbIDs() << " / "
            << veloTrack -> nMeasurements() << endreq
            << " - seed track (key=" << seedTrack -> key()
            << ") : # states / LHCbIDs / Measurements = "
            << seedTrack -> nStates() << " / "
            << seedTrack -> nLHCbIDs() << " / "
            << seedTrack -> nMeasurements() << endreq;
    
    // Copy velo hits
    aTrack -> setLhcbIDs( veloTrack -> lhcbIDs() );
    
    // Copy tt hits
    LHCbID lhcbID;
    std::vector<STCluster*> ttClusters = (*iterMatch)->ttClusters();
    std::vector<STCluster*>::const_iterator iClus = ttClusters.begin();
    while ( iClus != ttClusters.end() ) {
      lhcbID = LHCbID( (*iClus) -> channelID() );
      aTrack -> addToLhcbIDs( lhcbID );
      ++iClus;
    }

    // Copy seed hits
    const std::vector<LHCbID>& sids = seedTrack -> lhcbIDs();
    std::vector<LHCbID>::const_iterator itsids = sids.begin();
    while ( itsids != sids.end() ) {
      aTrack -> addToLhcbIDs( (*itsids) );
      ++itsids;
    }

    // Now make all the Measurements "in one go"
    sc = m_measProvider -> load( *aTrack );
    if ( sc.isFailure() )
      return Error( "Unable to load measurements!", StatusCode::FAILURE );

    debug() << "# LHCbIDs, Measurements = " << aTrack -> nLHCbIDs()
            << ", " << aTrack -> nMeasurements() << endreq;

    // initial parameters
    //std::vector<Measurement*>::const_iterator lastMeas =
    //  seedTrack -> measurements().end();
    //--lastMeas;
    std::vector<Measurement*>::const_iterator lastMeas =
      aTrack -> measurements().end();
    --lastMeas;

    debug() << "lastMeas at z = " << (*lastMeas)->z() << endreq;

     const State& closestState = seedTrack -> closestState( (*lastMeas)->z() );
    State* aState = closestState.clone();
    sc = m_extrapolatorSeed -> propagate( *aState, (*lastMeas)->z() );
    if ( sc.isFailure() ) {
      debug() << "extrapolation of state to z = "
              << (*lastMeas)->z() << " failed" << endmsg;
      delete aState;
      delete aTrack;
      continue;
    }

    debug() << "blow covariance matrix" << endreq;
    // Blow up covariance matrix
    TrackSymMatrix newC;
    newC(0,0) = m_errorX2;
    newC(1,1) = m_errorY2;
    newC(2,2) = m_errorTx2;
    newC(3,3) = m_errorTy2;
    newC(4,4) = gsl_pow_2( aState->qOverP() * m_errorP );
    aState -> setCovariance( newC );

    aTrack -> addToStates( *aState );

    // Set various flags
    aTrack -> setType( Track::Long );
    aTrack -> setHistory( Track::TrackMatching );
    aTrack -> setStatus( Track::PatRecMeas );

    // Set the ancestor tracks
    aTrack -> addToAncestors( veloTrack );
    aTrack -> addToAncestors( seedTrack );

    trackCont -> add( aTrack );

    delete aState;

    // debugging the Track
    if ( msgLevel(MSG::VERBOSE) ) {
      verbose()
        << "  -> Track stored with key # " << aTrack -> key() << endreq
        << "    * is Invalid    = "
        << aTrack -> checkFlag( Track::Invalid ) << endreq
        << "    * is Unique     = "
        << !aTrack -> checkFlag( Track::Clone ) << endreq
        << "    * is Long       = "
        << aTrack -> checkType( Track::Long ) << endreq
        << "    * chi2, nDoF    = "
        << aTrack -> chi2() << " , " << aTrack -> nDoF() << endreq
        << "    * # States       = " << aTrack -> states().size() << endreq
        << "    * # measurements = " << aTrack -> measurements().size() 
        << endreq;
        }
  } // iterMatch

  debug() << "Registering the tracks ..." << endreq;

  // Here the new tracks are registered
  put( trackCont, TrackLocation::Match );
  debug() << "Output container " << TrackLocation::Match
          << " registered" << endreq;

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
StatusCode TrackMatchVeloSeed::determineZ( double tX,
                                           double& zNew )
{
  if ( m_varZParameters.size() == 3 ) {
    zNew = m_matchAtZPosition ;
    zNew += m_varZParameters[0] +
            m_varZParameters[1] * tX +
            m_varZParameters[2] * gsl_pow_2( tX );
  }
  else {
    zNew = m_matchAtZPosition ;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
