// $Id: TrackMatchVeloSeed.cpp,v 1.4 2005-10-13 14:57:10 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from TrackFit Event
#include "Event/ITMeasurement.h"

// TT clusters
#include "Event/ITCluster.h"
#include "STDet/STDetectionLayer.h"
#include "STDet/STWafer.h"

// local
#include "TrackMatchVeloSeed.h"
#include "TTCandidate.h"
#include "TrackMatchingDataFunctor.h"

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
  , m_itTracker(0)
{
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
  declareProperty( "TtClusterCut",     m_ttClusterCut = 10.0 );
  declareProperty( "MinTTHits",        m_minTTHits = 3 );
  declareProperty( "NumTTLayers",      m_numTTLayers = 4 );
  declareProperty( "InterStationCut",  m_interStationCut = 2.0 );
  declareProperty( "IntraStationCut",  m_intraStationCut = 1.0 );
  declareProperty( "SpreadWeight",     m_spreadWeight = 7.0 );

  declareProperty( "extrapolatorVelo",
                   m_extrapolatorVeloName = "TrLinearExtrapolator" );
  declareProperty( "extrapolatorSeed",
                   m_extrapolatorSeedName = "TrHerabExtrapolator" );

  declareProperty( "ITGeometryPath",
                   m_itTrackerPath = "/dd/Structure/LHCb/Tracker/IT" );

  m_particleID = 211;   // track->particleID();
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

  // Get silicon tracker geometry
  m_itTracker = getDet<DeSTDetector>( m_itTrackerPath );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackMatchVeloSeed::execute() {

  debug() << "==> Execute" << endmsg;

  // Retrieve velo tracks from EvDS
  Tracks* veloTracks = get<Tracks>( TrackLocation::Velo );
  debug() << "retrieved succesfully " << veloTracks->size()
          << " velo tracks" << endmsg;

  // Retrieve seed tracks from EvDS
  Tracks* seedTracks = get<Tracks>( TrackLocation::Seed );
  debug() << "retrieved succesfully " << seedTracks->size()
          << " seed tracks" << endmsg;

  // create and register the container for the matched tracks
  TrackMatches* matches = new TrackMatches();
  StatusCode sc = put( matches, TrackMatchLocation::Default );
  if( sc.isFailure() ) {
    delete matches;
    error() << "Unable to register the output container "
            << TrackMatchLocation::Default << ". Status is " << sc << endreq;
    return sc ;
  }
  else {
    debug() << "Output container " << TrackMatchLocation::Default
            << " registered" << endreq;
  }
  
  // the actual matching of the tracks
  debug() << "Trying to match velo with seed tracks." << endmsg;
  sc = matchTracks( veloTracks, seedTracks, matches );
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
//  
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
    HepVector trackVector1;
    HepSymMatrix trackCov1;
    sc = extrapolate( *iTrack1, m_extrapolatorSeed, 
                      m_matchAtZPosition, trackVector1, trackCov1);
    if ( sc.isFailure() ) continue;

    // Cut away badly reconstructed tracks
    if ( !m_allCombinations ) {
      if ( sqrt(trackCov1[0][0] ) > m_seedXCut) continue;
      if ( sqrt(trackCov1[1][1] ) > m_seedYCut) continue;
      if ( sqrt(trackCov1[2][2] ) > m_seedTxCut) continue;
      if ( sqrt(trackCov1[3][3] ) > m_seedTyCut) continue;
    }

    for ( iTrack2 = veloTracks->begin(); iTrack2 != veloTracks->end(); 
          ++iTrack2) {

      // Check on backward going velo tracks
      if ( (*iTrack2) -> checkFlag( Track::Backward ) ) continue;

      // Remove uninteresting tracks
      State& veloState = (*iTrack2) -> closestState( 0.0 );
      HepVector vec = veloState.stateVector();
      if ( .350 < fabs( vec[2] ) ) continue;
      if ( .300 < fabs( vec[3] ) ) continue;

      // Extrapolate veloTrack to the actual m_matchAtZPosition
      HepVector trackVector2;
      HepSymMatrix trackCov2;
      sc = extrapolate( *iTrack2, m_extrapolatorVelo,
                        m_matchAtZPosition, trackVector2, trackCov2);
      if ( sc.isFailure() ) continue;

      // Cut away badly reconstructed tracks
      if ( !m_allCombinations ) {
        if ( sqrt(trackCov2[0][0]) > m_veloXCut) continue;
        if ( sqrt(trackCov2[1][1]) > m_veloYCut) continue;
        if ( sqrt(trackCov2[2][2]) > m_veloTxCut) continue;
        if ( sqrt(trackCov2[3][3]) > m_veloTyCut) continue;
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
                          newZ, trackVector1, trackCov1);
        if ( sc.isFailure() ) continue;
        sc = extrapolate( *iTrack2, m_extrapolatorVelo,
                          newZ, trackVector2, trackCov2);
        if ( sc.isFailure() ) continue;
      }

      // Calculate the chi2 distance between 2 tracks
      double chi2 = 0.0;
      HepVector fitVector;
      sc = m_chi2Calculator->calculateChi2( trackVector1, trackCov1, 
                                            trackVector2, trackCov2,
                                            chi2 );
      if ( sc.isFailure() ) continue;

      // found a match -> add it to temporary matchList
      if ( chi2 < m_chi2MatchingCut ) {
        TrackMatch* matchTrack = new TrackMatch( *iTrack2, *iTrack1, chi2);
        matchVector.push_back( matchTrack );
      }
    }
  }

  // Sort the list by chi2 and make new list with the best combinations
  std::stable_sort( matchVector.begin(), matchVector.end(),
                    TrackMatchingDataFunctor::Less_by_Chi2<const TrackMatch*>() );

  int numCombinations = 0;
  for ( TrackMatches::const_iterator ipair = matchVector.begin() ;
        ipair != matchVector.end() ; ++ipair) {
    bool found = false ;
    for ( TrackMatches::const_iterator igoodpair = matches->begin() ;
          igoodpair != matches->end() && !found ; ++igoodpair) 
      found = (*ipair)->veloTrack() == (*igoodpair)->veloTrack() ||
              (*ipair)->seedTrack() == (*igoodpair)->seedTrack() ;
    if ( (!found || m_allCombinations) && numCombinations < 500 ) {
      matches->add(*ipair) ;
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
//  
//=============================================================================
StatusCode TrackMatchVeloSeed::addTTClusters( TrackMatches*& matchCont )
{
  // This function will add TT clusters to the matched tracks
  // First a velo track is extrapolated using the momentum of the 
  // seed track. Then the closest TT clusters are found.

  // Get the IT Clusters
  ITClusters* clusters = get<ITClusters>(ITClusterLocation::Default);
  if ( 0 == clusters){
    warning() << "Failed to find IT Clusters" << endmsg;
    return StatusCode::FAILURE;
  }

  // cache the first TT cluster of each wafer
  std::vector<ITClusters::iterator> cachedFirst(5000);
  ITClusters::iterator clusIter = clusters -> begin();
  ITClusters::iterator cachedIter = clusIter;
  unsigned int uniqueWafer = 0;
  ITChannelID aChan;
  unsigned int iStation;
  for ( iStation = 1; iStation <= 2; ++iStation ) {
    unsigned int iLayer;
    for ( iLayer = 1; iLayer <= 2; ++iLayer) {
      // find the first TT cluster of each layer
      aChan = ITChannelID(iStation,iLayer,0,0);
      clusIter = std::lower_bound( cachedIter, clusters->end(), aChan,
                     TrackMatchingDataFunctor::compByLayer_LB<const ITCluster*>());
      cachedFirst[ uniqueWafer + 1 ] = clusIter;
      STDetectionLayer* aLayer = m_itTracker->layer( aChan );
      unsigned int numWafers = aLayer->numWafers();
      unsigned int iWafer;
      for ( iWafer = 1; iWafer <= numWafers; ++iWafer) {
        // find the first TT cluster of each wafer
        aChan = ITChannelID(iStation,iLayer,iWafer,0);
        clusIter = std::lower_bound( cachedIter, clusters->end(), aChan,
                     TrackMatchingDataFunctor::compByWafer_LB<const ITCluster*>());
        // calculate the unique wafer ID. Should be done by ITChannelID!
        uniqueWafer = (aChan.uniqueWafer() >> 16) + 1;
        cachedFirst[ uniqueWafer ] = clusIter;
        cachedIter = clusIter;
      }
    }
  }
  // cache the last TT cluster
  aChan = ITChannelID(3,1,0,0);
  clusIter = std::lower_bound( cachedIter, clusters->end(), aChan,
                     TrackMatchingDataFunctor::compByLayer_LB<const ITCluster*>());
  cachedFirst[ uniqueWafer + 1 ] = clusIter;

  // Loop over matched tracks
  TrackMatches::const_iterator iterMatch;
  for ( iterMatch = matchCont->begin() ;
        iterMatch != matchCont->end(); ++iterMatch) {

    // Make a new State from the velo state plus momentum from seed track
    const Track* veloTrack = (*iterMatch)->veloTrack();
    const Track* seedTrack = (*iterMatch)->seedTrack();
    const State& veloState = veloTrack -> closestState(0.);
    const State& seedState = seedTrack->closestState(9900.);
    double z = veloState.z();
    HepVector stateVec(5);
    stateVec.sub(1,veloState.stateVector());
    stateVec(5) = seedState.qOverP();
    HepSymMatrix stateCov(5,1);
    stateCov.sub(1,veloState.covariance());
    stateCov(5,5) = pow (0.015 * stateVec(5), 2.);
    State* state = new State();
    state -> setZ( z );
    state -> setState( stateVec );
    state -> setCovariance( stateCov );

    // The vector of pointers to TT candidate hit-clusters
    TTCandidates candidates;

    // loop over layers in TT and find the wafer which is hit
    std::vector<ITCluster*> clusVector;
    unsigned int iStation;
    for ( iStation = 1; iStation <= 2; ++iStation) {
      unsigned int iLayer;
      for ( iLayer = 1; iLayer <= 2; ++iLayer) {

        // For this TT layer: extrapolate the new State to the z of the layer
        aChan = ITChannelID( iStation, iLayer, 0, 0 );
        STDetectionLayer* aLayer = m_itTracker->layer( aChan );
        StatusCode sc =
          m_extrapolatorSeed -> propagate( *state, aLayer -> z(), m_particleID );
        if ( sc.isFailure() ) { 
          debug() << "extrapolation of state to z=" << aLayer->z()
              << " failed" << endmsg;
          continue;
        }

        // calculate the layer number (0-3)
        unsigned int layerNum = 2*iStation+iLayer-3;

        // calculate the u and v coordinates
        double x = state->x();
        double y = state->y();
        const double layerSin = aLayer->sinAngle();
        const double layerCos = aLayer->cosAngle();
        double u = x*layerCos + y*layerSin;
        double v = y*layerCos - x*layerSin;

        // loop over all wafer and check if prediction is inside wafer
        unsigned int iWafer = 1;
        while ( iWafer <= aLayer->numWafers() ) {
          const STWafer* aWafer = aLayer->wafer( iWafer );
          if ( aWafer->isInside(u,v) ) {
            // loop over all clusters using the cached cluster vector
            aChan = ITChannelID(iStation, iLayer, iWafer, 0 );
            uniqueWafer = (aChan.uniqueWafer() >> 16) + 1;
            ITClusters::iterator iClus;
            for ( iClus = cachedFirst[uniqueWafer]; 
                  iClus != cachedFirst[uniqueWafer+1]; ++iClus ) {
              // calculate the distance
              ITChannelID clusID = (*iClus)->channelID();
              double uClus = aWafer->U( clusID.strip() ) +
                (*iClus)->distToStripCenter();
              double distance = uClus - u;
              
              if ( fabs( distance ) < m_ttClusterCut ) {
                bool clusterUsed = false;
                unsigned int iCand = 0;
                unsigned int numCandidates = candidates.size();
                while ( iCand < numCandidates ) {
                  TTCandidate* cand = candidates[iCand];
                  if ( cand->lastLayer() < layerNum ) {
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
            } // loop iClus
          } // isInside
          ++iWafer;
        } // loop iWafer

        // delete all dead candidates and those with too few hits
        TTCandidates::iterator iCand = candidates.begin();
        while ( iCand != candidates.end() ) {
          bool tooFewHits = int( (*iCand)->numTTClusters() - m_minTTHits + 
                                 m_numTTLayers - layerNum ) < 1;
          if ( (*iCand)->isDead() || tooFewHits ) {
            delete *iCand;
            iCand = candidates.erase( iCand );
          } else ++iCand;
        }
      } // loop iLayer
    } // loop iStation
    delete state;

    // find the best TT candidate
    double bestQuality = 2.*m_ttClusterCut;
    std::vector<ITCluster*> bestCand;
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
//  
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

  // fill output container
  TrackMatches::const_iterator iterMatch;
  for ( iterMatch = matchCont->begin() ;
        iterMatch != matchCont->end(); ++iterMatch ) {

    const Track* veloTrack = (*iterMatch) -> veloTrack();
    const Track* seedTrack = (*iterMatch) -> seedTrack();
    Track* aTrack = new Track();

    // copy velo hits
    const std::vector<Measurement*>& vmeasures = veloTrack -> measurements();
    std::vector<Measurement*>::const_iterator iMeasure = vmeasures.begin();
    while ( iMeasure != vmeasures.end() ) {
      // copy measure to this
      aTrack -> addToMeasurements( *(*iMeasure) );
      // next measure
      ++iMeasure;
    }

    //copy tt hits
    std::vector<ITCluster*> ttClusters = (*iterMatch)->ttClusters();
    std::vector<ITCluster*>::const_iterator iClus = ttClusters.begin();
    while ( iClus != ttClusters.end() ) {
      // make a new ITClusterOnTrack
      ITMeasurement* ttMeas = new ITMeasurement( *(*iClus), *m_itTracker );
      aTrack -> addToMeasurements( *ttMeas);  //  addToMeasurements clones and owns
      delete ttMeas;
      ++iClus;
    }
    // copy seed hits
    const std::vector<Measurement*>& smeasures = seedTrack -> measurements();
    std::vector<Measurement*>::const_iterator jMeasure = smeasures.begin();
    while ( jMeasure != smeasures.end() ) {
      // copy measure to this
      aTrack -> addToMeasurements( *(*jMeasure) );
      // next measure
      ++jMeasure;
    }

    // initial parameters
    std::vector<Measurement*>::const_iterator lastMeas =
      seedTrack -> measurements().end();
    --lastMeas;
    const State& closestState = seedTrack -> closestState( (*lastMeas)->z() );
    State* aState = closestState.clone();
    sc = m_extrapolatorSeed -> propagate( *aState, (*lastMeas)->z(), m_particleID );
    if ( sc.isFailure() ) {
      debug() << "extrapolation of state to z = "
              << (*lastMeas)->z() << " failed" << endmsg;
      delete aState;
      delete aTrack;
      continue;
    }

    // Blow up covariance matrix
    HepSymMatrix newC(5, 1);
    HepSymMatrix& tC = aState -> covariance();
    HepVector stateVector = aState -> stateVector();
    tC = newC;
    tC.fast(1,1) *= m_errorX2;
    tC.fast(2,2) *= m_errorY2;
    tC.fast(3,3) *= m_errorTx2;
    tC.fast(4,4) *= m_errorTy2;
    tC.fast(5,5) = pow( stateVector(5)*m_errorP, 2);

    aTrack -> addToStates( *aState );
    aTrack -> setType( Track::Long );
    aTrack -> setHistory( Track::TrackMatching );
    trackCont -> add( aTrack );
    delete aState;
  } // iterMatch

  // Here the new tracks are registered
  sc = put( trackCont, TrackLocation::Match );
  if( sc.isFailure() ) {
    delete trackCont;
    error() << "Unable to register the output container "
            << TrackLocation::Match << ". Status is " << sc.getCode() << endreq;
    return sc ;
  }
  else {
    debug() << "Output container " << TrackLocation::Match
            << " registered" << endreq;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode TrackMatchVeloSeed::extrapolate( Track* track,
                                            ITrackExtrapolator* extrapolator,
                                            double zpos,
                                            HepVector& trackVector,
                                            HepSymMatrix& trackCov )
{
  State tmpState;
  StatusCode sc = extrapolator -> propagate( *track,
                                             zpos,
                                             tmpState,
                                             m_particleID );
  if ( sc.isFailure() ) return sc;
  trackVector = tmpState.stateVector();
  trackCov    = tmpState.covariance();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
StatusCode TrackMatchVeloSeed::determineZ( double tX,
                                           double& zNew )
{
  if ( m_varZParameters.size() == 3 ) {
    zNew = m_matchAtZPosition ;
    zNew += m_varZParameters[0] +
            m_varZParameters[1] * tX +
            m_varZParameters[2] * pow(tX,2);
  }
  else {
    zNew = m_matchAtZPosition ;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
