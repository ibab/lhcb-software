// $Id: TrackKalmanFilter.cpp,v 1.6 2006-01-17 15:50:55 jvantilb Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from CLHEP
#include "CLHEP/Matrix/DiagMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Units/PhysicalConstants.h"

// from TrackEvent
#include "Event/TrackFunctor.h"

// local
#include "TrackKalmanFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackKalmanFilter
//
//  Original Author: Rutger van der Eijk
//  Created: 07-04-1999
//  Adapted: 21-03-2002  Olivier Callot
//  Adapted: 15-04-2005  Jose A. Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const   ToolFactory<TrackKalmanFilter>  s_factory;
const IToolFactory& TrackKalmanFilterFactory = s_factory;

//=========================================================================
// Standard Constructor, initializes variables
//=========================================================================
TrackKalmanFilter::TrackKalmanFilter( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent) :
  GaudiTool( type, name, parent)
{
  declareInterface<ITrackFitter>( this );

  declareProperty( "Extrapolator"        , m_extrapolatorName =
                                           "TrackMasterExtrapolator" );
  declareProperty( "Projector"           , m_projectorName =
                                           "TrackMasterProjector" );
  declareProperty( "StoreTransport"      , m_storeTransport   = true  );

}

//=========================================================================
// Destructor
//=========================================================================
TrackKalmanFilter::~TrackKalmanFilter() {
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode TrackKalmanFilter::initialize() 
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName );
  m_projector    = tool<ITrackProjector>( m_projectorName );
  m_debugLevel   = msgLevel( MSG::DEBUG );
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// Helper to print a failure comment
//=========================================================================
StatusCode TrackKalmanFilter::failure(const std::string& comment) {
  info() << "TrackKalmanFilter failure: " << comment << endreq;
  return StatusCode::FAILURE;
}

//=========================================================================
// Fit the track
//=========================================================================
StatusCode TrackKalmanFilter::fit( Track& track )
{
  StatusCode sc;
 
  std::vector<Node*>& nodes = track.nodes();
 
  // First node must have a seed state
  std::vector<Node*>::iterator iNode = nodes.begin();
  State state = (*iNode)->state() ;

  // Loop over the nodes in the current order (should be sorted)
  // ==> prediction and filter
  for ( ; iNode != nodes.end(); ++iNode) {
    FitNode& node = *(dynamic_cast<FitNode*>(*iNode));

    // Prediction step
    sc = predict( node, state );
    if ( sc.isFailure() ) return failure( "Unable to predict at a node" );

    // Filter step
    if ( node.hasMeasurement() ) { 
      sc = filter( node, state );
      if ( sc.isFailure() ) return failure( "Unable to filter node " );
    }

    // save filtered state
    node.setState( state );
  }
  
  // Loop in opposite direction for the smoother
  std::vector<Node*>::reverse_iterator iPrevNode = nodes.rbegin();
  std::vector<Node*>::reverse_iterator ithisNode = iPrevNode;
  ++ithisNode;
  while ( iPrevNode != nodes.rend() && ithisNode != nodes.rend() ) {
    FitNode& prevNode = *(dynamic_cast<FitNode*>(*iPrevNode));
    FitNode& thisNode = *(dynamic_cast<FitNode*>(*ithisNode));
    // Smoother step
    sc = smooth( thisNode, prevNode );
    if ( sc.isFailure() ) return failure( "Unable to smooth node!" );  
    ++ithisNode;
    ++iPrevNode;
  }

  // Compute the chi2 and dof
  computeChiSq( track );

  // set the HistoryFit flag to "Kalman fit done"
  track.setHistoryFit( Track::Kalman );

  return sc;
}


