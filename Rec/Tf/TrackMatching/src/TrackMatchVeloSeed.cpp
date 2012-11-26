// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from GSL
#include "gsl/gsl_math.h"

// from TrackEvent
#include "Event/State.h"
#include "Event/StateParameters.h"
#include "Event/Node.h"

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
#include <boost/foreach.hpp>

#include "Event/ProcStatus.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackMatchVeloSeed
//
// 2001-05-16 : J. van Tilburg Jeroen.van.Tilburg@cern.ch
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( TrackMatchVeloSeed )

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

  declareProperty( "Chi2MatchingCut",  m_chi2MatchingCut =900.0 );
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
  declareProperty("Chi2SeedCut", m_chi2SeedCut = 25.);
  declareProperty("LikCut", m_likCut = -35.0);
  declareProperty("referenceT", m_refT = 7500.);
  declareProperty("maxStepSize" , m_maxStepSize = 1500.);
  declareProperty("chi2TCut", m_chi2TCut = 200);
  {
    std::vector<double> tmp = boost::assign::list_of(gsl_pow_2(1.6))(gsl_pow_2(6.9e4));
    declareProperty("yTParams", m_yTParams = tmp);
  }
  {
    std::vector<double> tmp = boost::assign::list_of(gsl_pow_2(1.6e-3))(gsl_pow_2(20));
    declareProperty("tyTParams", m_tyTParams = tmp);
  }
  declareProperty( "ExtrapolatorVelo",
                   m_extrapolatorVeloName = "TrackLinearExtrapolator" );
  declareProperty( "ExtrapolatorSeed",
                   m_extrapolatorSeedName = "TrackHerabExtrapolator" );
  
  declareProperty("TTClusterToolName", m_ttClusterToolName = "PatAddTTCoord" );

  declareProperty( "outputContainer",  m_outputLocation = TrackLocation::Match );

  declareProperty("nUseT", m_nUseT = 0u);
  declareProperty("nUseVelo", m_nUseVelo = 0u);
  
  declareProperty("OmitSeedFitOutliers", m_omitSeedFitOutliers = false) ;
  /*switch on or off NN var. writing*/
  declareProperty( "writeNNVariables", m_writeNNVariables = true);

  declareProperty( "DiscardUsedVelo"      , m_discardUsedVelo    = false      );
  declareProperty( "DiscardChi2"         , m_discardChi2    = 1.5      );

  declareProperty("maxNVelo", m_maxNVelo = 1000);
  declareProperty("maxNSeed", m_maxNSeed = 2000);
  
  declareProperty( "TimingMeasurement", m_doTiming = false);
 
}
//=============================================================================
// Destructor
//=============================================================================
TrackMatchVeloSeed::~TrackMatchVeloSeed() {}

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
  m_measProvider = tool<IMeasurementProvider>( "MeasurementProvider","MeasProvider",this );

  m_usedVelo.clear();
  m_usedT.clear();

  if (m_tyTParams.empty() || m_yTParams.empty()){
    return Error("Failed to initialize: Empty parameter vector !", StatusCode::FAILURE);
  }
  

  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timerTool->increaseIndent();
    m_matchTime = m_timerTool->addTimer( "Internal TrackMatching" );
    m_timerTool->decreaseIndent();
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackMatchVeloSeed::execute()
{
  if ( m_doTiming ) m_timerTool->start( m_matchTime );

  // Retrieve velo and seed tracks from EvDS
  Tracks* veloTracks = get<Tracks>( m_veloTracks );
  Tracks* seedTracks = get<Tracks>( m_seedTracks );

  // create and register the container for the matched tracks
  Tracks* matches = new Tracks();
  matches->reserve(veloTracks->size());
  put( matches, m_outputLocation);
  
  //  plot(veloTracks->size(), "v size", 0, 1000, 100);
  // plot(seedTracks->size(), "s size", 0, 1000, 100);


  if (veloTracks->size() > m_maxNVelo) {
     LHCb::ProcStatus* procStat =
	getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(
	    LHCb::ProcStatusLocation::Default);
      // give some indication that we had to skip this event
      // (ProcStatus returns zero status for us in cases where we don't
      // explicitly add a status code)
     procStat->addAlgorithmStatus(name(), "Tracking", "LimitOfVeloTracksExceeded", -3 , true );
     return Warning("Too many velo tracks", StatusCode::SUCCESS, 1);
  }

  if (seedTracks->size() > m_maxNSeed) {
        LHCb::ProcStatus* procStat =
	getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(
	    LHCb::ProcStatusLocation::Default);
      // give some indication that we had to skip this event
      // (ProcStatus returns zero status for us in cases where we don't
      // explicitly add a status code)
      procStat->addAlgorithmStatus(name(), "Tracking", "LimitOfSeedTracksExceeded", -3 , true );
    return Warning("Too many seed tracks", StatusCode::SUCCESS, 1);
  }

  // make a vector of selected velo tracks
  VeloCandidates selectedVelo; selectedVelo.reserve(veloTracks->size());
  createVeloCandidates(veloTracks,selectedVelo);  

  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "retrieved succesfully " << veloTracks -> size()
            << " velo tracks selected " << selectedVelo.size() << endmsg;
    debug() << "retrieved succesfully " << seedTracks -> size()
            << " seed tracks" << endmsg;
  }
  
  
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

  if ( m_doTiming) m_timerTool->stop( m_matchTime );

  return StatusCode::SUCCESS;
}

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

    StatusCode sc = extrapolateSeed(stateAtT, matchZ);
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

      // plot(chi2T, "chiT" ,  0., 1000., 100);
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
      // plot( (*ipair)->chi2(), "chi2Match", 0., 1000, 100);
    }
  } // loop matches
  if( m_writeNNVariables == true){
    /*************************************************************************/
    /*added for NNTools; Adrian Perieanu*/
    Tracks::const_iterator imatch;
    for( imatch = matches->begin(); imatch != matches->end(); ++imatch){
      const SmartRefVector<Track>& Ancestors = (*imatch)->ancestors();
      const Track* veloTrack = Ancestors.front();
      const Track* seedTrack = Ancestors.back();
      /*define counter for candidates*/
      int cand_counter = 0;
      float cand1stchi2 = 0.;
      float cand2ndchi2 = 0.;
      /*loop over matchVector*/
      TrackMatches::const_iterator iall;
      for( iall = matchVector.begin(); iall != matchVector.end(); ++iall){
        if((*iall)->veloTrack() == veloTrack ||
           (*iall)->seedTrack() == seedTrack){
          ++cand_counter;

          if(cand_counter == 1){
            cand1stchi2   = (float) (*iall)->chi2();
          }else if(cand_counter == 2){
            cand2ndchi2   = (float) (*iall)->chi2();
          }
        }
      }/*end loop over matchVector*/
      (*imatch)->addInfo(LHCb::Track::NCandCommonHits, cand_counter);
      (*imatch)->addInfo(LHCb::Track::Cand1stChi2Mat, cand1stchi2);
      (*imatch)->addInfo(LHCb::Track::Cand2ndChi2Mat, cand2ndchi2);
    }/*end loop over matches*/
    /*************************************************************************/
  }
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

  if ( msgLevel(MSG::DEBUG) ) debug() << "Registering the tracks ..." << endmsg;

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
    
    // Search for tt hits
    if ( m_addTTClusters ) {  
      StatusCode sc = addTTClusters( aTrack );
      if ( sc.isFailure() )
        return Error( "Unable to add TT Clusters to matched tracks" );
    }

    // Copy seed hits (= LHCbIDs) -- if track is not fitted
    if (seedTrack->fitStatus() == Track::FitStatusUnknown ||
	!m_omitSeedFitOutliers){
      const std::vector<LHCbID>& sids = seedTrack -> lhcbIDs();
      std::vector<LHCbID>::const_iterator itsids = sids.begin();
      while ( itsids != sids.end() ) {
        aTrack -> addToLhcbIDs( (*itsids) );
        ++itsids;
      }
    }
    else {
      BOOST_FOREACH( const LHCb::Node* node, seedTrack->nodes() ) 
	if( node->hasMeasurement() && node->type() != LHCb::Node::Outlier ) 
	  aTrack -> addToLhcbIDs( node->measurement().lhcbID() ) ;
    }

    // copy all the states from the seed
    const std::vector<State*>& seedStates = seedTrack->states();
    for (std::vector<State*>::const_iterator iterS = seedStates.begin(); iterS != seedStates.end(); ++iterS){ 
      State aState = **iterS;
      TrackSymMatrix newC;
      aState.setCovariance( newC );
      aTrack -> addToStates( aState );
    } // iterS
 
    // Add state at Velo
    const State* vStateP = veloTrack->stateAt( LHCb::State::EndVelo ) ;
    if (!vStateP) return Warning("No State at Velo", StatusCode::FAILURE);
    State vState = *vStateP ;
    vState.setQOverP(seedTrack->states().front()->qOverP());
    aTrack -> addToStates( vState );

    // Set various flags
    aTrack -> setType( Track::Long );
    aTrack -> setHistory( Track::TrackMatching );
    aTrack -> setPatRecStatus( Track::PatRecIDs  );

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
  const State& seedState = track->closestState( StateParameters::ZEndT );
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

  LHCb::Track::Range fwdTracks ;
  if(m_discardUsedVelo) fwdTracks = get<LHCb::Track::Range>( LHCb::TrackLocation::Forward );
  
  for (Tracks::iterator iterTrack = tracks->begin(); iterTrack != tracks->end(); ++iterTrack) {
    
    Track* aTrack = *iterTrack;
    
    // Check on Velo tracks used in PatForward tracks
    bool veloUsed = false;
    if (m_discardUsedVelo) {
      for ( LHCb::Track::Range::const_iterator itTF = fwdTracks.begin();
            fwdTracks.end() != itTF; ++itTF ) {
        // check for good PatFwd tracks
        // from PatForward fit
        if ((*itTF)->fitStatus() == LHCb::Track::Fitted) {
          // if fitted - used chi2pdf from fit
          if ((*itTF)->chi2PerDoF() > m_discardChi2) continue;
        }
        //else {
        // otherwise use PatQuality from patreco
        //if ((*itTF)->info(LHCb::Track::PatQuality, -1.) > m_discardPatQual) continue;
        //}
        const SmartRefVector<LHCb::Track>& ancestors = (*itTF)->ancestors();
        for ( SmartRefVector<LHCb::Track>::const_iterator itA = ancestors.begin();
              ancestors.end() != itA; ++itA ) {
          const LHCb::Track* velo = (*itA);
          if ( aTrack==velo )  {
            if(msgLevel( MSG::DEBUG ))
              debug() << "Velo track " << aTrack->key()
                      << " is used in good PatFwd, skipping" << endreq;
            veloUsed = true;
          }
        }
      }
    }

    if (veloUsed) continue;


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
  // plot(aTrack->chi2PerDoF(),"chi2",  0., 100., 200);
  if (aTrack->chi2PerDoF() > m_chi2SeedCut ) return false;
  
  // check if after outlier removal there are at least two x and two 
  // stereo hits int two diff. layer
  if (m_omitSeedFitOutliers){
    
    bool first_x = true;
    bool first_stereo = true;
    bool second_x = false;
    bool second_stereo = false;
    int layer_x = -999;
    int layer_stereo = -999;
    int station_x = -999;
    int station_stereo = -999;

    BOOST_FOREACH( const LHCb::Node* node, aTrack->nodes() ) 
      if( node->hasMeasurement() && node->type() != LHCb::Node::Outlier ){
	
	LHCb::LHCbID ihit = node->measurement().lhcbID() ;
	
	int layer = -1;
	int station = -1;
	
	if ( ihit.detectorType()==LHCb::LHCbID::IT ) {
	  layer = ihit.stID().layer();
	  station = ihit.stID().station();
	} else if (ihit.detectorType()==LHCb::LHCbID::OT){
	  layer = ihit.otID().layer()+1;
	  station = ihit.otID().station(); 
	} else
	  return false;

	if (layer == 1 || layer == 4) {
	  if (first_x){
	    layer_x = layer;
	    station_x = station;
	    first_x = false;
	  } else 
	    if (layer_x != layer || station_x != station)
	      second_x = true;
	} else {
	  if (first_stereo){
	    layer_stereo = layer;
	    station_stereo = station;
	    first_stereo = false;
	  } else 
	    if (layer_stereo != layer || station_stereo != station)
	      second_stereo = true;
	}
      }
    if (!second_x || !second_stereo)
      return false;
  }

  //plot(aTrack->info(Track::TsaLikelihood, -9999.), "likelihood", -50., 0 );

  // remove seeds with bad likelihood
  if (aTrack->info(Track::TsaLikelihood, -9999.) < m_likCut ) return false;

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



