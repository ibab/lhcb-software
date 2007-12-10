// $Id: TrackKalmanFilter.cpp,v 1.53 2007-12-10 16:01:52 wouter Exp $
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

  declareProperty( "BiDirectionalFit" , m_biDirectionalFit  = true   );
  declareProperty( "Smooth", m_smooth = true ) ;
  declareProperty( "checked", m_checked = true);
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

  m_projectorSelector = tool<ITrackProjectorSelector>( "TrackProjectorSelector", "Projector", this );

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
  bool upstream = track.nodes().front()->z() > track.nodes().back()->z() ;
  
  // Set the reference vector and seed covariance (initially this is taken from the seed state)
  std::vector<Node*>::iterator iNode = nodes.begin();
  State state = (*iNode)->state() ;
  TrackSymMatrix seedCov = state.covariance();
  
  // Loop over the nodes in the current order (should be sorted)
  // ==> prediction and filter
  double chisq(0) ;
  int ndof(-state.nParameters()) ;
  if( msgLevel( MSG::VERBOSE ) )
    verbose() << "Running forward filter" << endmsg ;

  for ( ; iNode != nodes.end(); ++iNode) {
    FitNode& node = dynamic_cast<FitNode&>(**iNode);

    // Prediction step (not for first node)
    if( iNode != nodes.begin() ) {
      sc = predict( node, state );
      if ( sc.isFailure() ) return Warning( "Fit failure: unable to predict node", StatusCode::FAILURE,1 );
    }
    
    // save predicted state
    if ( upstream ) node.setPredictedStateUp( state );
    else            node.setPredictedStateDown( state );

    // filter if there is a measurement
    if ( node.hasMeasurement() ) {
      // Project the reference (only in the forward filter)
      sc = projectReference( node );
      if ( sc.isFailure() ) return Warning( "Fit Failure: unable to project reference", StatusCode::FAILURE,1 );

      // Filter step
      sc = filter( node, state );
      if ( sc.isFailure() ) return Warning( "Fit failure:unable to filter node", StatusCode::FAILURE, 1 );

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
        sc = predictReverseFit( prevNode, node, state );
        if ( sc.isFailure() ) 
          return Warning( "Fit failure unable to predict (reverse fit) node",StatusCode::FAILURE, 1 );
      }

      // save predicted state
      if ( !upstream ) node.setPredictedStateUp( state );
      else             node.setPredictedStateDown( state );

      // filter, if there is a measuerment
      if ( node.hasMeasurement() ) {
        // Filter step
        sc = filter( node, state );
        if ( sc.isFailure() ) return Warning( "Fit Failure unable to filter node!",StatusCode::FAILURE, 1 );
	
        // add the chisquare
        chisqreverse += node.chi2() ;
        if ( !upstream ) node.setDeltaChi2Upstream( node.chi2() );
        else             node.setDeltaChi2Downstream( node.chi2() ) ;
      }
      
      // save filtered state, but not for the first node. we also
      // don't smooth for the first state.
      if( irNode != nodes.rbegin() ) {
	if ( !upstream  ) node.setState( state );
	
	// Smoother step
	if(m_smooth ) {
	  // If this is the last reverse node, the smoothed state is just this state.
	  if( *irNode == nodes.front() ) {
	    node.setState( state );
	  } else {
	    sc = biSmooth( node, upstream );
	    if ( sc.isFailure() ) return Warning( "Fit failure: unable to biSmooth node!",StatusCode::FAILURE, 1 );
	  }
	}
      }
      irPrevNode = irNode;
      ++irNode;
    } // end of prediction and filter
    track.setFitHistory( Track::BiKalman );
    // Ideally, the chisquares of the two directions are equal. For
    // convergence and breakpoint analysis we rather have the maximum
    // if they differ.
    if( chisq < chisqreverse ) chisq = chisqreverse ;
  } else if (m_smooth) {
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
StatusCode TrackKalmanFilter::predict( FitNode& aNode, State& aState ) const
{
  double z = aNode.z();
  // don't do the math if we are already there
  if( fabs(z - aState.z()) > TrackParameters::propagationTolerance) {
    TrackVector prevStateVec = aState.stateVector();
    TrackSymMatrix prevStateCov = aState.covariance();
    
    // next iterations: update node with transport information 
    // from 1st iteration to save CPU time
    const TrackMatrix& F = aNode.transportMatrix();
    TrackVector& stateVec = aState.stateVector();
    TrackSymMatrix& stateCov = aState.covariance();
    TrackVector tempVec( stateVec );
    stateVec = F * tempVec + aNode.transportVector();
    stateCov = Similarity( F, stateCov ) + aNode.noiseMatrix();
  }
  aState.setZ( z );
  aState.setLocation( (aNode.state()).location() );
  
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
  double z = aNode.z() ;
  //assert(fabs( prevNode.z() - aState.z()) < TrackParameters::propagationTolerance) ;
  if( fabs(z - aState.z()) > TrackParameters::propagationTolerance ) {
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
  }
  aState.setZ( aNode.z() );
  aState.setLocation( (aNode.state()).location() );

  if( msgLevel( MSG::VERBOSE ) )
    verbose() << " after predictReverseFit state = " << aState << endmsg ;

  return StatusCode::SUCCESS;
}

//=========================================================================
// 
//=========================================================================
StatusCode TrackKalmanFilter::projectReference(FitNode& node) const
{
  // if the reference is not set, issue an error
  StatusCode sc = StatusCode::SUCCESS;
  if( !node.refIsSet() ) {
    error() << "Node without reference. " << node.measurement().type() << endmsg ;
    sc = StatusCode::FAILURE ;
  } else {
    // project the reference state
    Measurement& meas = node.measurement();
    ITrackProjector *proj = m_projectorSelector->projector(meas);
    if ( proj==0 ) {
      error() << "could not get projector for measurement" << endreq ;
      sc = StatusCode::FAILURE ;
    } else {
      sc = proj -> project(node.refVector(), meas );
      if ( sc.isFailure() ) {
        error() << "unable to project this statevector" << endreq ;
      } else {
        node.setProjectionMatrix( proj->projectionMatrix() );
        node.setRefResidual( proj -> residual() ) ;
        node.setErrMeasure( proj -> errMeasure() ) ;
      }
    }
  }
  return sc;
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
  double res        = node.refResidual() + ( H * (node.refVector().parameters() - X) ) (0) ;
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

StatusCode TrackKalmanFilter::biSmooth( FitNode& thisNode, bool upstream ) const
{
  // Get the predicted state from the reverse fit
  const TrackVector& predRevX = thisNode.predictedStateDown().stateVector();
  const TrackSymMatrix& predRevC = thisNode.predictedStateDown().covariance();
  // Get the filtered state from the forward fit
  const TrackVector& filtStateX = thisNode.state().stateVector();
  const TrackSymMatrix& filtStateC = thisNode.state().covariance();
  // Calculate the gain matrix. Start with inverting the cov matrix of the difference.
  TrackSymMatrix invR = filtStateC + predRevC ;
  StatusCode sc = invertMatrix( invR );
  if ( sc.isFailure() ) {
    error() << "unable to invert matrix in smoother" << invR << endreq ;
    return Warning( "Failure inverting matrix in smoother", StatusCode::FAILURE, 1 );
  }
  
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
    const TrackVector& refX = thisNode.refVector().parameters() ;
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
    if (sc.isFailure())  return sc;
  }
  TrackUnitsConverters::convertToG3( m );
  const bool OK = m.Invert();
  TrackUnitsConverters::convertToG4( m );

  return OK ? StatusCode::SUCCESS: Warning("Failed to invert covariance matrix", StatusCode::FAILURE,1);
}

//=========================================================================


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
