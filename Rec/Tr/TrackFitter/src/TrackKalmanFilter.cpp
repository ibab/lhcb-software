// $Id: TrackKalmanFilter.cpp,v 1.28 2006-08-02 15:01:36 erodrigu Exp $
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
using namespace ROOT::Math;
using ROOT::Math::SMatrix;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : TrackKalmanFilter
//
//  Original Author: Rutger van der Eijk
//  Created: 07-04-1999
//  Adapted: 21-03-2002  Olivier Callot
//  Adapted: 15-04-2005  Jose A. Hernando, Eduardo Rodrigues
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
  declareProperty( "StoreTransport"  , m_storeTransport   = true   );
  declareProperty( "BiDirectionalFit", m_biDirectionalFit = true   );

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

  // Loop over the nodes in the current order (should be sorted)
  // ==> prediction and filter
  for ( ; iNode != nodes.end(); ++iNode) {
    FitNode& node = dynamic_cast<FitNode&>(**iNode);

    // Prediction step
    sc = predict( node, state );
    if ( sc.isFailure() ) return failure( "unable to predict node" );

    // save predicted state
    node.setPredictedState( state );

    if ( node.hasMeasurement() ) {
      // Projection step
      sc = project( node, state );
      if ( sc.isFailure() ) return failure( "unable to project node" );

      // Filter step
      sc = filter( node, state );
      if ( sc.isFailure() ) return failure( "unable to filter node" );
    }

    // save filtered state
    node.setState( state );
  } // end of prediction and filter

  // Do the bidirectional fit 
  if ( m_biDirectionalFit ) { 

    // Reset the covariance matrix
    state.setCovariance( seedCov );

    // Project and filter the first node
    std::vector<Node*>::reverse_iterator irNode = nodes.rbegin();
    if ( irNode == nodes.rend() ) return failure( "zero nodes left" );
    FitNode& firstNode = *(dynamic_cast<FitNode*>(*irNode));
      
    // save predicted state
    firstNode.setBiState( state );
    
    if ( firstNode.hasMeasurement() ) {
      // Projection step
      sc = project( firstNode, state );
      if ( sc.isFailure() ) return failure( "unable to project first node" );
      
      // Filter step
      sc = filter( firstNode, state );
      if ( sc.isFailure() ) return failure( "unable to filter first node " );
    }

    // Loop over the nodes in the revers order (should be sorted)
    std::vector<Node*>::reverse_iterator irPrevNode = irNode;
    ++irNode;
    while ( irNode != nodes.rend() ) {
      FitNode& node     = *(dynamic_cast<FitNode*>(*irNode));
      FitNode& prevNode = *(dynamic_cast<FitNode*>(*irPrevNode));

      // Prediction step to next node
      if ( m_storeTransport ) {
        sc = predictReverseFit( prevNode, node, state );
        if ( sc.isFailure() )
          return failure( "unable to predict (reverse fit)  node" );
      } else {
        sc = predict( node, state );
        if ( sc.isFailure() ) return failure( "unable to predict node" );
      }

      // save predicted state
      node.setBiState( state );

      if ( node.hasMeasurement() ) {
        // Projection step
        sc = project( node, state );
        if ( sc.isFailure() ) return failure( "unable to project node" );
      
        // Filter step
        sc = filter( node, state );
        if ( sc.isFailure() ) return failure( "unable to filter node!" );
      }

      // Smoother step
      sc = biSmooth( node );

      ++irNode;
      ++irPrevNode;
    } // end of prediction and filter
  }
  else {
    // Loop in opposite direction for the smoother
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
  }
  
  // Compute the chi2 and degrees of freedom
  computeChi2( track );

  // set the FitHistory flag to "Kalman fit done"
  if ( m_biDirectionalFit )
    track.setFitHistory( Track::BiKalman );
  else
    track.setFitHistory( Track::StdKalman );

  return sc;
}

