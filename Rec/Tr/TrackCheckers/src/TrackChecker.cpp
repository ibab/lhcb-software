// $Id: TrackChecker.cpp,v 1.7 2006-05-22 10:42:10 erodrigu Exp $
// Include files 

// local
#include "TrackChecker.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IRegistry.h"

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"

// from Event/TrackEvent
#include "Event/Track.h"
#include "Event/TrackFunctor.h"

// from Event/MCEvent
#include "Event/MCParticle.h"

// from Tr/TrackFitEvent
#include "Event/STMeasurement.h"
#include "Event/OTMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"

using namespace Gaudi;
using namespace Gaudi::Units;
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackChecker );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackChecker::TrackChecker( const std::string& name,
                            ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg( name , pSvcLocator ) {

  // default z-positions
  m_zPositions.clear();
  m_zPositions.push_back(   990.0 );
  m_zPositions.push_back(  2165.0 );
  m_zPositions.push_back(  9450.0 );
  m_zPositions.push_back( 11900.0 );

  declareProperty( "TracksInContainer",
                   m_tracksInContainer = TrackLocation::Default );
  declareProperty( "LinkerInTable", m_linkerInTable = "" );
  declareProperty( "ZPositions",    m_zPositions         );
  declareProperty( "TrackSelector",
                   m_trackSelectorName = "TrackCriteriaSelector" );
}

//=============================================================================
// Destructor
//=============================================================================
TrackChecker::~TrackChecker() {}; 

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackChecker::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiHistoAlg::initialize();
  if( sc.isFailure() ) { return sc; }

  // Set the path for the linker table Track - MCParticle
  if ( m_linkerInTable == "" ) m_linkerInTable = m_tracksInContainer;

  // Set counters
  m_nTracks         = 0;
  m_nMCTracks       = 0;
  m_nAsctTracks     = 0;
  m_nAsctMCTracks   = 0;
  m_evtAveEff       = 0.;
  m_err2EvtAveEff   = 0.;
  m_evtAveGhost     = 0.;
  m_err2EvtAveGhost = 0.;
  m_nMCEvt          = 0;
  m_nEvt            = 0;

  m_trackSelector = tool<ITrackCriteriaSelector>( m_trackSelectorName,
                                                  "TrackSelector", this );
  
  m_stateCreator  = tool<IIdealStateCreator>( "IdealStateCreator"       );
  m_extrapolatorL = tool<ITrackExtrapolator>( "TrackLinearExtrapolator" );
  m_extrapolatorM = tool<ITrackExtrapolator>( "TrackMasterExtrapolator" );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackChecker::execute()
{  
  // Set local counters
  int nTracks = 0;        //< # selected Tracks
  int nMCTracks = 0;      //< # selected MCParticles
  int nAsctTracks = 0;    //< # selected Tracks with a MCParticle associated
  // # selected MCParticles with a selectByTrackType Track associated
  int nAsctMCTracks = 0;

  // Get the Tracks
  Tracks* tracks = get<Tracks>( m_tracksInContainer );

  // Get the MCParticles
  MCParticles* particles = get<MCParticles>( MCParticleLocation::Default );
  
  // Retrieve the Linker table
  LinkedTo<MCParticle, Track> directLink( evtSvc(), msgSvc(), m_linkerInTable );
  if( directLink.notFound() ) {
    error() << "Linker table not found" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve the reverse direction Linker table
  LinkedFrom<Track,MCParticle> reverseLink( evtSvc(), msgSvc(), m_linkerInTable );

  // Loop over the Tracks
  Tracks::const_iterator iTrack;
  for( iTrack = tracks->begin(); iTrack != tracks->end(); ++iTrack ) {
    Track* track = *iTrack;
    // Decde whether the Track will be checked
    if( m_trackSelector->select( track ) ) {
      ++nTracks;
      plot1D( track->type(), 12, "Track type", -0.5, 7.5, 8 );
      double momentum = ( track->firstState() ).p();
      plot1D( momentum/GeV, 30, "Momentum (GeV) at first state", -1.0, 101.0, 51 ); 
      // Get MCParticle linked by highest weight to Track
      MCParticle* mcPart = directLink.first( track );
      if( NULL != mcPart ) {
        ++nAsctTracks;
        resolutionHistos( track, mcPart );
        purityHistos( track, mcPart );
      }
    }
  } // End loop over Tracks
  
  // Loop over the MCParticles
  MCParticles::const_iterator iPart;
  for( iPart = particles->begin(); particles->end() != iPart; ++iPart ) {
    MCParticle* particle = *iPart;
    // Decide whether the MCParticle will be checked
    if( m_trackSelector->select( particle ) ) {
      ++nMCTracks;
      // Fill the general histograms
      plot1D( particle->momentum().mag() / GeV,
              33, "True momentum (GeV) for MCParticles", -1.0, 101.0, 51 );
      Track* track = reverseLink.first( particle );
      if( NULL != track ) {
        bool found = false;
        while( !found ) {
          if( m_trackSelector->selectByTrackType( track ) ) {
            found = true;
            ++nAsctMCTracks;
          }
          // If FALSE, try next Track linked to the MCParticle
          else { track = reverseLink.next(); }
          // Break from while loop when there is no next Track linked
          if( NULL == track ) { break; }
        }
      }
    }
  } // End loop over MCParticles

  // Fill number of tracks histos
  plot1D( nTracks, 1, "Number of Tracks per Event", -1., 201., 101 );
  plot1D( nMCTracks, 2, "Number of true Tracks per Event", -1., 201., 101 );

  // Fill global counters
  m_nTracks += nTracks;          //< Total # of selected Tracks
  m_nMCTracks += nMCTracks;      //< Total # of selected MCParticles
  // Total # of selected Tracks with a MCParticle associated
  m_nAsctTracks += nAsctTracks;
  // Total # of selected MCParticles with a selectByTrackType Track associated
  m_nAsctMCTracks += nAsctMCTracks;

  // Counters for event averaged efficiency
  if( nMCTracks != 0 ) {
    ++m_nMCEvt;
    // Fraction of selected MCParticles with a selectByTrackType Track associated
    double evtAveEff = double( nAsctMCTracks ) / double( nMCTracks );
    double err2EvtAveEff = nAsctMCTracks * ( 1.0 - evtAveEff ) /
      ( nMCTracks * nMCTracks );
    m_evtAveEff = ( m_evtAveEff * ( m_nMCEvt - 1 ) + evtAveEff ) / m_nMCEvt;
    m_err2EvtAveEff = ( m_err2EvtAveEff * ( m_nMCEvt - 1 ) * ( m_nMCEvt - 1 )
                        + err2EvtAveEff ) / ( m_nMCEvt * m_nMCEvt );
    plot1D( evtAveEff, 20, "Track efficiency per Event", -0.01, 1.01, 51 );
  }

  // For event averaged ghost rate
  if( nTracks != 0 ) {
    ++m_nEvt;
    double evtAveGhost = 1.0 - double( nAsctTracks ) / double( nTracks );
    double err2EvtAveGhost = nAsctTracks * evtAveGhost / ( nTracks * nTracks );
    m_evtAveGhost = ( m_evtAveGhost * ( m_nEvt - 1 ) + evtAveGhost ) / m_nEvt;
    m_err2EvtAveGhost = ( m_err2EvtAveGhost * ( m_nEvt - 1 ) * ( m_nEvt - 1 )
                          + err2EvtAveGhost ) / ( m_nEvt * m_nEvt );
    plot1D( evtAveGhost, 21, "Track ghost rate per Event", -0.01, 1.01, 51 );
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackChecker::finalize()
{
  // Calculate the track averaged efficiency and ghost rate
  double trackAveEff = double( m_nAsctMCTracks ) / double( m_nMCTracks );
  double errTrackAveEff = sqrt( m_nAsctMCTracks * ( 1. - trackAveEff ) ) /
    m_nMCTracks;
  double trackAveGhost = 1.0 - double( m_nAsctTracks ) / double( m_nTracks );
  double errTrackAveGhost = sqrt( m_nAsctTracks * trackAveGhost ) / m_nTracks;
  
  // print out efficiency and ghost rate
  info() << "Track averaged:" << endreq;
  info() << "Track efficiency = "
         << format( "%5.1f +/- %3.1f %%",
                    100.0 * trackAveEff, 100.0 * errTrackAveEff )
         << "  (=" << m_nAsctMCTracks << "/" <<  m_nMCTracks << ")" << endreq;
  info() << "Ghost rate =       "
         << format( "%5.1f +/- %3.1f %%",
                    100.0 * trackAveGhost, 100.0 * errTrackAveGhost )
         << "  (=" << m_nTracks-m_nAsctTracks << "/" << m_nTracks << ")"
         << endreq;
  info() << "Event averaged:" << endreq;
  info() << "Track efficiency = "
         << format( "%5.1f +/- %3.1f %%",
                    100.0 * m_evtAveEff, 100. * sqrt(m_err2EvtAveEff) )
         << endreq;
  info() << "Ghost rate =       "
         << format( "%5.1f +/- %3.1f %%",
                    100.0*m_evtAveGhost, 100.*sqrt(m_err2EvtAveGhost) )
         << endreq;
  
  return GaudiHistoAlg::finalize();
}

//=============================================================================
//
//=============================================================================
StatusCode TrackChecker::resolutionHistos( Track* track, MCParticle* mcPart )
{
  // Resolutions and pulls at true vertex position
  // Get true values at vertex
  const MCVertex* vOrigin = mcPart->originVertex();
  if( 0 != vOrigin ) {
    XYZPoint vertPos = vOrigin->position();
    LorentzVector pVec = mcPart->momentum();
    LorentzVector mcVec( pVec / pVec.z() );
    
    // Find closest state to true vertex position
    State& vtxState = track->closestState( vertPos.z() );
    
    // Choose Extrapolator
    ITrackExtrapolator* extrap;
    if( (vtxState.z() < 1000*mm) ) { extrap = m_extrapolatorL; }
    else { extrap = m_extrapolatorM; }
    
    // Extrapolate closest state to true vertex z-position
    StatusCode sc = extrap->propagate( vtxState, vertPos.z() );
    if( !sc.isFailure() ) {
      // Update the vtxState
      TrackVector vec = vtxState.stateVector();
      TrackSymMatrix cov = vtxState.covariance();

      // calculate impact parameter vector
      XYZVector dist( vec(0) - vertPos.x(), vec(1) - vertPos.y(), 0.0 );
      XYZVector trackVec( vec(2), vec(3), 1 );
      XYZVector ipVector = dist.Cross( trackVec ) / sqrt(trackVec.mag2());

      // fill the histograms
      plot1D( dist.x(), 101, "X resolution at vertex", -0.5, 0.5, 50 );
      plot1D( dist.y(), 102, "Y resolution at vertex", -0.5, 0.5, 50 );
      plot1D( vec(2) - mcVec.x(), 103, "Tx resolution at vertex", -0.01, 0.01, 50 );
      plot1D( vec(3) - mcVec.y(), 104, "Ty resolution at vertex", -0.01, 0.01, 50 );
      plot1D( 1.0/( fabs(vec(4)) * pVec.mag() ) - 1.0,
              105, "Momentum resolution dp/p at vertex", -0.05, 0.05, 50 );
      plot1D( sqrt(ipVector.mag2()), 106, "Impact parameter at vertex", -0.01, 1.01, 51 );
      plot1D( dist.x() / sqrt(cov(0,0)), 111, "X pull at vertex", -5.0, 5.0, 50 );
      plot1D( dist.y() / sqrt(cov(1,1)), 112, "Y pull at vertex", -5.0, 5.0, 50 );
      plot1D( (vec(2) - mcVec.x()) / sqrt(cov(2,2)),
              113, "Tx pull at vertex", -5.0, 5.0, 50 );
      plot1D( (vec(3) - mcVec.y()) / sqrt(cov(3,3)),
              114, "Ty pull at vertex", -5.0, 5.0, 50 );
      plot1D( ( fabs(vec(4)) - 1.0/pVec.mag() ) / sqrt(cov(4,4)),
              115, "Momentum pull q/p at vertex", -5.0, 5.0, 50 );
    }
  }

  // Resolutions and pulls at 1st measurement
  double zAt1stMeas = track->measurements().front()->z();
  // find closest state to z at first measurement
  State& stateAt1stMeas = track->closestState( zAt1stMeas );

  if( fabs( stateAt1stMeas.z() - zAt1stMeas ) < 10.0*mm ) {
    // get the true state
    State* trueState;
    StatusCode sc =
      m_stateCreator->createState( mcPart, stateAt1stMeas.z(), trueState );
    if( sc.isSuccess() ) {
      TrackVector vec = stateAt1stMeas.stateVector();
      TrackVector trueVec = trueState->stateVector();
      TrackSymMatrix cov = stateAt1stMeas.covariance();
      double dx = vec(0) - trueVec(0);
      double dy = vec(1) - trueVec(1);
      double dtx = vec(2) - trueVec(2);
      double dty = vec(3) - trueVec(3);
      // fill the histograms
      plot1D( dx, 201, "X resolution at 1st measurement", -0.5, 0.5, 50 );
      plot1D( dy, 202, "Y resolution at 1st measurement", -0.5, 0.5, 50 );
      plot1D( dtx, 203, "Tx resolution at 1st measurement", -0.01, 0.01, 50 );
      plot1D( dty, 204, "Ty resolution at 1st measurement", -0.01, 0.01, 50 );
      plot1D( dx / sqrt(cov(0,0)), 211,"X pull at 1st measurement", -5.0, 5.0, 50 );
      plot1D( dy / sqrt(cov(1,1)), 212,"Y pull at 1st measurement", -5.0, 5.0, 50 );
      plot1D( dtx / sqrt(cov(2,2)), 213,"Tx pull at 1st measurement", -5.0, 5.0, 50 );
      plot1D( dty / sqrt(cov(3,3)), 214,"Ty pull at 1st measurement", -5.0, 5.0, 50 );
    }
  }
    
  // Resolutions and pulls at certain z-positions
  int numPos = 0;
  std::vector<double>::const_iterator iZpos;
  for( iZpos = m_zPositions.begin(); iZpos != m_zPositions.end(); ++iZpos ) {
    // find closest state to z-position
    State& state = track->closestState( *iZpos );

    if( fabs( state.z() - ( *iZpos ) ) < 10.0*mm ) {
      // get the true state
      State* trueState;
      StatusCode sc =
        m_stateCreator->createState( mcPart, state.z(), trueState );
      if( sc.isSuccess() ) {
        TrackVector vec = state.stateVector();
        TrackVector trueVec = trueState->stateVector();
        TrackSymMatrix cov = state.covariance();
        double dx = vec(0) - trueVec(0);
        double dy = vec(1) - trueVec(1);
        double dtx = vec(2) - trueVec(2);
        double dty = vec(3) - trueVec(3);
        // fill the histograms
        int ID = 300 + 100 * numPos;
        std::string title = format( " at z=%d mm", int( *iZpos ) );
        plot1D( dx, ID+1, "x resolution"+title, -0.5, 0.5, 50 );
        plot1D( dy, ID+2, "y resolution"+title, -0.5, 0.5, 50 );
        plot1D( dtx, ID+3, "tx resolution"+title, -0.01, 0.01, 50 );
        plot1D( dty, ID+4, "ty resolution"+title, -0.01, 0.01, 50 );
        plot1D( dx / sqrt(cov(0,0)), ID+11,"x pull"+title, -5.0, 5.0, 50 );
        plot1D( dy / sqrt(cov(1,1)), ID+12,"y pull"+title, -5.0, 5.0, 50 );
        plot1D( dtx / sqrt(cov(2,2)), ID+13,"tx pull"+title, -5.0, 5.0, 50 );
        plot1D( dty / sqrt(cov(3,3)), ID+14,"ty pull"+title, -5.0, 5.0, 50 );
      }
    }
    ++numPos;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
StatusCode TrackChecker::purityHistos( Track* track, MCParticle* mcPart )
{
  // fill hit purity and hit efficiency histograms
  // get VeloClusters and count correct and total number of clusters

  // Count number of Measurements of types
  int nTotalVelo = 0;
  int nTotalIT = 0;
  int nTotalOT = 0;
  std::vector<Measurement*>::const_iterator itm;
  for( itm = track->measurements().begin();
       track->measurements().end() != itm; ++itm ) {
    if( (*itm)->type() == Measurement::VeloPhi ) { ++nTotalVelo; }
    else if( (*itm)->type() == Measurement::VeloR ) { ++nTotalVelo; }
    else if( (*itm)->type() == Measurement::TT ) { ++nTotalIT; }
    else if( (*itm)->type() == Measurement::IT ) { ++nTotalIT; }
    else if( (*itm)->type() == Measurement::OT ) { ++nTotalOT; }
  }

  // get VeloClusters and count correct and total number of clusters
  // Get the linker table MCParticle => VeloCluster
  LinkedFrom<VeloCluster,MCParticle> veloLink(evtSvc(),msgSvc(),
                                              LHCb::VeloClusterLocation::Default);
  if( veloLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to VeloCluster linker table." << endreq;
    return StatusCode::FAILURE;
  }
  
  int nGoodVelo = 0;
  int nMCTotalVelo = 0;
  
  const VeloCluster* veloCluster = veloLink.first( mcPart );
  bool found = false;
  while( 0 != veloCluster ) {
    ++nMCTotalVelo;
    std::vector<Measurement*>::const_iterator iMeas = track->measurements().begin();
    while( !found && iMeas != track->measurements().end() ) {
      if( (*iMeas)->type() == Measurement::VeloR ) {
        VeloRMeasurement* meas = dynamic_cast<VeloRMeasurement*>( *iMeas );
        found = ( veloCluster == meas->cluster() );
      }
      if( (*iMeas)->type() == Measurement::VeloPhi ) {
        VeloPhiMeasurement* meas = dynamic_cast<VeloPhiMeasurement*>( *iMeas );
        found = ( veloCluster == meas->cluster() );
      }
      ++iMeas;
    }
    if( found ) { ++nGoodVelo; }
    veloCluster = veloLink.next();
  }

  // get STClusters and count correct and total number of clusters
  // Get the linker table MCParticle => TTCluster
  LinkedFrom<STCluster,MCParticle> ttLink(evtSvc(),msgSvc(),
                                          LHCb::STClusterLocation::TTClusters);
  if( ttLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to TTCluster linker table." << endreq;
    return StatusCode::FAILURE;
  }
  // Get the linker table MCParticle => ITCluster
  LinkedFrom<STCluster,MCParticle> itLink(evtSvc(),msgSvc(),
                                          LHCb::STClusterLocation::ITClusters);
  if( itLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to ITCluster linker table." << endreq;
    return StatusCode::FAILURE;
  }

  int nGoodIT = 0;
  int nMCTotalIT = 0;

  // TT
  const STCluster* ttCluster = ttLink.first ( mcPart );
  found = false;
  while( 0 != ttCluster ) {
    ++nMCTotalIT;
    std::vector<Measurement*>::const_iterator iMeas = ( track->measurements() ).begin();
    while(!found && iMeas != ( track->measurements() ).end()) {
      if( (*iMeas)->type() == Measurement::TT ) {
        STMeasurement* meas = dynamic_cast<STMeasurement*>( *iMeas );
        found = ( ttCluster == meas->cluster() );
      }
      ++iMeas;
    }
    if( found ) { ++nGoodIT; }
    ttCluster = ttLink.next();
  }

  //IT
  const STCluster* itCluster = itLink.first ( mcPart );
  found = false;
  while( 0 != itCluster ) {
    ++nMCTotalIT;
    std::vector<Measurement*>::const_iterator iMeas = ( track->measurements() ).begin();
    while(!found && iMeas != ( track->measurements() ).end()) {
      if( (*iMeas)->type() == Measurement::IT ) {
        STMeasurement* meas = dynamic_cast<STMeasurement*>( *iMeas );
        found = ( itCluster == meas->cluster() );
      }
      ++iMeas;
    }
    if( found ) { ++nGoodIT; }
    itCluster = itLink.next();
  }

  // get OTTimes and count correct and total number of clusters
  // Get the linker table MCParticle => OTTime
  LinkedFrom<OTTime,MCParticle> otLink(evtSvc(),msgSvc(),
                                       LHCb::OTTimeLocation::Default);
  if( itLink.notFound() ) {
    error() << "Unable to retrieve MCParticle to ITCluster linker table." << endreq;
    return StatusCode::FAILURE;
  }

  int nGoodOT = 0;
  int nMCTotalOT = 0;
  
  const OTTime* otTime = otLink.first( mcPart );
  found = false;
  while( 0 != otTime ) {
    ++nMCTotalOT;
    std::vector<Measurement*>::const_iterator iMeas = track->measurements().begin();
    while( !found && iMeas != track->measurements().end() ) {
      if( (*iMeas)->type() == Measurement::OT ) {
        OTMeasurement* meas = dynamic_cast<OTMeasurement*>( *iMeas );
        found = ( otTime == meas->time() );
      }
      ++iMeas;
    }
    if( found ) { ++nGoodOT; }
    veloCluster = veloLink.next();
  }

  // Sum of Velo, IT and OT hits
  int nTotalHits   = nTotalVelo   + nTotalIT   + nTotalOT;
  int nMCTotalHits = nMCTotalVelo + nMCTotalIT + nMCTotalOT;
  int nGoodHits    = nGoodVelo    + nGoodIT    + nGoodOT;

  // calculate hit purities and fill histograms
  if( nTotalHits != 0 ) { plot1D( float( nGoodHits ) / float( nTotalHits ),
                                  40, "Hit purity", -0.01, 1.01, 51 ); }
  if( nTotalVelo != 0 ) { plot1D( float( nGoodVelo ) / float( nTotalVelo ),
                                  41, "Velo hit purity", -0.01, 1.01, 51 ); }
  if( nTotalIT != 0 )   { plot1D( float( nGoodIT ) / float( nTotalIT ), 42,
                                  "IT hit purity", -0.01, 1.01, 51 ); }
  if( nTotalOT != 0 )   { plot1D( float( nGoodOT ) / float( nTotalOT ),
                                  43, "OT hit purity", -0.01, 1.01, 51 ); }
  
  // calculate hit efficiencies and fill histograms
  if( nMCTotalHits != 0 ) { plot1D( float( nGoodHits ) / float( nMCTotalHits ),
                                    50, "Hit efficiency", -0.01, 1.01, 51 ); }
  if( nMCTotalVelo != 0 ) { plot1D( float( nGoodVelo ) / float( nMCTotalVelo ),
                                    51, "Velo hit efficiency", -0.01, 1.01, 51 ); }
  if( nMCTotalIT != 0 )   { plot1D( float( nGoodIT ) / float( nMCTotalIT ),
                                    52, "IT hit efficiency", -0.01, 1.01, 51 ); }
  if( nMCTotalOT != 0 )   { plot1D( float( nGoodOT ) / float( nMCTotalOT ),
                                    53, "OT hit efficiency", -0.01, 1.01, 51 ); }
  
  return StatusCode::SUCCESS;
}
