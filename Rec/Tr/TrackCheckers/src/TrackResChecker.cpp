// $Id: TrackResChecker.cpp,v 1.3 2007-11-30 14:36:02 wouter Exp $
// Include files 
#include "TrackResChecker.h"

//event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateVector.h"
#include "Event/StateParameters.h"
#include "Event/StateTraj.h"
#include "Event/OTMeasurement.h"
#include "Kernel/LHCbID.h"

#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "TrackInterfaces/ITrackProjectorSelector.h"
#include "TrackInterfaces/ITrackProjector.h"

// boost 
#include <boost/assign/list_of.hpp> // for 'map_list_of()'



DECLARE_ALGORITHM_FACTORY( TrackResChecker );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackResChecker::TrackResChecker(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
  TrackCheckerBase( name , pSvcLocator ),
  m_otLinker(0,0,"")
{
  declareProperty( "ZPositions", m_zPositions = boost::assign::list_of(StateParameters::ZBegRich1)
                                                               (StateParameters::ZEndRich1)
                                                               (StateParameters::ZBegRich2)
                                                               (StateParameters::ZEndRich2)
		   (7500.)(8450.)(800.)(2700.)(400.)
);
  declareProperty( "PlotsByMeasType", m_plotsByMeasType = false  );
  declareProperty( "CheckAmbiguity", m_checkAmbiguity = false  );
  declareProperty( "MinToCountAmb", m_minToCountAmb = 8 );
  declareProperty( "MinAmbDist",  m_minAmbDist = 0.3*Gaudi::Units::mm );
}

//=============================================================================
// Destructor
//=============================================================================
TrackResChecker::~TrackResChecker() {}; 

StatusCode TrackResChecker::initialize(){

  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = TrackCheckerBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  m_projectorSelector = tool<ITrackProjectorSelector>("TrackProjectorSelector",
                                                      "Projector", this );

  return StatusCode::SUCCESS;
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

  // get the mcparticles 
  LHCb::MCParticles* mcParticles = get <LHCb::MCParticles>(LHCb::MCParticleLocation::Default);

  // loop over them
  LHCb::MCParticles::const_iterator iterPart = mcParticles->begin();
  for (; iterPart != mcParticles->end(); ++iterPart){
   if (selected(*iterPart) == true){
     const TrackCheckerBase::LinkInfo info = reconstructed(*iterPart);
     const LHCb::Track* aTrack = info.track;
     if (aTrack != 0) {

       // split by type..
       std::string type;
       splitByAlgorithm() == true ? type = 
          Gaudi::Utils::toString(aTrack->history()):type= all(); 

       // resolutions at predefined z.
       resolutionHistos(aTrack,*iterPart, type);

       // prob chi^2
       plot(aTrack->probChi2(),type+"/1","probChi2", -0.01, 1.01, 51);

       // resolution of drift ambiguity 
       if (m_checkAmbiguity == true) checkAmbiguity(aTrack,*iterPart, type); 

       // Resolutions and pulls per Measurement type
       if ( m_plotsByMeasType && aTrack->nMeasurements() > 0 ) {
         plotsByMeasType(aTrack,*iterPart,type);
       } // resolutions by meas type
     }    
   } // selected
  } //iterPart
  
  return StatusCode::SUCCESS;
}

void TrackResChecker::resolutionHistos(const LHCb::Track* track, 
                                       const LHCb::MCParticle* mcPart,
                                       const std::string& type ) const{

  // pulls at vertex
  LHCb::State trueStateVertex;
  idealStateCreator()->createStateVertex(mcPart,trueStateVertex).ignore();
  LHCb::State vtxState;
  StatusCode sc = extrapolator()->propagate(*track,trueStateVertex.z(),
                                            vtxState );
  if (sc.isSuccess()) pullplots(trueStateVertex,vtxState,type+"/vertex");

  // for vertex also make some 2-d plots
  if (fullDetail() == true){

    const double errP =  sqrt(track->firstState().errP2()); 
    plot2D(track->p()/Gaudi::Units::GeV , (track->p() - mcPart->p()) / track->p(), 
          type+"/vertex/100", "dp/p vs p", 0., 200., -0.1,0.1, 20, 50);

    plot2D( track->p()/Gaudi::Units::GeV, (track->p() - mcPart->p())/errP, 
          type+"/vertex/101","p pull vs p", 0., 200., -10.,10., 20, 50);

    const double eta = track->pseudoRapidity();
    plot2D( eta, (track->p() - mcPart->p()) / track->p(), 
          type+"/vertex/102", "dp/p vs eta", 2., 5., -0.05,0.05, 20, 50);
    plot2D( eta, (track->p() - mcPart->p()) /errP, 
          type+"/vertex/103","p pull vs eta", 2., 5., -10.,10., 20, 50);
  } 

  // pulls at first measurement
  if (fullDetail() == true && track->nMeasurements() > 0u){
    const double zFirst = track->measurements().front()->z();
    LHCb::State trueStateAt1stMeas;
    idealStateCreator()->createState(mcPart,zFirst,trueStateAt1stMeas).ignore();
    LHCb::State firstState;
    sc = extrapolator()->propagate(*track,trueStateAt1stMeas.z(),
                                 firstState );
    if (sc.isSuccess()) pullplots(trueStateAt1stMeas,firstState,type+"/firstMeas");

   
    // pulls at some z
    std::vector<double>::const_iterator zpos;
    for ( zpos = m_zPositions.begin(); zpos != m_zPositions.end(); ++zpos) {
      // Extrapolate to z-position
    
      LHCb::State state = track->closestState(*zpos);
      sc = extrapolator()->propagate( *track, *zpos, state );
      if ( sc.isSuccess() ) {
        // get the true state
        std::string location = format( "state_%d_mm", int( *zpos ) );
        LHCb::State trueState;
        StatusCode sc = idealStateCreator()->createState( mcPart, state.z(), trueState );
        if (sc.isSuccess()) pullplots(trueState,state,type+"/"+location);
      } // succes
    } // loop zpos
  } // full detail
}

void TrackResChecker::pullplots(const LHCb::State& trueState, const LHCb::State& recState,
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
  const double trkP = recState.p();
  const double mcP  = trueState.p();

  // fill the histograms
  plot1D( dx/Gaudi::Units::cm, location+"/1", "x resolution", -2.5, 2.5, 100 );
  plot1D( dy/Gaudi::Units::cm, location+"/2","y resolution", -50.5, 50.5, 100 );
  plot1D( dtx, location+"/3", "tx resolution", -0.1, 0.1, 100 );
  plot1D( dty, location+"/4", "ty resolution", -0.1, 0.1, 100 );
  plot1D(1.0  - mcP / trkP,
         location+"/5", "dp/p", -0.05, 0.05, 100 );

  plot1D( dx / sqrt(cov(0,0)+trueCov(0,0)),
          location+"/11", "x pull", -5., 5., 100 );
  plot1D( dy / sqrt(cov(1,1)+trueCov(1,1)),
          location+"/12", "y pull", -5., 5., 100 );
  plot1D( dtx / sqrt(cov(2,2)+trueCov(2,2)),
          location+"/13", "tx pull", -5., 5., 100 );
  plot1D( dty / sqrt(cov(3,3)+trueCov(3,3)),
          location+"/14", "ty pull", -5., 5., 100 );
  plot1D(( fabs(vec(4)) - 1.0/mcP ) / sqrt(cov(4,4)+trueCov(4,4)),
         location+"/15", "p pull", -5., 5., 100 ); 
  plot1D(sqrt(recState.errP2()) / trkP,
         location+"/22", "expected dp/p", 0., 0.01, 100 );
}

//=============================================================================
//
//=============================================================================
void TrackResChecker::checkAmbiguity(const LHCb::Track* track, 
                                     const LHCb::MCParticle* mcPart,
                                     const std::string& type) const
{
  unsigned int wrongOnTrack   = 0;
  unsigned int correctOnTrack = 0;
    
  std::vector<LHCb::Measurement*>::const_iterator itMeas;
  std::vector<LHCb::Measurement*>::const_iterator endMeas =
    track->measurements().end();
  for ( itMeas = track->measurements().begin(); itMeas != endMeas; ++itMeas ) {
    if ( (*itMeas)->type() == LHCb::Measurement::OT ) {
      // only count ones that came from same particle as track.
      LHCb::OTMeasurement* otMeas = dynamic_cast<LHCb::OTMeasurement*>(*itMeas);
      LHCb::MCParticle* aParticle = m_otLinker.first(otMeas->channel());
      
      bool found = false;
      if (0 != aParticle) {
        while (( 0 != aParticle )&&(found == false)) {
          if (aParticle == mcPart) found = true;
          aParticle = m_otLinker.next();
        }  // while
      }  // if
      
      Gaudi::XYZVector direction = otMeas->trajectory().direction(0.0);

      if (found == true) {
        if (checkAmbiguity(mcPart,otMeas) == true){
          ++correctOnTrack;
        }
        else {
          ++wrongOnTrack;
        }
      }
    } // if
  }  // iterMeas
  
  const double sum = wrongOnTrack + correctOnTrack;

  if (sum > m_minToCountAmb){
    plot1D(correctOnTrack/double(sum),type+"/100" , 
           "frac correct ambiguity",-0.005, 1.005, 101);
  }

}

//=============================================================================
//
//=============================================================================
bool TrackResChecker::checkAmbiguity(const LHCb::MCParticle* mcPart, 
                                     const LHCb::OTMeasurement* otMeas ) const
{
  // create true state...
  LHCb::StateVector trueState;
  idealStateCreator()->createStateVector( mcPart, otMeas->z(), trueState ).ignore();

  // Get the ambiguity using the Poca tool
  Gaudi::XYZVector distance;
  Gaudi::XYZVector bfield;
  fieldSvc()-> fieldVector( trueState.position(), bfield );
  LHCb::StateTraj stateTraj = LHCb::StateTraj( trueState.parameters(), trueState.z(), bfield );

  double s1 = 0.0;
  double s2 = (otMeas->trajectory()).arclength( stateTraj.position(s1) );
  StatusCode sc = pocaTool()->minimize(stateTraj, s1, otMeas->trajectory(), s2,
                                       distance, 20*Gaudi::Units::mm);
  if( sc.isFailure() ) {
    warning() << "TrajPoca minimize failed in checkAmbiguity." << endreq;
  }
  int ambiguity = ( distance.x() > 0.0 ) ? 1 : -1 ;
  bool ok = false;
  if (otMeas->ambiguity() == ambiguity || distance.R() < m_minAmbDist){
    ok = true;
  }
  return ok;
}

void TrackResChecker::plotsByMeasType(const LHCb::Track* track, 
                                      const LHCb::MCParticle* mcPart,
                                      const std::string& type ) const
{

  const std::vector<LHCb::Measurement*>& measures = track->measurements();
  for ( std::vector<LHCb::Measurement*>::const_iterator it = measures.begin();
        it != measures.end(); ++it ) {

    LHCb::State trueStateAtMeas;
    StatusCode sc = idealStateCreator()->createState( mcPart, (*it)->z(), trueStateAtMeas); 
    if ( sc.isSuccess() ) {

      const std::string dir = type+"/"+Gaudi::Utils::toString((*it)->type());
      LHCb::State stateAtMeas;
      StatusCode sc = extrapolator()->propagate(*track, (*it)->z(), stateAtMeas );
      if ( sc.isSuccess()) {
        // make pull plots as before
        pullplots(trueStateAtMeas,stateAtMeas,dir);
      } 

      // Monitor unbiased measurement resolutions
      ITrackProjector* proj = m_projectorSelector -> projector( *(*it) );
      if (proj != 0){
        StatusCode sc = proj -> project(trueStateAtMeas , *(*it) );
        if ( sc.isFailure() ){
	  Warning( "not able to project a state into a measurement" );
	}
        else {
          const double res       = proj -> residual();
          const double errorMeas = proj -> errMeasure();
          const double chi2      = proj -> chi2();
          plot1D( res, dir+"/30", 
                  " Measurement resolution", -0.5, 0.5, 100 );
          plot1D( res/errorMeas,dir+"/31", 
                  " Measurement pull", -5., 5., 100 );
          plot1D( chi2, dir+"/32", 
                  " Measurement chi2", 0., 10., 200 );
	}
      }
      else { 
        Warning( "could not get projector for measurement", StatusCode::SUCCESS );
      }
    }
  } // iterate measurements
}