//=========================================================================
// Predict the state to this node
//=========================================================================
StatusCode TrackKalmanFilter::predict(FitNode& aNode, State& aState )
{
  TrackVector prevStateVec = aState.stateVector();
  TrackSymMatrix prevStateCov = aState.covariance();
  double z = aNode.z();

  // first iteration only: need to call extrapolator to get 
  // the predicted state at measurement
  if ( !(aNode.transportIsSet()) || !m_storeTransport ) {

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
  const TrackMatrix& F = prevNode.transportMatrix();

  // invert the covariance matrix
  TrackMatrix invF = F;
  if ( !(invF.Invert()) )
    return failure( "unable to invert matrix in prediction" );

  // Get state vector
  TrackVector& stateVec = aState.stateVector();
  TrackVector tempVec( stateVec );
  stateVec = invF * ( tempVec - prevNode.transportVector() );

  // Invert noise matrix
  TrackSymMatrix noise = prevNode.noiseMatrix();
  noise(0,2) = - noise(0,2);
  noise(0,3) = - noise(0,3);
  noise(1,2) = - noise(1,2);
  noise(1,3) = - noise(1,3);

  // Calculate the predicted covariance
  TrackSymMatrix& stateCov = aState.covariance();
  TrackSymMatrix tempCov = stateCov;
  stateCov = Similarity( invF, tempCov ) + noise;

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
    Warning( "Z positions of State and Measurement are not equal", 0 );
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

  // check that the elements are not too large else dsinv will crash
  StatusCode sc = checkInvertMatrix( prevNodeC );
  if ( sc.isFailure() ) return failure( "not valid matrix in smoother" );

  // invert the covariance matrix
  TrackSymMatrix invPrevNodeC = prevNodeC;
  sc = invertMatrix( invPrevNodeC );
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

  // No need to update residuals for node w/o measurement
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
  const TrackVector& biNodeX = thisNode.biState().stateVector();
  const TrackSymMatrix& biNodeC = thisNode.biState().covariance();

  // check that the elements are not too large else dsinv will crash
  StatusCode sc = checkInvertMatrix( biNodeC );
  if ( sc.isFailure() ) return failure( "not valid matrix in smoother" );

  // invert the covariance matrix
  TrackSymMatrix invBiNodeC = biNodeC;
  sc = invertMatrix( invBiNodeC );
  if ( sc.isFailure() ) return failure( "inverting matrix in smoother" );

  // references to _predicted_ state + cov of this node from the first step
  TrackVector& thisNodeX = thisNode.state().stateVector();
  TrackSymMatrix& thisNodeC = thisNode.state().covariance();

  // check that the elements are not too large else dsinv will crash
  sc = checkInvertMatrix( thisNodeC );
  if ( sc.isFailure() ) return failure( "not valid matrix in smoother" );

  // invert the covariance matrix
  TrackSymMatrix invThisNodeC = thisNodeC;
  sc = invertMatrix( invThisNodeC );
  if ( sc.isFailure() ) {
    return failure( "inverting matrix in smoother" );
  }

  // Add the inverted matrices
  TrackSymMatrix sumInvNodeC = invThisNodeC + invBiNodeC;

  // check that the elements are not too large else dsinv will crash
  sc = checkInvertMatrix( sumInvNodeC );
  if ( sc.isFailure() ) return failure( "not valid matrix in smoother" );

  // invert the covariance matrix
  TrackSymMatrix smoothedC = sumInvNodeC;
  sc = invertMatrix( smoothedC );
  if ( sc.isFailure() ) {
    return failure( "inverting matrix in smoother" );
  }

  // Get the smoothed state by calculateing the weighted mean
  TrackVector smoothedX = smoothedC *((invBiNodeC * biNodeX) +
                                      (invThisNodeC * thisNodeX)) ;
  (thisNode.state()).setState( smoothedX );
  (thisNode.state()).setCovariance( smoothedC );

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

  double chi2Norma = ( ndof != 0 ) ? chi2/((double) (ndof)) : 0.0;
  track.setChi2PerDoF( chi2Norma );
  track.setNDoF( ndof );
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
        return Warning( "Covariance errors too big to invert!" );
    }
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkPositiveMatrix( TrackSymMatrix& mat ) 
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
  TrackUnitsConverters::convertToG3( m );
  bool OK = m.Invert();
  TrackUnitsConverters::convertToG4( m );

  if ( !OK ) {
    return Warning( "Failed to invert covariance matrix", StatusCode::FAILURE );
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
