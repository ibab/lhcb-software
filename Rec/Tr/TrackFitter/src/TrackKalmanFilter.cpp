// $Id: TrackKalmanFilter.cpp,v 1.43 2007-09-04 08:34:59 wouter Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// From LHCbMath
#include "LHCbMath/MatrixManip.h"

// from TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/TrackUnitsConverters.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjector.h"

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
  declareProperty( "Projector"     , m_projectorSelectorName =
                   "TrackProjectorSelector" );
  declareProperty( "StoreTransport"   , m_storeTransport    = true   );
  declareProperty( "BiDirectionalFit" , m_biDirectionalFit  = true   );
  
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
  m_projectorSelector = tool<ITrackProjectorSelector>( m_projectorSelectorName,
                                          "Projector", this );
  m_debugLevel   = msgLevel( MSG::DEBUG );
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// Helper to print a failure comment
//=========================================================================
StatusCode TrackKalmanFilter::failure( const std::string& comment ) const {
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
  bool upstream = track.nodes().front()->z() > track.nodes().back()->z() ;

  // Set the reference vector (initially this is taken from the seed state)
  TrackVector refVec = state.stateVector();

  // Loop over the nodes in the current order (should be sorted)
  // ==> prediction and filter
  double chisq(0) ;
  int ndof(-state.nParameters()) ;

  for ( ; iNode != nodes.end(); ++iNode) {
    FitNode& node = dynamic_cast<FitNode&>(**iNode);

    // Prediction step
    sc = predict( node, state, refVec );
    if ( sc.isFailure() ) return failure( "unable to predict node" );

    // save predicted state
    if ( upstream ) node.setPredictedStateUp( state );
    else            node.setPredictedStateDown( state );

    // update the reference vector
    refVec = state.stateVector();

    if ( node.hasMeasurement() ) {
      // Filter step
      sc = filter( node, state );
      if ( sc.isFailure() ) return failure( "unable to filter node" );

      // update the reference vector
      refVec = node.measurement().refVector();

      // add the chisquare
      chisq += node.chi2();
      if ( upstream ) node.setDeltaChi2Upstream( node.chi2() );
      else            node.setDeltaChi2Downstream( node.chi2() ) ;
      ++ndof ;
    }

    // save filtered state
    node.setState( state );

  } // end of prediction and filter

  // Run the bidirectional fit
  if ( m_biDirectionalFit ) {

    // Reset the covariance matrix and chisquare
    double chisqreverse(0) ;
    state.setCovariance( seedCov );

    // Loop over the nodes in the revers order (should be sorted)
    std::vector<Node*>::reverse_iterator irNode = nodes.rbegin();
    std::vector<Node*>::reverse_iterator irPrevNode = irNode;
    if ( irNode == nodes.rend() ) return failure( "zero nodes left" );
    while ( irNode != nodes.rend() ) {
      FitNode& node     = dynamic_cast<FitNode&>(**irNode);
      FitNode& prevNode = dynamic_cast<FitNode&>(**irPrevNode);

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
      if ( !upstream ) node.setPredictedStateUp( state );
      else             node.setPredictedStateDown( state );

      // set the reference vector
      refVec = state.stateVector();

      if ( node.hasMeasurement() ) {
        // Filter step
        sc = filter( node, state );
        if ( sc.isFailure() ) return failure( "unable to filter node!" );
	
        // update the reference vector and the chisquare
        refVec = node.measurement().refVector();

	// add the chisquare
	chisqreverse += node.chi2() ;
	if ( !upstream ) node.setDeltaChi2Upstream( node.chi2() );
	else             node.setDeltaChi2Downstream( node.chi2() ) ;
      }

      // save filtered state
      if ( !upstream ) node.setState( state );

      // Smoother step
      sc = biSmooth( node );
      if ( sc.isFailure() ) return failure( "unable to biSmooth node!" );

      irPrevNode = irNode;
      ++irNode;
    } // end of prediction and filter
    track.setFitHistory( Track::BiKalman );
    // Ideally, the chisquares of the two directions are equal. For
    // convergence and breakpoint analysis we rather have the maximum
    // if they differ.
    if( chisq < chisqreverse ) chisq = chisqreverse ;
  } else {
   // Loop in opposite direction for the old RTS smoother
    std::vector<Node*>::reverse_iterator iPrevNode = nodes.rbegin();
    std::vector<Node*>::reverse_iterator ithisNode = iPrevNode;
    ++ithisNode;
    while ( iPrevNode != nodes.rend() && ithisNode != nodes.rend() ) {
      FitNode& prevNode = dynamic_cast<FitNode&>(**iPrevNode);
      FitNode& thisNode = dynamic_cast<FitNode&>(**ithisNode);
      // Smoother step
      sc = smooth( thisNode, prevNode, upstream );
      if ( sc.isFailure() ) return failure( "unable to smooth node!" );
      ++ithisNode;
      ++iPrevNode;
    }
    track.setFitHistory( Track::StdKalman );
  }

  // set the chisquare
  track.setChi2AndDoF( chisq, ndof );

  return sc;
}

