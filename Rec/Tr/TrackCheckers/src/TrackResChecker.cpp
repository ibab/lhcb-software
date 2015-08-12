// $Id: TrackResChecker.cpp,v 1.13 2009-10-08 14:49:57 wouter Exp $
// Include files 
#include "TrackResChecker.h"

//event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateVector.h"
#include "Event/StateParameters.h"
#include "Event/OTTime.h"
#include "Event/OTMeasurement.h"
#include "Kernel/LHCbID.h"
#include "GaudiAlg/GaudiHistos.h"
#include "GaudiUtils/HistoStats.h"
#include "AIDA/IHistogram1D.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToStream.h"
#include "GaudiAlg/GaudiHistoTool.h"
#include "GaudiAlg/IHistoTool.h"

#include "TrackInterfaces/ITrackProjectorSelector.h"
#include "TrackInterfaces/ITrackProjector.h"


DECLARE_ALGORITHM_FACTORY( TrackResChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackResChecker::TrackResChecker(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
  TrackCheckerBase( name , pSvcLocator ),
  m_otLinker(0,0,"")
{
  declareProperty( "PlotsByMeasType", m_plotsByMeasType = false  );
  declareProperty( "CheckAmbiguity", m_checkAmbiguity = false  );
  declareProperty( "MinToCountAmb", m_minToCountAmb = 8 );
  declareProperty( "MinAmbDist",  m_minAmbDist = 0.3*Gaudi::Units::mm );
}

//=============================================================================
// Destructor
//=============================================================================
TrackResChecker::~TrackResChecker() {} 

StatusCode TrackResChecker::initialize(){

  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = TrackCheckerBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  m_projectorSelector = tool<ITrackProjectorSelector>("TrackProjectorSelector",
                                                      "Projector", this );

  m_histoTools[ 0 ] = createHistoTool("ALL") ;
  if( splitByType() ) 
    for( int itype = LHCb::Track::Velo ; itype <= LHCb::Track::Muon; ++itype ) 
      m_histoTools[ itype ] = createHistoTool(Gaudi::Utils::toString(LHCb::Track::Types(itype))) ;
  
  return StatusCode::SUCCESS;
} 

const IHistoTool* TrackResChecker::createHistoTool( const std::string& name) const
{
  IHistoTool* htool = tool<IHistoTool>( "HistoTool",name,this ) ;
  GaudiHistoTool* ghtool = dynamic_cast<GaudiHistoTool*>(htool) ;
  ghtool->setHistoTopDir( histoPath() + "/" ) ;
  std::string histodir = ghtool->histoDir() ;
  size_t pos = histodir.find('.') ;
  if( pos != std::string::npos) histodir.erase(0,pos+1) ;
  ghtool->setHistoDir(histodir) ;
  return htool ;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode TrackResChecker::execute()
{

  // init the linker stuff
  if (initializeEvent().isFailure()){
    return Error("Failed to initialize event", StatusCode::FAILURE);
  }

  // if we are supposed to check ambiguities, init otlinker
  if (m_checkAmbiguity == true){
    m_otLinker = OTLinks( evtSvc(), msgSvc(),LHCb::OTTimeLocation::Default );
  }

  // get the tracks
  const LHCb::Tracks* tracks = get<LHCb::Tracks>(tracksInContainer()) ;
  
  // loop over them
  for( LHCb::Tracks::const_iterator itrack = tracks->begin() ; 
       itrack != tracks->end(); ++itrack) {
    // Get the associated true particle
    const LHCb::Track* track = *itrack ;
    const LHCb::MCParticle* mcparticle = mcTruth(track) ;
    
    if(mcparticle
       // we actually just want to know if it passes the 'selector' inside the IMCReconstructible
       // && int(selector()->reconstructible(mcparticle)) > int(IMCReconstructible::NotReconstructible)
       ) {
      // split by type..
      const IHistoTool* histotool(0) ;
      if( splitByType() ) {
	histotool = m_histoTools[track->type()] ;
      } else {
	histotool = m_histoTools[0] ;
      }

      // resolutions at predefined z.
      resolutionHistos(*histotool,*track,*mcparticle);
  
      // prob chi^2
      histotool->plot1D(track->probChi2(),"probChi2","probChi2", 0., 1., 50);
      
      // fit status
      histotool->plot1D(track->fitStatus(),"fitStatus","fit status", -0.5, 4.5, 5);
      
      histotool->plot1D(mcparticle->p()/Gaudi::Units::GeV,"truemom","true p [GeV]",0,50,100) ;
      histotool->plot1D(mcparticle->pt()/Gaudi::Units::GeV,"truept","true pT [GeV]",0,10,100) ;
      
      // Resolutions and pulls per Measurement type
      if ( m_plotsByMeasType && track->nMeasurements() > 0 )
	plotsByMeasType(*histotool,*track,*mcparticle);
  
      
      // resolution of drift ambiguity 
      if (m_checkAmbiguity ) checkAmbiguity(*histotool,*track,*mcparticle); 
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
void TrackResChecker::resolutionHistos(const IHistoTool& htool,
				       const LHCb::Track& track, 
				       const LHCb::MCParticle& mcPart ) const
{
  // pulls at vertex
  LHCb::State trueStateVertex;
  idealStateCreator()->createStateVertex(&mcPart,trueStateVertex).ignore();
  LHCb::State vtxState;
  StatusCode sc = extrapolator()->propagate(track,trueStateVertex.z(),
                                            vtxState );
  if (sc.isSuccess()) pullplots(htool,trueStateVertex,vtxState,"vertex");

  // for vertex also make some 2-d plots
  if ( track.type() == LHCb::Track::Long ||
       track.type() == LHCb::Track::Upstream ||
       track.type() == LHCb::Track::Downstream ||
       track.type() == LHCb::Track::Ttrack ) {
    const double invp     = std::abs( track.firstState().qOverP() ) ;
    const double ptrue    = mcPart.p() ;
    const double eta      = mcPart.pseudoRapidity() ; //track.pseudoRapidity();
    
    htool.plot2D( ptrue/Gaudi::Units::GeV , invp * ptrue - 1,
		      "vertex/dpoverp_vs_p", "dp/p vs p", 0., 50., -0.1,0.1, 25, 50);
    htool.plot2D( eta, invp * ptrue -1, 
		  "vertex/dpoverp_vs_eta", "dp/p vs eta", 2., 5., -0.05,0.05, 20, 50);

    const double invperr2 = track.firstState().covariance()(4,4) ;
    if( invperr2 > 0 ) {
      const double ppull = (invp - 1/ptrue)/std::sqrt( invperr2 ) ;
      htool.plot2D( ptrue/Gaudi::Units::GeV, ppull,
		    "vertex/p_pull_vs_p","p pull vs p", 0., 50., -10.,10., 25, 50);
      htool.plot2D( eta, ppull, 
		    "vertex/p_pull_vs_eta","p pull vs eta", 2., 5., -10.,10., 20, 50);
    }
  }
  
  // fraction of tracks with correct charge
  bool correctcharge = track.firstState().qOverP()*mcPart.particleID().threeCharge()>0 ;
  htool.plot1D( correctcharge,"correctcharge","correct charge",-0.5,1.5,2) ;
  
  if ( fullDetail() == true && track.nMeasurements() > 0u) {
    for( LHCb::Track::StateContainer::const_iterator istate = track.states().begin() ;
	 istate != track.states().end(); ++istate) {
      const LHCb::State& state = **istate ;
      // skip the closest to beam, since we already have it
      if( state.location() != LHCb::State::ClosestToBeam ) {
	LHCb::State trueState;
        StatusCode sc = idealStateCreator()->createState( &mcPart, state.z(), trueState );
	if( sc.isSuccess() ) {
	  std::string location = state.location() != LHCb::State::LocationUnknown
	    ? Gaudi::Utils::toString( state.location() ) 
	    : format( "state_%d_mm", int( state.z() ) );
	  pullplots(htool,trueState,state,location);
	} 
      }
    }
  }
}

//=============================================================================
//
//=============================================================================
void TrackResChecker::pullplots(const IHistoTool& htool,
				const LHCb::State& trueState, const LHCb::State& recState,
				const std::string& location ) const {
  
  // save some typing
  const Gaudi::TrackVector&    vec     = recState.stateVector();
  const Gaudi::TrackVector&    trueVec = trueState.stateVector();
  const Gaudi::TrackSymMatrix& cov     = recState.covariance();
  const Gaudi::TrackSymMatrix& trueCov = trueState.covariance();
  const double dx   = vec(0) - trueVec(0);
  const double dy   = vec(1) - trueVec(1);
  const double dtx  = vec(2) - trueVec(2);
  const double dty  = vec(3) - trueVec(3);

  // fill the histograms
  htool.plot1D( dx/Gaudi::Units::cm, location+"/x_res", "x resolution", -0.06, 0.06, 101 );
  htool.plot1D( dy/Gaudi::Units::cm, location+"/y_res","y resolution", -0.06,0.06, 101 );
  htool.plot1D( dtx, location+"/tx_res", "tx resolution", -0.01, 0.01, 100 );
  htool.plot1D( dty, location+"/ty_res", "ty resolution", -0.01, 0.01, 100 );

  htool.plot1D( dx / sqrt(cov(0,0)+trueCov(0,0)),
		location+"/xpull", "x pull", -5., 5., 100 );
  htool.plot1D( dy / sqrt(cov(1,1)+trueCov(1,1)),
		location+"/ypull", "y pull", -5., 5., 100 );
  htool.plot1D( dtx / sqrt(cov(2,2)+trueCov(2,2)),
		location+"/txpull", "tx pull", -5., 5., 100 );
  htool.plot1D( dty / sqrt(cov(3,3)+trueCov(3,3)),
		location+"/typull", "ty pull", -5., 5., 100 );
  
  if ( std::abs(cov(4,4)) > 1e-20 ) { // test that there was a momentum measurement
    const double qop      = vec(4) ;
    const double qoptrue  = trueVec(4);
    const double invp     = std::abs(qop) ;
    const double invptrue = std::abs(qoptrue) ;
    const double qoperr   = std::sqrt(cov(4,4)+trueCov(4,4)) ;
    // make two pulls, to be sensitive to both a curvature and a momentum bias
    htool.plot1D( (qop - qoptrue) / qoperr, location+"/qoppull", "qop pull", -5., 5., 100 );
    htool.plot1D( (invp - invptrue) / qoperr, location+"/ppull", "p pull", -5., 5., 100 );
    htool.plot1D( invp / invptrue  - 1, location+"/dpoverp", "dp/p", -0.05, 0.05, 100 );
    if( invp > 0 )
      htool.plot1D( std::sqrt( cov(4,4) ) / invp, 
		    location+"/expecteddpoverp", "expected dp/p", 0., 0.01, 100 );
  }
}


//=============================================================================
//
//=============================================================================
void TrackResChecker::checkAmbiguity(const IHistoTool& histotool,
				     const LHCb::Track& track, 
                                     const LHCb::MCParticle& mcPart) const
{
  unsigned int wrongOnTrack   = 0;
  unsigned int correctOnTrack = 0;
    
  // copy the container, in anti-cipation of changes in Track
  LHCb::Track::MeasurementContainer measurements = track.measurements() ;
  for(LHCb::Track::MeasurementContainer::const_iterator itMeas = measurements.begin(); 
      itMeas != measurements.end() ; ++itMeas )
    if ( (*itMeas)->type() == LHCb::Measurement::OT ) {
      // only count ones that came from same particle as track.
      const LHCb::OTMeasurement* otMeas = dynamic_cast<const LHCb::OTMeasurement*>(*itMeas);
      LHCb::MCParticle* aParticle = m_otLinker.first(otMeas->channel());
      
      bool found = false;
      if (0 != aParticle) {
        while (( 0 != aParticle )&&(found == false)) {
          if (aParticle == &mcPart) found = true;
          aParticle = m_otLinker.next();
        }  // while
      }  // if
      
      if (found == true) {
	// create true state...
	LHCb::StateVector trueState;
	idealStateCreator()->createStateVector( &mcPart, otMeas->z(), trueState ).ignore();
	ITrackProjector* proj = m_projectorSelector -> projector( *otMeas );
	if (proj != 0) {
	  LHCb::OTMeasurement meascopy( *otMeas ) ;
	  StatusCode sc = proj -> project(trueState , meascopy );
	  if ( sc.isFailure() ){
	    Warning( "Unable to project a state into a measurement", sc, 0 ).ignore();
	  } else {
	    if( meascopy.ambiguity() == otMeas->ambiguity() )
	      ++correctOnTrack;
	    else 
	      ++wrongOnTrack;
	  }
	}
      }
    }
  
  const double sum = wrongOnTrack + correctOnTrack;

  if (sum > m_minToCountAmb){
    histotool.plot1D(correctOnTrack/double(sum),"/frac_corr_amb" , 
		     "frac correct ambiguity",-0.005, 1.005, 101);
  }

}


//=============================================================================
//
//=============================================================================
void TrackResChecker::plotsByMeasType(const IHistoTool& htool, const LHCb::Track& track, 
				      const LHCb::MCParticle& mcPart ) const
{
  
  LHCb::Track::MeasurementContainer measures = track.measurements();
  for ( LHCb::Track::MeasurementContainer::const_iterator it = measures.begin();
        it != measures.end(); ++it ) {

    LHCb::State trueStateAtMeas;
    StatusCode sc = idealStateCreator()->createState( &mcPart, (*it)->z(), trueStateAtMeas); 
    if ( sc.isSuccess() ) {

      const std::string dir = Gaudi::Utils::toString((*it)->type());
      LHCb::State stateAtMeas;
      StatusCode sc = extrapolator()->propagate(track, (*it)->z(), stateAtMeas );
      if ( sc.isSuccess()) {
        // make pull plots as before
        pullplots(htool,trueStateAtMeas,stateAtMeas,dir);
      } 
      
      // Monitor unbiased measurement resolutions
      ITrackProjector* proj = m_projectorSelector -> projector( *(*it) );
      if (proj != 0){
        StatusCode sc = proj -> project(trueStateAtMeas , *(*it) );
        if ( sc.isFailure() ){
          Warning( "Unable to project a state into a measurement", sc, 0 ).ignore();
        }
        else {
          const double res       = proj -> residual();
          const double errorMeas = proj -> errMeasure();
          const double chi2      = proj -> chi2();
          htool.plot1D( res, dir+"/meas_res", 
                        " Measurement resolution", -0.5, 0.5, 100 );
          htool.plot1D( res/errorMeas,dir+"/meas_pull", 
                        " Measurement pull", -5., 5., 100 );
          htool.plot1D( chi2, dir+"/meas_chi2", 
                        " Measurement chi2", 0., 10., 200 );
        }
      }
      else { 
        Warning( "could not get projector for measurement", StatusCode::SUCCESS, 0 ).ignore();
      }
    }
  } // iterate measurements
}

//=============================================================================
//
//=============================================================================
StatusCode TrackResChecker::finalize () 
{

  info() << "     ************************************    "<<endmsg;
  
  for( HistoToolMap::const_iterator ihtool = m_histoTools.begin() ;
       ihtool != m_histoTools.end(); ++ihtool) {
    const IHistoTool* htool = ihtool->second ;  
    const GaudiHistoTool* ghtool = dynamic_cast<const GaudiHistoTool*>(htool) ;
    const AIDA::IHistogram1D* pull[5] = {0,0,0,0,0} ;
    pull[0] = htool->histo( HistoID("vertex/xpull") ) ;
    pull[1] = htool->histo( HistoID("vertex/ypull") ) ;
    pull[2] = htool->histo( HistoID("vertex/txpull") ) ;
    pull[3] = htool->histo( HistoID("vertex/typull") ) ;
    pull[4] = htool->histo( HistoID("vertex/ppull") ) ;
    for(size_t i=0; i<5; ++i)
      if(pull[i]) 
	info() << ghtool->histoDir() << "/" 
	       << std::setiosflags(std::ios_base::left)
	       << std::setw(10) << pull[i]->title() << " "
	       <<  format( ":  mean =  %5.3f +/- %5.3f, RMS = %5.3f +/- %5.3f",
			   pull[i]->mean(), Gaudi::Utils::HistoStats::meanErr(pull[i]),
			   pull[i]->rms(), Gaudi::Utils::HistoStats::rmsErr(pull[i])) << endmsg;
    
    const AIDA::IHistogram1D* res[2]= {0,0} ;
    res[0]  = htool->histo( HistoID("vertex/x_res") ) ;
    res[1]  = htool->histo( HistoID("vertex/y_res") ) ;
    for(size_t i=0; i<2; ++i) 
      if(res[i])
	info() << ghtool->histoDir() << "/"
	       << res[i]->title() << format( ":  RMS =  %5.3f +/- %5.3f micron",
					     res[i]->rms()*1000, Gaudi::Utils::HistoStats::rmsErr(res[i])*1000) << endmsg;
    
    const AIDA::IHistogram1D* dpop = htool->histo( HistoID("vertex/dpoverp") ) ;
    if(dpop)
      info() << ghtool->histoDir() << "/"
	     << dpop->title() << format( ":  mean =  %6.4f +/- %6.4f, RMS =  %6.4f +/- %6.4f",
					 dpop->mean(), Gaudi::Utils::HistoStats::meanErr(dpop),
					 dpop->rms(), Gaudi::Utils::HistoStats::rmsErr(dpop)) << endmsg; 
  }
  
  return TrackCheckerBase::finalize();
}
