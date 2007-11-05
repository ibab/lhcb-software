// $Id: TrackKalmanFilter.cpp,v 1.48 2007-11-05 16:33:23 mneedham Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// From LHCbMath
#include "LHCbMath/MatrixManip.h"

// from TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/TrackUnitsConverters.h"
#include "Event/StateVector.h"

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
  declareProperty( "checked", m_checked = false);
  
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
// Fit the track
//=========================================================================
StatusCode TrackKalmanFilter::fit( Track& track ) 
{
  StatusCode sc(StatusCode::SUCCESS, true); 
 
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
  if( msgLevel( MSG::VERBOSE ) )
    verbose() << "Running forward filter" << endmsg ;

  for ( ; iNode != nodes.end(); ++iNode) {
    FitNode& node = dynamic_cast<FitNode&>(**iNode);

    // Prediction step
    sc = predict( node, state, refVec );
    if ( sc.isFailure() ) return Warning( "Fit failure: unable to predict node", StatusCode::FAILURE,1 );

    // save predicted state
    if ( upstream ) node.setPredictedStateUp( state );
    else            node.setPredictedStateDown( state );

    // update the reference vector
    refVec = state.stateVector();

    if ( node.hasMeasurement() ) {
      // if the reference is not set, set it to the prediction.
      if( !node.measurement().refIsSet() )
        node.measurement().setRefVector( refVec ) ;
      
      // Project the reference (only in the forward filter)
      sc = projectReference( node );
      if ( sc.isFailure() ) return Warning( "Fit Failure: unable to project reference", StatusCode::FAILURE,1 );

      // Filter step
      sc = filter( node, state );
      if ( sc.isFailure() ) return Warning( "Fit failure:unable to filter node", StatusCode::FAILURE, 1 );

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

    if( msgLevel( MSG::VERBOSE ) )
      verbose() << "Running backward filter" << endmsg ;
    
    // Reset the covariance matrix and chisquare
    double chisqreverse(0) ;
    state.setCovariance( seedCov );

    // Loop over the nodes in the revers order (should be sorted)
    std::vector<Node*>::reverse_iterator irNode = nodes.rbegin();
    std::vector<Node*>::reverse_iterator irPrevNode = irNode;
    if ( irNode == nodes.rend() ) return Warning( "Fit failure: zero nodes left" ,StatusCode::FAILURE,  1);
    while ( irNode != nodes.rend() ) {
      FitNode& node     = dynamic_cast<FitNode&>(**irNode);
      FitNode& prevNode = dynamic_cast<FitNode&>(**irPrevNode);

      // Prediction step to next node
      if ( irPrevNode != irNode ) { // No prediction needed for 1st node
        if ( m_storeTransport ) {
          sc = predictReverseFit( prevNode, node, state );
          if ( sc.isFailure() ) 
            return Warning( "Fit failure unable to predict (reverse fit) node",StatusCode::FAILURE, 1 );
        } else {
          sc = predict( node, state, refVec );
          if ( sc.isFailure() ) return Warning( "Fit Failure: unable to predict node",StatusCode::FAILURE,1  );
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
        if ( sc.isFailure() ) return Warning( "Fit Failure unable to filter node!",StatusCode::FAILURE, 1 );
	
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
      if ( sc.isFailure() ) return Warning( "Fit failure: unable to biSmooth node!",StatusCode::FAILURE, 1 );

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
      if ( sc.isFailure() ) return Warning( "Fit failure: unable to smooth node!", StatusCode::FAILURE,1 );
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
  
  if( msgLevel( MSG::VERBOSE ) )
    verbose() << " after predict state = " << aState << endmsg ;

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

  if( msgLevel( MSG::VERBOSE ) )
    verbose() << " after predictReverseFit state = " << aState << endmsg ;

  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::projectReference(FitNode& aNode) const
{
  // project the reference state
  Measurement& meas = aNode.measurement();
  ITrackProjector *proj = m_projectorSelector->projector(meas);
  if ( proj==0 )
    return Warning( "could not get projector for measurement", StatusCode::FAILURE, 1 );
  
  StatusCode sc = proj -> project(LHCb::StateVector(meas.refVector(),meas.z()), meas );
  if ( sc.isFailure() ) 
    return Warning( "unable to project a state into a measurement", StatusCode::FAILURE,1 );
   
  aNode.setProjectionMatrix( proj->projectionMatrix() );
  aNode.setRefResidual( proj -> residual() ) ;
  aNode.setErrMeasure( proj -> errMeasure() ) ;
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::filter(FitNode& node, State& state) const
{
  // check z position
  Measurement& meas = node.measurement();
  if ( fabs(meas.z() - state.z()) > TrackParameters::propagationTolerance ) {
    Warning( "Z positions of State and Measurement are not equal", StatusCode::FAILURE, 1 );
    debug() << "State at z=" << state.z() 
            << ", Measurement at z=" << meas.z() << endmsg;
  }

  // get reference to the state vector and cov
  TrackVector&    X = state.stateVector();
  TrackSymMatrix& C = state.covariance();

  // calculate the linearized residual of the prediction and its error
  const TrackProjectionMatrix& H = node.projectionMatrix();
  const double errorMeas2 = node.errMeasure2();
  double res        = node.refResidual() + ( H * (meas.refVector() - X) ) (0) ;
  double errorRes2  = errorMeas2 + Similarity(H,C)(0,0) ;  

  // calculate gain matrix K
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

  if( msgLevel( MSG::VERBOSE ) )
    verbose() << " chisquare of node = " << node.chi2() << std::endl
	      << " filtered state = " << state << endmsg ;
  
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
  if (m_checked == true){
    sc = checkPositiveMatrix( thisNodeC ) ;
    if ( sc.isFailure() ) {
      std::ostringstream mess;
      mess << "Non-positive cov. matrix in smoother for z = "
         << thisNode.z() << " thisNodeC = " << thisNodeC;
      Warning("Problems in smoothing",StatusCode::FAILURE,1);
      return failure( mess.str() );
    }
  } // only if checked

  // Only update residuals for node with measurement
  if ( thisNode.hasMeasurement() ) {
    // update = smooth the residuals
    const TrackProjectionMatrix& H = thisNode.projectionMatrix();
    const double res = thisNode.residual() - Vector1(H*(thisNodeX - oldNodeX ))(0) ;
    thisNode.setResidual( res );
    const double errRes2 = thisNode.errResidual2() - 
      Matrix1x1(Similarity( H, covUpDate ))(0,0);
    if ( errRes2 < 0.) {
      return Warning( "Negative residual error in smoother!", StatusCode::SUCCESS, 1 );
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
  if ( sc.isFailure() ) return Warning( "Failure inverting matrix in smoother", StatusCode::FAILURE, 1 );
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
    const TrackVector& refX = thisNode.measurement().refVector() ;
    const double res = thisNode.refResidual() + ( H*(refX - smoothedX) )(0) ;
    thisNode.setResidual( res );
    const double errRes2 = thisNode.errMeasure2() - 
      Matrix1x1(Similarity( H, smoothedC ))(0,0);
    if ( errRes2 < 0.) return Warning( "Negative residual error in smoother!",StatusCode::FAILURE ,1 );
    thisNode.setErrResidual( sqrt(errRes2) );
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
  if (m_checked) {
    StatusCode sc = checkInvertMatrix( m );
    return sc;
  }
  TrackUnitsConverters::convertToG3( m );
  const bool OK = m.Invert();
  TrackUnitsConverters::convertToG4( m );

  return OK ? StatusCode::SUCCESS: Warning("Failed to invert covariance matrix", StatusCode::FAILURE,1);
}

//=========================================================================