//=========================================================================
// Predict the state to this node
//=========================================================================
StatusCode TrackKalmanFilter::predict( FitNode& aNode, State& aState,
                                       const Gaudi::TrackVector& refVec ) const
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
    TrackMatrix transMat = TrackMatrix( ROOT::Math::SMatrixIdentity() );
    StatusCode sc = m_extrapolator -> propagate( aState, z, &transMat );
    if ( sc.isFailure() ) {
      std::ostringstream mess;
      mess << "unable to predict state at next measurement at z = " << z;
      return failure( mess.str() );
    }
    aState.setLocation( (aNode.state()).location() );
    
    // store transport matrix F
    const TrackMatrix& F = transMat;
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
                                                State& aState) const
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
StatusCode TrackKalmanFilter::project(FitNode& aNode, const State& aState) const
{
  // project the state into the measurement
  Measurement& meas = aNode.measurement();
  ITrackProjector *proj = m_projectorSelector->projector(meas);
  if ( proj==0 )
    return failure( "could not get projector for measurement" );

  StatusCode sc = proj -> project( aState, meas );
  if ( sc.isFailure() ) 
    return failure( "unable to project a state into a measurement" );
  const TrackProjectionMatrix& H = proj -> projectionMatrix();
  const double res                     = proj -> residual();
  
  aNode.setProjectionMatrix( H );
  aNode.setResidual( res ) ;
  aNode.setErrResidual( proj -> errResidual() ) ;
  aNode.setErrMeasure( proj -> errMeasure() ) ;
  aNode.setProjectionTerm( res + Vector1(H*aState.stateVector())(0) );
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::filter(FitNode& node, State& state) const
{
  // Projection step
  StatusCode sc = project( node, state );
  if ( sc.isFailure() ) return sc;

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

  // update the covariance matrix. The original expression is
  //   static const TrackSymMatrix unit = TrackSymMatrix( SMatrixIdentity());
  //   static SymMatrix1x1 V ;
  //   V(0,0) = errorMeas2 ;
  //   C = Similarity( unit - ( K*H ), C ) + Similarity(K,V) ;
  // but the following expression is just as stable and less complex
  // (see NIM.A552:566-575,2005).
  static SymMatrix1x1 R ;
  R(0,0) = errorRes2;
  TrackSymMatrix tmp ;
  ROOT::Math::AssignSym::Evaluate(tmp, -2 * K * H * C) ;
  C += tmp + Similarity(K,R) ;  

  // update the residual and the error on the residual
  const double gain = 1.- Matrix1x1( H*K )(0,0);
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
                                      const FitNode& prevNode,
				      bool upstream ) const
{
  // Get the predicted state from the previous node
  const TrackVector& prevNodeX = (upstream) ? 
    prevNode.predictedStateUp().stateVector() :
    prevNode.predictedStateDown().stateVector();
  const TrackSymMatrix& prevNodeC = (upstream) ?
    prevNode.predictedStateUp().covariance() :
    prevNode.predictedStateDown().covariance();

  // invert the covariance matrix
  TrackSymMatrix invPrevNodeC = prevNodeC;
  StatusCode sc = invertMatrix( invPrevNodeC );
  if ( sc.isFailure() ) return failure( "unable to invert matrix in smoother" );

  // Get the filtered result from this node
  TrackVector& thisNodeX = thisNode.state().stateVector();
  TrackSymMatrix& thisNodeC = thisNode.state().covariance();

  // Save filtered state vector
  TrackVector oldNodeX = thisNodeX;

  // transport
  const TrackMatrix& F = prevNode.transportMatrix();

  // calculate gain matrix A
  TrackMatrix A = thisNodeC * Transpose( F ) * invPrevNodeC;

  // Get the smoothed state of the previous node
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
         << thisNode.z() << " thisNodeC = " << thisNodeC;
    return failure( mess.str() );
  }

  // Only update residuals for node with measurement
  if ( thisNode.hasMeasurement() ) {
    // update = smooth the residuals
    const TrackProjectionMatrix& H = thisNode.projectionMatrix();
    const double res = thisNode.residual() - Vector1(H*(thisNodeX - oldNodeX ))(0) ;
    thisNode.setResidual( res );
    const double errRes2 = thisNode.errResidual2() - 
      Matrix1x1(Similarity( H, covUpDate ))(0,0);
    if ( errRes2 < 0.) {
      return failure( "Negative residual error in smoother!" );
    }
    thisNode.setErrResidual( sqrt(errRes2) );
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackKalmanFilter::biSmooth( FitNode& thisNode ) const
{
  // Get the predicted state from the downstream fit
  const TrackVector& predRevX = thisNode.predictedStateDown().stateVector();
  const TrackSymMatrix& predRevC = thisNode.predictedStateDown().covariance();
  // Get the filtered state from the upstream fit
  const TrackVector& filtStateX = thisNode.state().stateVector();
  const TrackSymMatrix& filtStateC = thisNode.state().covariance();
  // Calculate the gain matrix. Start with inverting the cov matrix of the difference.
  TrackSymMatrix invR = filtStateC + predRevC ;
  StatusCode sc = invertMatrix( invR );
  if ( sc.isFailure() ) return failure( "inverting matrix in smoother" );
  TrackVector    smoothedX ;
  TrackSymMatrix smoothedC ;
  // Now we need to choose wisely which state is the reference. (In
  // a perfect world it would not make a difference.)
  if( filtStateC(0,0) < predRevC(0,0) ) {
    SMatrix<double,5,5> K = filtStateC * invR ;
    smoothedX = filtStateX + K * (predRevX - filtStateX) ;
    ROOT::Math::AssignSym::Evaluate(smoothedC, K * predRevC ) ;
  } else {
    SMatrix<double,5,5> K = predRevC * invR ;
    smoothedX    = predRevX + K * (filtStateX - predRevX) ;
    ROOT::Math::AssignSym::Evaluate(smoothedC, K * filtStateC ) ;
  }
  (thisNode.state()).setState( smoothedX );
  (thisNode.state()).setCovariance( smoothedC );

  // Only update residuals for node with measurement
  if ( thisNode.hasMeasurement() ) {
    const TrackProjectionMatrix& H = thisNode.projectionMatrix();
    const double res = thisNode.projectionTerm() - Vector1(H*smoothedX)(0) ;
    thisNode.setResidual( res );
    const double errRes2 = thisNode.errMeasure2() - 
      Matrix1x1(Similarity( H, smoothedC ))(0,0);
    if ( errRes2 < 0.) return failure( "Negative residual error in smoother!" );
    thisNode.setErrResidual( sqrt(errRes2) );
  }
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::checkInvertMatrix( const Gaudi::TrackSymMatrix& mat ) const
{

  for ( unsigned int i = 0; i < TrackSymMatrix::kRows ; ++i ) {
    for ( unsigned int j = 0; j <=i; ++j ) {
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
StatusCode TrackKalmanFilter::checkPositiveMatrix( const Gaudi::TrackSymMatrix& mat ) const 
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
StatusCode TrackKalmanFilter::invertMatrix( Gaudi::TrackSymMatrix& m ) const
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
