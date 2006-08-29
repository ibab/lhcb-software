// $Id: TrackChecker.cpp,v 1.15 2006-08-29 16:52:38 erodrigu Exp $
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
#include "Event/StateTraj.h"

#include "gsl/gsl_cdf.h"

#include "Kernel/ITrajPoca.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

using namespace Gaudi;
using namespace Gaudi::Units;
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TrackChecker );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackChecker::TrackChecker( const std::string& name,
                            ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg( name , pSvcLocator ), 
  m_linkerInTable(""), 
  m_nTracks(0),
  m_nMCTracks(0),
  m_nAsctTracks(0),
  m_nAsctMCTracks(0),
  m_evtAveEff(0.),
  m_err2EvtAveEff(0.),
  m_evtAveGhost(0.),
  m_err2EvtAveGhost(0.),
  m_nMCEvt(0),
  m_nEvt(0), 
  m_correctAmbiguity(0),
  m_wrongAmbiguity(0){

  // default z-positions
  m_zPositions.clear();
  m_zPositions.push_back(   990.0*mm );
  m_zPositions.push_back(  2165.0*mm );
  m_zPositions.push_back(  9450.0*mm );
  m_zPositions.push_back( 11900.0*mm );

  declareProperty( "TracksInContainer",
                   m_tracksInContainer = TrackLocation::Default  );
  declareProperty( "LinkerInTable",   m_linkerInTable = ""       );
  declareProperty( "ZPositions",      m_zPositions               );
  declareProperty( "PlotsByMeasType", m_plotsByMeasType = false  );
  declareProperty( "TrackSelector",
                   m_trackSelectorName = "TrackCriteriaSelector" );
  declareProperty( "RejectFitFailures", m_rejectFitFailures = false );
  declareProperty( "CheckAmbiguity",    m_checkAmbiguity    = true  );
  declareProperty( "MinToCountAmb",     m_minToCountAmb     = 8     );

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
  if ( sc.isFailure() ) { return sc; }

  // Set the path for the linker table Track - MCParticle
  if ( m_linkerInTable == "" ) m_linkerInTable = m_tracksInContainer;

  // Set counters

  m_trackSelector = tool<ITrackCriteriaSelector>( m_trackSelectorName,
                                                  "TrackSelector", this );
  
  m_stateCreator = tool<IIdealStateCreator>( "IdealStateCreator"       );
  m_extrapolator = tool<ITrackExtrapolator>( "TrackMasterExtrapolator" );
  m_projector    = tool<ITrackProjector>( "TrackMasterProjector",
                                          "Projector", this );
  
  // Retrieve the magnetic field and the poca tool
  m_poca = tool<ITrajPoca>( "TrajPoca" );
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackChecker::execute()
{  
  // Set local counters
  int nTracks     = 0;    //< # selected Tracks
  int nMCTracks   = 0;    //< # selected MCParticles
  int nAsctTracks = 0;    //< # selected Tracks with a MCParticle associated
  // # selected MCParticles with a selectByTrackType Track associated
  int nAsctMCTracks = 0;

  // Get the Tracks
  Tracks* tracks = get<Tracks>( m_tracksInContainer );

  // Get the MCParticles
  MCParticles* particles = get<MCParticles>( MCParticleLocation::Default );
  
  // Retrieve the Linker table
  LinkedTo<MCParticle,Track> directLink( evtSvc(), msgSvc(), m_linkerInTable );
  if ( directLink.notFound() ) {
    error() << "Linker table not found" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve the reverse direction Linker table
  LinkedFrom<Track,MCParticle> revLink( evtSvc(), msgSvc(), m_linkerInTable );

  // Loop over the Tracks
  Tracks::const_iterator iTrack;
  for( iTrack = tracks->begin(); iTrack != tracks->end(); ++iTrack ) {
    Track* track = *iTrack;
    // Decide whether the Track will be checked
    if ( select( track ) == true) {
      ++nTracks;
      plot1D( track->type(), 12, "Track type", -0.5, 7.5, 8 );
      plot1D( track->p()/GeV, 30, "Momentum (GeV) at first state", -1., 101., 51 ); 
      // Get MCParticle linked by highest weight to Track
      MCParticle* mcPart = directLink.first( track );
   
      double prob = gsl_cdf_chisq_Q( track->chi2(), track->nDoF() );
      if ( NULL != mcPart ) {
        ++nAsctTracks;
        resolutionHistos( track, mcPart );
        purityHistos( track, mcPart );
        if ( m_checkAmbiguity ) checkAmbiguity( track, mcPart );
        plot1D( track->chi2PerDoF(), 13,
                "Chi2/nDoF (MC-matched tracks)", 0., 500., 1000 );
        plot1D( prob, 14,
                "Chi2 probability (MC-matched tracks)", -0.005, 1.005, 101 );
      }
      else {
        plot1D( track->chi2PerDoF(), 15,
                "Chi2/nDoF (ghost tracks)", 0., 500., 1000 );
        plot1D( prob, 16 ,
                "Chi2 probability (ghost tracks)", -0.005, 1.005, 101);
      }
    }
  } // End loop over Tracks
  
  // Loop over the MCParticles
  MCParticles::const_iterator iPart;
  for( iPart = particles->begin(); particles->end() != iPart; ++iPart ) {
    MCParticle* particle = *iPart;
    // Decide whether the MCParticle will be checked
    if ( m_trackSelector->select( particle ) ) {
      ++nMCTracks;
      // Fill the general histograms
      plot1D( particle->p() / GeV,
              33, "True momentum (GeV) for MCParticles", -1., 101., 51 );
      Track* track = revLink.first( particle );
      if ( NULL != track ) {
        bool found = false;
        while ( !found ) {
          if ( m_trackSelector->selectByTrackType( track ) ) {
            found = true;
            ++nAsctMCTracks;
          }
          // If FALSE, try next Track linked to the MCParticle
          else { track = revLink.next(); }
          // Break from while loop when there is no next Track linked
          if ( NULL == track ) { break; }
        }
      }
    }
  } // End loop over MCParticles

  // Fill number of tracks histos
  plot1D( nTracks, 1, "Number of Tracks per Event", -1., 201., 101 );
  plot1D( nMCTracks, 2, "Number of true Tracks per Event", -1., 201., 101 );

  // Fill global counters
  m_nTracks   += nTracks;        //< Total # of selected Tracks
  m_nMCTracks += nMCTracks;      //< Total # of selected MCParticles
  // Total # of selected Tracks with a MCParticle associated
  m_nAsctTracks += nAsctTracks;
  // Total # of selected MCParticles with a selectByTrackType Track associated
  m_nAsctMCTracks += nAsctMCTracks;

  // Counters for event averaged efficiency
  if ( nMCTracks != 0 ) {
    ++m_nMCEvt;
    // Fraction of selected MCParticles with a selectByTrackType Track associated
    double evtAveEff     = double( nAsctMCTracks ) / double( nMCTracks );
    double err2EvtAveEff =
      nAsctMCTracks * ( 1. - evtAveEff ) / ( nMCTracks * nMCTracks );
    m_evtAveEff = ( m_evtAveEff * ( m_nMCEvt - 1 ) + evtAveEff ) / m_nMCEvt;
    m_err2EvtAveEff = ( m_err2EvtAveEff * ( m_nMCEvt - 1 ) * ( m_nMCEvt - 1 )
                        + err2EvtAveEff ) / ( m_nMCEvt * m_nMCEvt );
    plot1D( evtAveEff, 20, "Track efficiency per event", -0.01, 1.01, 51 );
  }

  // For event averaged ghost rate
  if ( nTracks != 0 ) {
    ++m_nEvt;
    double evtAveGhost     = 1. - double( nAsctTracks ) / double( nTracks );
    double err2EvtAveGhost = nAsctTracks * evtAveGhost / ( nTracks * nTracks );
    m_evtAveGhost = ( m_evtAveGhost * ( m_nEvt - 1 ) + evtAveGhost ) / m_nEvt;
    m_err2EvtAveGhost = ( m_err2EvtAveGhost * ( m_nEvt - 1 ) * ( m_nEvt - 1 )
                          + err2EvtAveGhost ) / ( m_nEvt * m_nEvt );
    plot1D( evtAveGhost, 21, "Track ghost rate per event", -0.01, 1.01, 51 );
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackChecker::finalize()
{
  // Calculate the track averaged efficiency and ghost rate
  double trackAveEff    = double( m_nAsctMCTracks ) / double( m_nMCTracks );
  double errTrackAveEff =
    sqrt( m_nAsctMCTracks * ( 1. - trackAveEff ) ) / m_nMCTracks;
  double trackAveGhost    = 1. - double( m_nAsctTracks ) / double( m_nTracks );
  double errTrackAveGhost = sqrt( m_nAsctTracks * trackAveGhost ) / m_nTracks;
  
  // print out efficiency and ghost rate
  info() << "Track averaged:" << endmsg
         << "Track efficiency = "
         << format( "%5.1f +/- %3.1f %%",
                    100. * trackAveEff, 100. * errTrackAveEff )
         << "  (=" << m_nAsctMCTracks << "/" <<  m_nMCTracks << ")" << endmsg
         << "Ghost rate =       "
         << format( "%5.1f +/- %3.1f %%",
                    100. * trackAveGhost, 100. * errTrackAveGhost )
         << "  (=" << m_nTracks-m_nAsctTracks << "/" << m_nTracks << ")"
         << endmsg
         << "Event averaged:" << endmsg
         << "Track efficiency = "
         << format( "%5.1f +/- %3.1f %%",
                    100. * m_evtAveEff, 100. * sqrt(m_err2EvtAveEff) )
         << endmsg
         << "Ghost rate =       "
         << format( "%5.1f +/- %3.1f %%",
                    100. * m_evtAveGhost, 100. * sqrt(m_err2EvtAveGhost) )
         << endmsg;
  
  return GaudiHistoAlg::finalize();
}

//=============================================================================
//
//=============================================================================
StatusCode TrackChecker::resolutionHistos( Track* track, MCParticle* mcPart )
{
  // Resolutions and pulls at true vertex position
  // ---------------------------------------------
  // Get true values at vertex
  const MCVertex* vOrigin = mcPart->originVertex();
  if ( 0 != vOrigin ) {
    XYZPoint vertPos = vOrigin->position();
    double mcP          = mcPart -> p();
    LorentzVector mcMom = mcPart -> momentum();
    XYZVector mcSlp( mcMom.Px()/mcMom.Pz(), mcMom.Py()/mcMom.Pz(), 1. );
    
    // Extrapolate to true vertex z-position
    State vtxState;
    StatusCode sc = m_extrapolator -> propagate( *track, vertPos.z(),
                                                 vtxState );
    if ( sc.isSuccess() ) {
      double trkX = vtxState.x();
      double trkY = vtxState.y();
      double trkP = vtxState.p();
      TrackSymMatrix cov = vtxState.covariance();
      // calculate impact parameter vector
      XYZVector dist( trkX - vertPos.x(), trkY - vertPos.y(), 0.0 );
      XYZVector trkSlp   = vtxState.slopes();
      XYZVector ipVector = dist.Cross( trkSlp ) / trkSlp.R();

      // fill the histograms
      plot1D( dist.x(), 101, "X resolution at vertex", -0.5, 0.5, 100 );
      plot1D( dist.y(), 102, "Y resolution at vertex", -0.5, 0.5, 100 );
      plot1D( trkSlp.x() - mcSlp.x(),
              103, "Tx resolution at vertex", -0.01, 0.01, 100 );
      plot1D( trkSlp.y() - mcSlp.y(),
              104, "Ty resolution at vertex", -0.01, 0.01, 100 );
      plot1D( trkP / mcP - 1.,
              105, "Momentum (P) resolution at vertex", -0.05, 0.05, 100 );
      plot1D( ipVector.R(),
              106, "Impact parameter at vertex", -0.01, 1.01, 51 );
      plot1D( dist.x() / sqrt(cov(0,0)),
              111, "X pull at vertex", -5., 5., 100 );
      plot1D( dist.y() / sqrt(cov(1,1)),
              112, "Y pull at vertex", -5., 5., 100 );
      plot1D( (trkSlp.x() - mcSlp.x()) / sqrt(cov(2,2)),
              113, "Tx pull at vertex", -5., 5., 100 );
      plot1D( (trkSlp.y() - mcSlp.y()) / sqrt(cov(3,3)),
              114, "Ty pull at vertex", -5., 5., 100 );
      plot1D( ( fabs(vtxState.qOverP()) - 1.0/mcP ) / sqrt(cov(4,4)),
              115, "Q/P pull at vertex", -5., 5., 100 );
      plot1D( ( trkP - mcP ) / sqrt(vtxState.errP2()),
              116, "P pull at vertex", -5., 5., 100 );
      plot1D( sqrt(vtxState.errP2()) / trkP,
              120, "Delta(P)/P at vertex", 0., 0.01, 100 );
      plot2D( trkP/GeV, sqrt(vtxState.errP2()) / trkP,
              121, "Delta(P)/P versus P (GeV) at vertex",
              -1., 101., 0., 0.01, 51, 100 );
    }
  }

  // Resolutions and pulls at 1st measurement
  // ----------------------------------------
  double zAt1stMeas = track->measurements().front()->z();
  // Extrapolate to z-position of 1st measurement
  State stateAt1stMeas;
  StatusCode sc = m_extrapolator -> propagate( *track, zAt1stMeas,
                                               stateAt1stMeas );
  // get the true state at same z-position
  State* trueState;
  sc = m_stateCreator -> createState( mcPart, stateAt1stMeas.z(), trueState );
  if ( sc.isSuccess() ) {
    TrackVector    vec     = stateAt1stMeas.stateVector();
    TrackVector    trueVec = trueState -> stateVector();
    TrackSymMatrix cov     = stateAt1stMeas.covariance();
    TrackSymMatrix trueCov = trueState -> covariance();
    double dx   = vec(0) - trueVec(0);
    double dy   = vec(1) - trueVec(1);
    double dtx  = vec(2) - trueVec(2);
    double dty  = vec(3) - trueVec(3);
    double trkP = stateAt1stMeas.p();
    double mcP  = trueState -> p();
    // fill the histograms
    plot1D( dx, 201, "X resolution at 1st measurement", -0.5, 0.5, 100 );
    plot1D( dy, 202, "Y resolution at 1st measurement", -0.5, 0.5, 100 );
    plot1D( dtx, 203, "Tx resolution at 1st measurement", -0.01, 0.01, 100 );
    plot1D( dty, 204, "Ty resolution at 1st measurement", -0.01, 0.01, 100 );
    plot1D( trkP / mcP - 1.,
            205, "Momentum (P) resolution at 1st measurement", -0.05, 0.05, 100 );
    plot1D( dx / sqrt(cov(0,0)+trueCov(0,0)),
            211, "X pull at 1st measurement", -5., 5., 100 );
    plot1D( dy / sqrt(cov(1,1)+trueCov(1,1)),
            212, "Y pull at 1st measurement", -5., 5., 100 );
    plot1D( dtx / sqrt(cov(2,2)+trueCov(2,2)),
            213, "Tx pull at 1st measurement", -5., 5., 100 );
    plot1D( dty / sqrt(cov(3,3)+trueCov(3,3)),
            214, "Ty pull at 1st measurement", -5., 5., 100 );
    plot1D( ( fabs(vec(4)) - 1.0/mcP ) / sqrt(cov(4,4)+trueCov(4,4)),
            215, "Q/P pull at 1st measurement", -5., 5., 100 );
    plot1D( ( trkP - mcP ) / sqrt(stateAt1stMeas.errP2()+trueState->errP2()),
            216, "P pull at 1st measurement", -5., 5., 100 );
    plot1D( sqrt(stateAt1stMeas.errP2()) / trkP,
            220, "Delta(P)/P at 1st measurement", 0., 0.01, 100 );
    delete trueState;
  }

  // Resolutions and pulls at defined z-positions
  // --------------------------------------------
  if ( !m_plotsByMeasType ) {
    int numPos = 0;
    std::vector<double>::const_iterator iZpos;
    for( iZpos = m_zPositions.begin(); iZpos != m_zPositions.end(); ++iZpos, ++numPos ) {
      // Extrapolate to z-position
      State state;
      StatusCode sc = m_extrapolator -> propagate( *track, *iZpos, state );
      if ( sc.isSuccess() ) {
        // get the true state
        State* trueState;
        StatusCode sc =
          m_stateCreator -> createState( mcPart, state.z(), trueState );
        if ( sc.isSuccess() ) {
          TrackVector    vec     = state.stateVector();
          TrackVector    trueVec = trueState -> stateVector();
          TrackSymMatrix cov     = state.covariance();
          TrackSymMatrix trueCov = trueState -> covariance();
          double dx   = vec(0) - trueVec(0);
          double dy   = vec(1) - trueVec(1);
          double dtx  = vec(2) - trueVec(2);
          double dty  = vec(3) - trueVec(3);
          double trkP = state.p();
          double mcP  = trueState -> p();
          // fill the histograms
          int ID = 300 + 100 * numPos;
          std::string title = format( " at z=%d mm", int( *iZpos ) );
          plot1D( dx, ID+1, "X resolution"+title, -0.5, 0.5, 100 );
          plot1D( dy, ID+2, "Y resolution"+title, -0.5, 0.5, 100 );
          plot1D( dtx, ID+3, "Tx resolution"+title, -0.01, 0.01, 100 );
          plot1D( dty, ID+4, "Ty resolution"+title, -0.01, 0.01, 100 );
          plot1D( trkP / mcP - 1.,
                  ID+5, "Momentum (P) resolution"+title, -0.05, 0.05, 100 );
          plot1D( dx / sqrt(cov(0,0)+trueCov(0,0)),
                  ID+11,"X pull"+title, -5., 5., 100 );
          plot1D( dy / sqrt(cov(1,1)+trueCov(1,1)),
                  ID+12,"Y pull"+title, -5., 5., 100 );
          plot1D( dtx / sqrt(cov(2,2)+trueCov(2,2)),
                  ID+13,"Tx pull"+title, -5., 5., 100 );
          plot1D( dty / sqrt(cov(3,3)+trueCov(3,3)),
                  ID+14,"Ty pull"+title, -5., 5., 100 );
          plot1D( ( fabs(vec(4)) - 1.0/mcP ) / sqrt(cov(4,4)+trueCov(4,4)),
                  ID+15, "Q/P pull"+title, -5., 5., 100 );
          plot1D( ( trkP - mcP ) / sqrt(state.errP2()+trueState->errP2()),
                  ID+16, "P pull"+title, -5., 5., 100 );
          plot1D( sqrt(state.errP2()) / trkP,
                  ID+20, "Delta(P)/P"+title, 0., 0.01, 100 );
        } // if ( sc.isSuccess() )
        delete trueState;
      } // if ( sc.isSuccess() )
    } // loop pos
  } // m_plotsByMeasType
  
  // Resolutions and pulls per Measurement type
  // ------------------------------------------
  if ( m_plotsByMeasType && track->nMeasurements() > 0 ) {
    const std::vector<Measurement*>& measures = track -> measurements();
    for ( std::vector<Measurement*>::const_iterator it = measures.begin();
          it != measures.end(); ++it ) {
      // Extrapolate to z-position
      State state;
      StatusCode sc = m_extrapolator -> propagate( *track, (*it) -> z(), state );
      if ( sc.isSuccess() ) {
        // get the true state
        State* trueState;
        StatusCode sc =
          m_stateCreator -> createState( mcPart, state.z(), trueState );
        if ( sc.isSuccess() ) {
          TrackVector    vec     = state.stateVector();
          TrackVector    trueVec = trueState -> stateVector();
          TrackSymMatrix cov     = state.covariance();
          TrackSymMatrix trueCov = trueState -> covariance();
          double dx   = vec(0) - trueVec(0);
          double dy   = vec(1) - trueVec(1);
          double dtx  = vec(2) - trueVec(2);
          double dty  = vec(3) - trueVec(3);
          double trkP = state.p();
          double mcP  = trueState -> p();
          // fill the histograms
          int ID = 10000 + 100 * (*it) -> type(); // offset given by Meas. type
          std::string title =
            " at z of " + measType( (*it)->type() ) + " Measurements";
          plot1D( dx, ID+1, "X resolution"+title, -0.5, 0.5, 100 );
          plot1D( dy, ID+2, "Y resolution"+title, -0.5, 0.5, 100 );
          plot1D( dtx, ID+3, "Tx resolution"+title, -0.01, 0.01, 100 );
          plot1D( dty, ID+4, "Ty resolution"+title, -0.01, 0.01, 100 );
          plot1D( trkP / mcP - 1.,
                  ID+5, "Momentum (P) resolution"+title, -0.05, 0.05, 100 );
          plot1D( dx / sqrt(cov(0,0)+trueCov(0,0)),
                  ID+11,"X pull"+title, -5., 5., 100 );
          plot1D( dy / sqrt(cov(1,1)+trueCov(1,1)),
                  ID+12,"Y pull"+title, -5., 5., 100 );
          plot1D( dtx / sqrt(cov(2,2)+trueCov(2,2)),
                  ID+13,"Tx pull"+title, -5., 5., 100 );
          plot1D( dty / sqrt(cov(3,3)+trueCov(3,3)),
                  ID+14,"Ty pull"+title, -5., 5., 100 );
          plot1D( ( fabs(vec(4)) - 1.0/mcP ) / sqrt(cov(4,4)+trueCov(4,4)),
                  ID+15, "Q/P pull"+title, -5., 5., 100 );
          plot1D( ( trkP - mcP ) / sqrt(state.errP2()+trueState->errP2()),
                  ID+16, "P pull"+title, -5., 5., 100 );
          plot1D( sqrt(state.errP2()) / trkP,
                  ID+20, "Delta(P)/P"+title, 0., 0.01, 100 );

          // Monitor unbiased measurement resolutions
          StatusCode sc = m_projector -> project( *trueState, *(*it) );
          if ( sc.isFailure() )
            return Error( "not able to project a state into a measurement" );
          double res       = m_projector -> residual();
          double errorMeas = m_projector -> errMeasure();
          double chi2      = m_projector -> chi2();
          plot1D( res, ID+30, measType( (*it)->type() ) + 
                  " Measurement resolution (residual)", -0.5, 0.5, 100 );
          plot1D( res/errorMeas, ID+31, measType( (*it)->type() ) + 
                  " Measurement pull", -5., 5., 100 );
          plot1D( chi2, ID+32, measType( (*it)->type() ) + 
                  " Measurement chi2", 0., 10., 200 );
        } // if ( sc.isSuccess() )
        delete trueState;
      } // if ( sc.isSuccess() )
    } // loop over Measurements
  } // m_plotsByMeasType
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
StatusCode TrackChecker::purityHistos( Track* track, MCParticle* mcPart )
{
  // fill hit purity and hit efficiency histograms
  // get VeloClusters and count correct and total number of clusters

  // Count number of Measurements of the different types
  int nTotalVelo = 0;
  int nTotalTT   = 0;
  int nTotalIT   = 0;
  int nTotalOT   = 0;
  std::vector<Measurement*>::const_iterator itMeas;
  std::vector<Measurement*>::const_iterator endMeas = track->measurements().end();
  for ( itMeas = track->measurements().begin(); itMeas != endMeas; ++itMeas ) {
    if (      (*itMeas)->type() == Measurement::VeloPhi ) { ++nTotalVelo; }
    else if ( (*itMeas)->type() == Measurement::VeloR )   { ++nTotalVelo; }
    else if ( (*itMeas)->type() == Measurement::TT )      { ++nTotalTT;   }
    else if ( (*itMeas)->type() == Measurement::IT )      { ++nTotalIT;   }
    else if ( (*itMeas)->type() == Measurement::OT )      { ++nTotalOT;   }
  }

  // get VeloClusters and count correct and total number of clusters
  // Get the linker table MCParticle => VeloCluster
  LinkedFrom<VeloCluster,MCParticle> veloLink(evtSvc(),msgSvc(),
                                              LHCb::VeloClusterLocation::Default);
  if ( veloLink.notFound() )
    return Error( "Unable to retrieve MCParticle-VeloCluster linker table" );
  
  int nGoodVelo    = 0;
  int nMCTotalVelo = 0;
  const VeloCluster* veloCluster = veloLink.first( mcPart );
  bool found = false;
  while ( 0 != veloCluster ) {
    ++nMCTotalVelo;
    std::vector<Measurement*>::const_iterator iMeas   = track->measurements().begin();
    std::vector<Measurement*>::const_iterator endMeas = track->measurements().end();
    while ( !found && ( iMeas != endMeas ) ) {
      if ( (*iMeas)->type() == Measurement::VeloR ) {
        VeloRMeasurement* meas = dynamic_cast<VeloRMeasurement*>( *iMeas );
        found = ( veloCluster == meas->cluster() );
      }
      if ( (*iMeas)->type() == Measurement::VeloPhi ) {
        VeloPhiMeasurement* meas = dynamic_cast<VeloPhiMeasurement*>( *iMeas );
        found = ( veloCluster == meas->cluster() );
      }
      ++iMeas;
    }
    if ( found ) { ++nGoodVelo; }
    veloCluster = veloLink.next();
  }

  // get STClusters and count correct and total number of clusters
  // Get the linker table MCParticle => TT STCluster
  LinkedFrom<STCluster,MCParticle> ttLink(evtSvc(),msgSvc(),
                                          LHCb::STClusterLocation::TTClusters);
  if ( ttLink.notFound() )
    return Error( "Unable to retrieve MCParticle-TT STCluster linker table" );
  
  // Get the linker table MCParticle => IT STCluster
  LinkedFrom<STCluster,MCParticle> itLink(evtSvc(),msgSvc(),
                                          LHCb::STClusterLocation::ITClusters);
  if ( itLink.notFound() )
    return Error( "Unable to retrieve MCParticle-IT STCluster linker table" );

  // TT
  int nGoodTT    = 0;
  int nMCTotalTT = 0;
  const STCluster* ttCluster = ttLink.first ( mcPart );
  found = false;
  while ( 0 != ttCluster ) {
    ++nMCTotalTT;
    std::vector<Measurement*>::const_iterator iMeas   = track->measurements().begin();
    std::vector<Measurement*>::const_iterator endMeas = track->measurements().end();
    while ( !found && ( iMeas != endMeas ) ) {
      if ( (*iMeas)->type() == Measurement::TT ) {
        STMeasurement* meas = dynamic_cast<STMeasurement*>( *iMeas );
        found = ( ttCluster == meas->cluster() );
      }
      ++iMeas;
    }
    if ( found ) { ++nGoodTT; }
    ttCluster = ttLink.next();
  }

  //IT
  int nGoodIT    = 0;
  int nMCTotalIT = 0;
  const STCluster* itCluster = itLink.first ( mcPart );
  found = false;
  while ( 0 != itCluster ) {
    ++nMCTotalIT;
    std::vector<Measurement*>::const_iterator iMeas   = track->measurements().begin();
    std::vector<Measurement*>::const_iterator endMeas = track->measurements().end();
    while ( !found && ( iMeas != endMeas ) ) {
      if ( (*iMeas)->type() == Measurement::IT ) {
        STMeasurement* meas = dynamic_cast<STMeasurement*>( *iMeas );
        found = ( itCluster == meas->cluster() );
      }
      ++iMeas;
    }
    if ( found ) { ++nGoodIT; }
    itCluster = itLink.next();
  }

  // get OTTimes and count correct and total number of clusters
  // Get the linker table MCParticle => OTTime
  LinkedFrom<OTTime,MCParticle> otLink(evtSvc(),msgSvc(),
                                       LHCb::OTTimeLocation::Default);
  if ( itLink.notFound() )
    return Error( "Unable to retrieve MCParticle-OTTime linker table" );
  
  int nGoodOT    = 0;
  int nMCTotalOT = 0;
  const OTTime* otTime = otLink.first( mcPart );
  found = false;
  while ( 0 != otTime ) {
    ++nMCTotalOT;
    std::vector<Measurement*>::const_iterator iMeas   = track->measurements().begin();
    std::vector<Measurement*>::const_iterator endMeas = track->measurements().end();
    while ( !found && ( iMeas != endMeas ) ) {
      if ( (*iMeas)->type() == Measurement::OT ) {
        OTMeasurement* meas = dynamic_cast<OTMeasurement*>( *iMeas );
        found = ( otTime == meas->time() );
      }
      ++iMeas;
    }
    if ( found ) { ++nGoodOT; }
    otTime = otLink.next();
  }

  // Sum of Velo, TT, IT and OT hits
  int nTotalHits   = nTotalVelo   + nTotalTT   + nTotalIT   + nTotalOT;
  int nMCTotalHits = nMCTotalVelo + nMCTotalTT + nMCTotalIT + nMCTotalOT;
  int nGoodHits    = nGoodVelo    + nGoodTT    + nGoodIT    + nGoodOT;

  // calculate hit purities and fill histograms
  if ( nTotalHits != 0 ) { plot1D( float( nGoodHits ) / float( nTotalHits ),
                                   40, "Hit purity", -0.01, 1.01, 51 ); }
  if ( nTotalVelo != 0 ) { plot1D( float( nGoodVelo ) / float( nTotalVelo ),
                                   41, "Velo hit purity", -0.01, 1.01, 51 ); }
  if ( nTotalTT != 0 )   { plot1D( float( nGoodTT ) / float( nTotalTT ),
                                   42, "TT hit purity", -0.01, 1.01, 51 ); }
  if ( nTotalIT != 0 )   { plot1D( float( nGoodIT ) / float( nTotalIT ),
                                   43, "IT hit purity", -0.01, 1.01, 51 ); }
  if ( nTotalOT != 0 )   { plot1D( float( nGoodOT ) / float( nTotalOT ),
                                   44, "OT hit purity", -0.01, 1.01, 51 ); }
  
  // calculate hit efficiencies and fill histograms
  if ( nMCTotalHits != 0 ) { plot1D( float( nGoodHits ) / float( nMCTotalHits ),
                                     50, "Hit efficiency", -0.01, 1.01, 51 ); }
  if ( nMCTotalVelo != 0 ) { plot1D( float( nGoodVelo ) / float( nMCTotalVelo ),
                                     51, "Velo hit efficiency", -0.01, 1.01, 51 ); }
  if ( nMCTotalTT != 0 )   { plot1D( float( nGoodTT ) / float( nMCTotalTT ),
                                     52, "TT hit efficiency", -0.01, 1.01, 51 ); }
  if ( nMCTotalIT != 0 )   { plot1D( float( nGoodIT ) / float( nMCTotalIT ),
                                     53, "IT hit efficiency", -0.01, 1.01, 51 ); }
  if ( nMCTotalOT != 0 )   { plot1D( float( nGoodOT ) / float( nMCTotalOT ),
                                     54, "OT hit efficiency", -0.01, 1.01, 51 ); }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// TODO: remove this helper as soon as G.O.D. has functionality incorporated
//=============================================================================
std::string TrackChecker::measType ( unsigned int type )
{
  switch ( type ) {
    case LHCb::Measurement::Unknown : return "Unknown";
    case LHCb::Measurement::VeloR   : return "VeloR";
    case LHCb::Measurement::VeloPhi : return "VeloPhi";
    case LHCb::Measurement::TT      : return "TT";
    case LHCb::Measurement::IT      : return "IT";
    case LHCb::Measurement::OT      : return "OT";
    case LHCb::Measurement::Muon    : return "Muon";
    default : return "ERROR wrong value for enum LHCb::Measurement::Type";
  }
}

bool TrackChecker::select(LHCb::Track* aTrack) const{

  if (m_rejectFitFailures && aTrack->fitStatus() != Track::Fitted) return false;

  if (!m_trackSelector->select(aTrack)) return false;

  return true;
}

//=============================================================================
//
//=============================================================================
StatusCode TrackChecker::checkAmbiguity( Track* track, MCParticle* mcPart )
{
  unsigned int wrongOnTrack   = 0;
  unsigned int correctOnTrack = 0;
  
  typedef LinkedTo<LHCb::MCParticle,LHCb::OTTime> OTLinks;
  OTLinks aLinker = OTLinks( evtSvc(), msgSvc(),LHCb::OTTimeLocation::Default );
  
  std::vector<Measurement*>::const_iterator itMeas;
  std::vector<Measurement*>::const_iterator endMeas =
    track->measurements().end();
  for ( itMeas = track->measurements().begin(); itMeas != endMeas; ++itMeas ) {
    if ( (*itMeas)->type() == Measurement::OT ) {
      // only count ones that came from same particle as track.
      OTMeasurement* otMeas = dynamic_cast<OTMeasurement*>(*itMeas);
      LHCb::MCParticle* aParticle = aLinker.first(otMeas->time());
      
      bool found = false;
      if (0 != aParticle) {
        while (( 0 != aParticle )&&(found == false)) {
          if (aParticle == mcPart) found = true;
          aParticle = aLinker.next();
        }  // while
      }  // if
      
      if (found == true) {
        if (checkAmbiguity(mcPart,otMeas) == true){
          ++m_correctAmbiguity;
          ++correctOnTrack;
        }
        else {
          ++m_wrongAmbiguity;
          ++wrongOnTrack;
        }
      }
    } // if
  }  // iterMeas
  
  double sum = wrongOnTrack + correctOnTrack;
  if (sum > m_minToCountAmb){
    plot1D(correctOnTrack/double(sum),17 , "frac correct ambiguity",-0.005, 1.005, 101);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
bool TrackChecker::checkAmbiguity(MCParticle* mcPart, OTMeasurement* otMeas )
{
  // create true state...
  State* trueState;
  m_stateCreator->createState( mcPart, otMeas->z(), trueState );

  // Get the ambiguity using the Poca tool
  XYZVector distance;
  XYZVector bfield;
  m_pIMF -> fieldVector( trueState->position(), bfield );
  StateTraj stateTraj = StateTraj( trueState->stateVector(), trueState->z(), bfield );

  double s1 = 0.0;
  double s2 = (otMeas->trajectory()).arclength( stateTraj.position(s1) );
  m_poca->minimize(stateTraj, s1, otMeas->trajectory(), s2, distance, 20*Gaudi::
Units::mm);
  int ambiguity = ( distance.x() > 0.0 ) ? 1 : -1 ;

  delete trueState;

  return (otMeas->ambiguity() == ambiguity);
}

