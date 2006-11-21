// $Id: TrackKalmanFilter.cpp,v 1.35 2006-11-21 10:06:48 cattanem Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// From LHCbMath
#include "LHCbMath/MatrixManip.h"

// from TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/TrackUnitsConverters.h"

// local
#include "TrackKalmanFilter.h"

using namespace Gaudi;
using namespace Gaudi::Math;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TrackKalmanFilter
//
//  Original Author: Rutger van der Eijk
//  Created: 07-04-1999
//  Adapted: 21-03-2002  Olivier Callot
//  Adapted: 15-04-2005  Jose A. Hernando, Eduardo Rodrigues
//  Adapted: 20-10-2006  J. van Tilburg
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackKalmanFilter );

//=========================================================================
// Standard Constructor, initializes variables
//=========================================================================
TrackKalmanFilter::TrackKalmanFilter( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent) :
  GaudiTool( type, name, parent)
{
  declareInterface<ITrackFitter>( this );

  declareProperty( "Extrapolator"  , m_extrapolatorName =
                   "TrackMasterExtrapolator" );
  declareProperty( "Projector"     , m_projectorName =
                   "TrackMasterProjector" );
  declareProperty( "StoreTransport"   , m_storeTransport    = true   );
  declareProperty( "BiDirectionalFit" , m_biDirectionalFit  = true   );
  declareProperty( "UnbiasedResiduals", m_unbiasedResiduals = false  );

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

  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName,
                                             "Extrapolator", this );
  m_projector    = tool<ITrackProjector>( m_projectorName,
                                          "Projector", this );
  m_debugLevel   = msgLevel( MSG::DEBUG );
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// Helper to print a failure comment
//=========================================================================
StatusCode TrackKalmanFilter::failure( const std::string& comment ) {
  if ( m_debugLevel )
    debug() << "TrackKalmanFilter failure: " << comment << endreq;
  return StatusCode::FAILURE;
}

//=========================================================================
// Fit the track
//=========================================================================
StatusCode TrackKalmanFilter::fit( Track& track )
{
  StatusCode sc; 
 
  // First node must have a seed state
  std::vector<Node*>& nodes = track.nodes();
  std::vector<Node*>::iterator iNode = nodes.begin();
  State state = (*iNode)->state() ;
  TrackSymMatrix seedCov = state.covariance();

  // Set the reference vector (initially this is taken from the seed state)
  TrackVector refVec = state.stateVector();

  // Loop over the nodes in the current order (should be sorted)
  // ==> prediction and filter
  for ( ; iNode != nodes.end(); ++iNode) {
    FitNode& node = dynamic_cast<FitNode&>(**iNode);

    // Prediction step
    sc = predict( node, state, refVec );
    if ( sc.isFailure() ) return failure( "unable to predict node" );

    // save predicted state
    node.setPredictedState( state );

    // update the reference vector
    refVec = state.stateVector();

    if ( node.hasMeasurement() ) {
      // Projection step
      sc = project( node, state );
      if ( sc.isFailure() ) return failure( "unable to project node" );

      // Filter step
      sc = filter( node, state );
      if ( sc.isFailure() ) return failure( "unable to filter node" );

      // update the reference vector
      refVec = node.measurement().refVector();
    }

    // save filtered state
    node.setState( state );

  } // end of prediction and filter

  // Run the bidirectional fit
  if ( m_biDirectionalFit ) {

    // Reset the covariance matrix
    state.setCovariance( seedCov );

    // Loop over the nodes in the revers order (should be sorted)
    std::vector<Node*>::reverse_iterator irNode = nodes.rbegin();
    std::vector<Node*>::reverse_iterator irPrevNode = irNode;
    if ( irNode == nodes.rend() ) return failure( "zero nodes left" );
    while ( irNode != nodes.rend() ) {
      FitNode& node     = *(dynamic_cast<FitNode*>(*irNode));
      FitNode& prevNode = *(dynamic_cast<FitNode*>(*irPrevNode));

      // Prediction step to next node
      if ( irPrevNode != irNode ) { // No prediction needed for 1st node
        if ( m_storeTransport ) {
          sc = predictReverseFit( prevNode, node, state );
          if ( sc.isFailure() )
            return failure( "unable to predict (reverse fit) node" );
        } else {
          sc = predict( node, state, refVec );
          if ( sc.isFailure() ) return failure( "unable to predict node" );
        }
      }

      // save predicted state
      node.setPredictedStateRev( state );

      // set the reference vector
      refVec = state.stateVector();

      if ( node.hasMeasurement() ) {
        // Projection step
        sc = project( node, state );
        if ( sc.isFailure() ) return failure( "unable to project node" );
      
        // Filter step
        sc = filter( node, state );
        if ( sc.isFailure() ) return failure( "unable to filter node!" );

        // update the reference vector
        refVec = node.measurement().refVector();
      }

      // Smoother step
      if ( irPrevNode != irNode ) sc = biSmooth( node );
      if ( sc.isFailure() ) return failure( "unable to biSmooth node!" );

      irPrevNode = irNode;
      ++irNode;
    } // end of prediction and filter
    track.setFitHistory( Track::BiKalman );
  }
  else {
   // Loop in opposite direction for the old RTS smoother
    std::vector<Node*>::reverse_iterator iPrevNode = nodes.rbegin();
    std::vector<Node*>::reverse_iterator ithisNode = iPrevNode;
    ++ithisNode;
    while ( iPrevNode != nodes.rend() && ithisNode != nodes.rend() ) {
      FitNode& prevNode = dynamic_cast<FitNode&>(**iPrevNode);
      FitNode& thisNode = dynamic_cast<FitNode&>(**ithisNode);
      // Smoother step
      sc = smooth( thisNode, prevNode );
      if ( sc.isFailure() ) return failure( "unable to smooth node!" );
      ++ithisNode;
      ++iPrevNode;
    }
    track.setFitHistory( Track::StdKalman );
  }
  
  // Compute the chi2 and degrees of freedom
  computeChi2( track );

  return sc;
}