//=========================================================================
// Predict the state to this node
//=========================================================================
StatusCode TrackKalmanFilter::predict(FitNode& aNode, State& aState)
{
  HepVector prevStateVec = aState.stateVector();
  HepSymMatrix prevStateCov = aState.covariance();
  double z = aNode.z();
  double deltaZ = z - aState.z();

  if ( deltaZ != 0.0 ) { // only extrapolate when needed
    // first iteration only: need to call extrapolator to get 
    // the predicted state at measurement
    if ( !(aNode.transportIsSet(deltaZ)) || !m_storeTransport ) {
      StatusCode sc = m_extrapolator -> propagate( aState, z );
      if ( sc.isFailure() )
        return failure( "unable to predict state at next measurement" );
      aState.setLocation( (aNode.state()).location() );

      // store transport matrix F, noise matrix and vector for next iterations
      if ( m_storeTransport ) {
        const HepMatrix& F = m_extrapolator->transportMatrix();
        aNode.setTransportMatrix( F );
        aNode.setTransportVector( aState.stateVector() - F * prevStateVec );    
        aNode.setNoiseMatrix( aState.covariance() - prevStateCov.similarity(F));
        aNode.setTransportDeltaZ( deltaZ );
      }
    }
    // next iterations: update node with transport information 
    // from 1st iteration to save CPU time
    else { 
      const HepMatrix& F = aNode.transportMatrix();
      HepVector& stateVec = aState.stateVector();
      HepSymMatrix& stateCov = aState.covariance();
      stateVec = F * stateVec + aNode.transportVector();
      stateCov = stateCov.similarity(F) + aNode.noiseMatrix();
      aState.setZ( z );
      aState.setLocation( (aNode.state()).location() );
    }
  }

  // save predicted state
  aNode.setPredictedState( aState );

  // project the state into the measurement
  if ( aNode.hasMeasurement() ) {
    Measurement& meas = aNode.measurement();
    StatusCode sc = m_projector -> project( aState, meas );
    if ( sc.isFailure() ) 
      return failure( "not able to project a state into a measurement" );
    const HepVector& H = m_projector -> projectionMatrix();

    // calculate predicted residuals
    double res       = m_projector->residual();
    double errorRes  = m_projector->errResidual();

    aNode.setProjectionMatrix( H );
    aNode.setResidual( res ) ;
    aNode.setErrResidual( errorRes ) ;
  }
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::filter(FitNode& node, State& state) 
{
  Measurement& meas = node.measurement();

  // check z position
  if ( fabs(meas.z() - state.z()) > 1e-6) {
    warning() << "z-position of State (=" << state.z() 
              << ") and Measurement (= " << meas.z() 
              << ") are not equal." << endreq;
  }

  // get reference to the state vector and cov
  HepVector&    tX = state.stateVector();
  HepSymMatrix& tC = state.covariance();

  // get the predicted residuals
  double res        = node.residual();
  double errorRes2  = node.errResidual2();
  double errorMeas2 = meas.resolution2();

  // calculate gain matrix K
  const HepVector& H = node.projectionMatrix();
  HepVector mK = (tC * H) / errorRes2;

  // update state vector 
  tX += ( mK * res );

  HepMatrix B  = HepDiagMatrix(tC.num_row(), 1) - ( mK * H.T());
  tC.assign( B * tC * B.T() + ( mK * errorMeas2 * mK.T()));

  double HK = dot ( H, mK );
  res     *= (1 - HK);
  errorRes2 = (1 - HK) * ( errorMeas2 );

  node.setResidual( res );
  node.setErrResidual( sqrt(errorRes2) );

  if ( m_debugLevel ) verbose() << "filter z = " << node.z()
                                << " chi2 = " << node.chi2() << endmsg;

  return StatusCode::SUCCESS;
}


//----------------------------------------------------------------
// smooth a node on a track
// Input: node to be smoothed (k) + last node (k+1)
// remember we are now running in the opposite direction to the filter !!
// M. Needham 9/11/99
//----------------------------------------------------------------
StatusCode TrackKalmanFilter::smooth( FitNode& thisNode,
                                      const FitNode& prevNode )
{
  // preliminaries, first we need to invert the _predicted_ covariance
  // matrix at k+1
  const HepVector&    prevNodeX = prevNode.predictedState().stateVector();
  const HepSymMatrix& prevNodeC = prevNode.predictedState().covariance();

  // check that the elements are not too large else dsinv will crash
  StatusCode sc = checkInvertMatrix(prevNodeC);
  if ( sc.isFailure() ) return failure("not valid matrix in smoother"); 

  // invert the covariance matrix
  HepSymMatrix invPrevNodeC = prevNodeC;
  sc = invertMatrix(invPrevNodeC);
  if ( sc.isFailure() ) return failure("inverting matrix in smoother");

  // references to _predicted_ state + cov of this node from the first step
  HepVector&    thisNodeX = thisNode.state().stateVector();
  HepSymMatrix& thisNodeC = thisNode.state().covariance();

  // Save filtered state vector
  HepVector oldNodeX = thisNodeX ;

  // transport
  const HepMatrix& F = prevNode.transportMatrix();

  // calculate gain matrix A
  HepMatrix A = thisNodeC * F.T() * invPrevNodeC;

  // best = smoothed state at prev Node
  const HepVector&    prevNodeSmoothedX = prevNode.state().stateVector();
  const HepSymMatrix& prevNodeSmoothedC = prevNode.state().covariance();

  // smooth state
  thisNodeX += A * (prevNodeSmoothedX - prevNodeX);

  // smooth covariance  matrix
  HepSymMatrix covDiff = prevNodeSmoothedC - prevNodeC;
  HepSymMatrix covUpDate = covDiff.similarity(A);
  thisNodeC += covUpDate;

  // check that the cov matrix is positive
  sc = checkPositiveMatrix( thisNodeC ) ;
  if ( sc.isFailure() ) return Warning("non-positive Matrix in smoother");

  // No need to update residuals for node w/o measurement
  if ( thisNode.hasMeasurement() ) { 
    // update = smooth the residuals
    const HepVector& H = thisNode.projectionMatrix();
    double res = thisNode.residual() - dot( H, thisNodeX - oldNodeX ) ;
    thisNode.setResidual( res );
    double errRes2 = thisNode.errResidual2() - covUpDate.similarity( H );
    if ( errRes2 < 0.) {
      return Warning( "Negative residual error in smoother!" );
    }
    thisNode.setErrResidual( sqrt(errRes2) );    

    if ( m_debugLevel ) verbose() << "smoother z = " << thisNode.z()
                                  << " chi2 = " << thisNode.chi2() << endmsg;
  }
       
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
void TrackKalmanFilter::computeChiSq( Track& track ) 
{
  double chi2 = 0.;
  int ndof = -( track.firstState().nParameters() );

  // Loop over the node to calculate total chi2 and # measurements
  const std::vector<Node*>& nodes = track.nodes();
  std::vector<Node*>::const_iterator it = nodes.begin();
  for ( ; it != nodes.end(); ++it ) {
    chi2 += (*it)->chi2();
    if ( (*it)->hasMeasurement() ) ++ndof; 
  }

  double chi2Norma = (ndof != 0 ) ? chi2/((double) (ndof)) : 0.0;
  track.setChi2PerDoF( chi2Norma );
  track.setNDoF( ndof );
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkInvertMatrix( const HepSymMatrix& mat ) 
{
  unsigned int nParam = (unsigned int) mat.num_row();
  for ( unsigned int i = 0; i < nParam; ++i ) {
    for ( unsigned int j = 0; j < nParam; ++j ) {
      if ( mat[i][j] > 1e20 )
        return Warning( "Covariance errors too big to invert!" );
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkPositiveMatrix( HepSymMatrix& mat ) 
{
  unsigned int nParam = (unsigned int) mat.num_row();
  for ( unsigned int i=0; i < nParam; ++i ) {
    if ( mat[i][i] <= 0. )
      return Warning( "Covariance matrix has non-positive elements!" );
  }

 return StatusCode::SUCCESS;
}

//=========================================================================
// Invert prev node covariance matrix
// What follows may seem strange - trust me it works - you
// are strongly recommended NOT to change it. It turns out that
// the choice of MeV, mm as units is BAD - the inversion simply fails
// for numerical reasons. Therefore it is necessary to change back to G3
// units, invert then go back to G4 units
// M. Needham 13/6/2000
// J.A. Hernando (we trust you) 15/05/05
//=========================================================================
StatusCode TrackKalmanFilter::invertMatrix( HepSymMatrix& invPrevNodeC )
{
  // Geant3 units
  cToG3(invPrevNodeC);

  HepSymMatrix tempMatrix = invPrevNodeC;

  int ifail;
  invPrevNodeC.invert(ifail);

  // Geant4 units
  cToG4(invPrevNodeC);

  if ( ifail != 0 ) {
    warning() << "Failed to invert covariance matrix, failure code "
              << ifail << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Change some units for better matrix invertability
//=========================================================================
void TrackKalmanFilter::cToG3( HepSymMatrix& C )
{
  // cov matrix
  C[0][0] /= cm2;
  C[0][1] /= cm2;
  C[0][2] /= cm;
  C[0][3] /= cm;
  C[0][4]  = C[0][4]*GeV/cm;

  C[1][1] /= cm2;
  C[1][2] /= cm;
  C[1][3] /= cm;
  C[1][4]  = C[1][4]*GeV/cm;

  C[2][4]  = C[2][4]*GeV;

  C[3][4]  = C[3][4]*GeV;

  C[4][4]  = C[4][4]*GeV*GeV;
}

//=========================================================================
//  Restore units after inversion. same as cToG3 !
//=========================================================================
void TrackKalmanFilter::cToG4( HepSymMatrix& invC )
{
  // cov matrix
  invC[0][0] /= cm2;
  invC[0][1] /= cm2;
  invC[0][2] /= cm;
  invC[0][3] /= cm;
  invC[0][4]  = invC[0][4]*GeV/cm;

  invC[1][1] /= cm2;
  invC[1][2] /= cm;
  invC[1][3] /= cm;
  invC[1][4]  = invC[1][4]*GeV/cm;

  invC[2][4]  = invC[2][4]*GeV;

  invC[3][4]  = invC[3][4]*GeV;

  invC[4][4]  = invC[4][4]*GeV*GeV;
}

//=========================================================================