//=========================================================================
// Predict the state to this node
//=========================================================================
StatusCode TrackKalmanFilter::predict( FitNode& aNode, State& aState,
                                       const TrackVector& refVec )
{
  TrackVector prevStateVec = aState.stateVector();
  TrackSymMatrix prevStateCov = aState.covariance();
  double z = aNode.z();

  // first iteration only: need to call extrapolator to get 
  // the predicted state at measurement
  if ( !(aNode.transportIsSet()) || !m_storeTransport ) {

    // Use reference vector to fill state
    aState.setState( refVec );

    // TODO: I want to call the extrapolator with the reference vector not 
    //       a State (JvT)
    StatusCode sc = m_extrapolator -> propagate( aState, z );
    if ( sc.isFailure() ) {
      std::ostringstream mess;
      mess << "unable to predict state at next measurement at z = " << z;
      return failure( mess.str() );
    }
    aState.setLocation( (aNode.state()).location() );
    
    // store transport matrix F
    const TrackMatrix& F = m_extrapolator -> transportMatrix();
    aNode.setTransportMatrix( F );

    // Use Taylor expansion
    TrackVector& stateVec = aState.stateVector();
    stateVec += F * ( prevStateVec - refVec );

    // store noise matrix and vector for next iterations
    if ( m_storeTransport ) {
      aNode.setTransportVector( aState.stateVector() - F * prevStateVec );
      aNode.setNoiseMatrix( aState.covariance() - Similarity( F,prevStateCov));
      aNode.setTransportIsSet( true );
    }
  }
  // next iterations: update node with transport information 
  // from 1st iteration to save CPU time
  else { 
    const TrackMatrix& F = aNode.transportMatrix();
    TrackVector& stateVec = aState.stateVector();
    TrackSymMatrix& stateCov = aState.covariance();
    TrackVector tempVec( stateVec );
    stateVec = F * tempVec + aNode.transportVector();
    stateCov = Similarity( F, stateCov ) + aNode.noiseMatrix();
    aState.setZ( z );
    aState.setLocation( (aNode.state()).location() );
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// Predict the state to this node
//=========================================================================
StatusCode TrackKalmanFilter::predictReverseFit(const FitNode& prevNode, 
                                                const FitNode& aNode,
                                                State& aState)
{
  // invert the transport matrix
  TrackMatrix invF = prevNode.transportMatrix();
  if ( !(invF.Invert()) )
    return failure( "unable to invert matrix in predictReverseFit" );

  // Get state vector
  TrackVector& stateVec = aState.stateVector();
  TrackVector tempVec( stateVec );
  stateVec = invF * ( tempVec - prevNode.transportVector() );

  // Calculate the predicted covariance
  TrackSymMatrix& stateCov = aState.covariance();
  TrackSymMatrix tempCov = stateCov + prevNode.noiseMatrix();
  stateCov = Similarity( invF, tempCov );

  aState.setZ( aNode.z() );
  aState.setLocation( (aNode.state()).location() );

  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::project(FitNode& aNode, const State& aState)
{
  // project the state into the measurement
  Measurement& meas = aNode.measurement();
  StatusCode sc = m_projector -> project( aState, meas );
  if ( sc.isFailure() ) 
    return failure( "unable to project a state into a measurement" );
  const TrackProjectionMatrix& H = m_projector -> projectionMatrix();

  // calculate predicted residuals
  double res       = m_projector -> residual();
  double errorRes  = m_projector -> errResidual();
  double errorMeas = m_projector -> errMeasure();
  
  aNode.setProjectionMatrix( H );
  aNode.setResidual( res ) ;
  aNode.setErrResidual( errorRes ) ;
  aNode.setErrMeasure( errorMeas ) ;
  aNode.setProjectionTerm( res + Vector1(H*aState.stateVector())(0) );
  
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
    Warning( "Z positions of State and Measurement are not equal", 0, 1 );
    debug() << "State at z=" << state.z() 
            << ", Measurement at z=" << meas.z() << endmsg;
  }

  // get reference to the state vector and cov
  TrackVector&    X = state.stateVector();
  TrackSymMatrix& C = state.covariance();

  // get the predicted residuals
  double res        = node.residual();
  double errorRes2  = node.errResidual2();
  double errorMeas2 = node.errMeasure2();

  // calculate gain matrix K
  const TrackProjectionMatrix& H = node.projectionMatrix();
  SMatrix<double,5,1> K = (C * Transpose(H)) / errorRes2;

  // update the state vector
  X += K.Col(0) * res ;

  // update the covariance matrix
  static const TrackSymMatrix unit = TrackSymMatrix( SMatrixIdentity());
  C = Symmetrize( Similarity( unit - ( K*H ), C ) 
                  +(errorMeas2*K)*Transpose(K) );

  // update the residual and the error on the residual
  double gain = 1.- Matrix1x1( H*K )(0,0);
  res      *= gain;
  errorRes2 = gain * errorMeas2;

  node.setResidual( res );
  node.setErrResidual( sqrt(errorRes2) );

  return StatusCode::SUCCESS;
}

//=========================================================================
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
  const TrackVector& prevNodeX = prevNode.predictedState().stateVector();
  const TrackSymMatrix& prevNodeC = prevNode.predictedState().covariance();

  // invert the covariance matrix
  TrackSymMatrix invPrevNodeC = prevNodeC;
  StatusCode sc = invertMatrix( invPrevNodeC );
  if ( sc.isFailure() ) return failure( "unable to invert matrix in smoother" );

  // references to _predicted_ state + cov of this node from the first step
  TrackVector& thisNodeX = thisNode.state().stateVector();
  TrackSymMatrix& thisNodeC = thisNode.state().covariance();

  // Save filtered state vector
  TrackVector oldNodeX = thisNodeX;

  // transport
  const TrackMatrix& F = prevNode.transportMatrix();

  // calculate gain matrix A
  TrackMatrix A = thisNodeC * Transpose( F ) * invPrevNodeC;

  // best = smoothed state at prev Node
  const TrackVector& prevNodeSmoothedX = prevNode.state().stateVector();
  const TrackSymMatrix& prevNodeSmoothedC = prevNode.state().covariance();

  // smooth state
  thisNodeX += A * ( prevNodeSmoothedX - prevNodeX );

  // smooth covariance  matrix
  TrackSymMatrix covUpDate = 
    Similarity<double,TrackMatrix::kRows,TrackMatrix::kCols>
    ( A,  prevNodeSmoothedC - prevNodeC );
  thisNodeC += covUpDate;

  // check that the cov matrix is positive
  sc = checkPositiveMatrix( thisNodeC ) ;
  if ( sc.isFailure() ) {
    std::ostringstream mess;
    mess << "Non-positive cov. matrix in smoother for z = "
         << thisNode.z() << " thisNodeC = "
         << thisNodeC;
    return failure( mess.str() );
  }

  // Only update residuals for node with measurement
  if ( thisNode.hasMeasurement() ) {
    // update = smooth the residuals
    const TrackProjectionMatrix& H = thisNode.projectionMatrix();
    double res = thisNode.residual() - Vector1(H*(thisNodeX - oldNodeX ))(0) ;
    thisNode.setResidual( res );
    double errRes2 = thisNode.errResidual2() - 
      Matrix1x1(Similarity( H, covUpDate ))(0,0);
    if ( errRes2 < 0.) {
      return failure( "Negative residual error in smoother!" );
    }
    thisNode.setErrResidual( sqrt(errRes2) );
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackKalmanFilter::biSmooth( FitNode& thisNode )
{
  // Get the predicted state from the reverse fit
  const TrackVector& predRevX = thisNode.predictedStateRev().stateVector();
  TrackSymMatrix invPredRevC = thisNode.predictedStateRev().covariance();
  StatusCode sc = invertMatrix( invPredRevC );
  if ( sc.isFailure() ) return failure( "inverting matrix in smoother" );

  // Get the filtered state from the forward fit
  const TrackVector& filtStateX = thisNode.state().stateVector();
  TrackSymMatrix invFiltStateC = thisNode.state().covariance();
  sc = invertMatrix( invFiltStateC );
  if ( sc.isFailure() ) return failure( "inverting matrix in smoother" );

  // Add the inverted matrices
  TrackSymMatrix smoothedC = invFiltStateC + invPredRevC;
  sc = invertMatrix( smoothedC );
  if ( sc.isFailure() ) return failure( "inverting matrix in smoother" );

  // Get the smoothed state by calculating the weighted mean
  TrackVector smoothedX = smoothedC *((invPredRevC * predRevX) +
                                      (invFiltStateC * filtStateX)) ;
  (thisNode.state()).setState( smoothedX );
  (thisNode.state()).setCovariance( smoothedC );

  // Only update residuals for node with measurement
  if ( thisNode.hasMeasurement() ) {
    const TrackProjectionMatrix& H = thisNode.projectionMatrix();
    double res = thisNode.projectionTerm() - Vector1(H*smoothedX)(0) ;
    thisNode.setResidual( res );
    double errRes2 = thisNode.errMeasure2() - 
      Matrix1x1(Similarity( H, smoothedC ))(0,0);
    if ( errRes2 < 0.) return failure( "Negative residual error in smoother!" );
    thisNode.setErrResidual( sqrt(errRes2) );
  }

  if ( m_unbiasedResiduals ) {
    // Get the predicted state from the forward fit
    const TrackVector& predStateX = thisNode.predictedState().stateVector();
    TrackSymMatrix invPredStateC = thisNode.predictedState().covariance();
    sc = invertMatrix( invPredStateC );
    if ( sc.isFailure() ) return failure( "inverting matrix in smoother" );
    
    // Add the inverted matrices for the unbiased track parameters
    TrackSymMatrix unbiasedC = invPredStateC + invPredRevC;
    sc = invertMatrix( unbiasedC );
    if ( sc.isFailure() ) return failure( "inverting matrix in smoother" );

    // Get the unbiased state by calculating the weighted mean
    TrackVector unbiasedX = unbiasedC *((invPredRevC * predRevX) +
                                        (invPredStateC * predStateX)) ;

    // Only update unbiased residuals for node with measurement
    if ( thisNode.hasMeasurement() ) {
      const TrackProjectionMatrix& H = thisNode.projectionMatrix();
      double unbiasedRes = thisNode.projectionTerm() - Vector1(H*unbiasedX)(0) ;
      thisNode.setUnbiasedResidual( unbiasedRes );
      double errUnbiasedRes2 = thisNode.errMeasure2() +
        Matrix1x1(Similarity( H, unbiasedC ))(0,0);
      if ( errUnbiasedRes2 < 0.) {
        return failure( "Negative unbiased residual error in smoother!" );
      }
      thisNode.setErrUnbiasedResidual( sqrt(errUnbiasedRes2) );
    }
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
void TrackKalmanFilter::computeChi2( Track& track ) 
{
  double chi2 = 0.;
  int ndof = -track.firstState().nParameters();

  // Loop over the node to calculate total chi2 and # measurements
  const std::vector<Node*>& nodes = track.nodes();
  std::vector<Node*>::const_iterator it = nodes.begin();
  for ( ; it != nodes.end(); ++it ) {
    // a node without a measurement has chi2 = 0 ;-)
    chi2 += (*it)->chi2();
    if ( (*it)->hasMeasurement() ) ++ndof;
  }
  track.setChi2AndDoF( chi2, ndof );
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkInvertMatrix( const TrackSymMatrix& mat ) 
{
  unsigned int nParams = TrackSymMatrix::kRows;
  for ( unsigned int i = 0; i < nParams; ++i ) {
    for ( unsigned int j = 0; j < nParams; ++j ) {
      if ( mat(i,j) > 1e20 )
        return Warning( "Covariance errors too big to invert!",
                        StatusCode::FAILURE, 1 );
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkPositiveMatrix( const TrackSymMatrix& mat ) 
{
  for ( unsigned int i=0; i < TrackSymMatrix::kRows; ++i ) {
    if ( mat(i,i) <= 0. )
      return failure( "Covariance matrix has non-positive elements!" );
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
StatusCode TrackKalmanFilter::invertMatrix( TrackSymMatrix& m )
{
  // check that the elements are not too large else dsinv will crash
  StatusCode sc = checkInvertMatrix( m );
  if ( sc.isFailure() ) return failure( "not valid matrix in smoother" );

  TrackUnitsConverters::convertToG3( m );
  bool OK = m.Invert();
  TrackUnitsConverters::convertToG4( m );

  if ( !OK ) {
    return Warning("Failed to invert covariance matrix", StatusCode::FAILURE,1);
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
