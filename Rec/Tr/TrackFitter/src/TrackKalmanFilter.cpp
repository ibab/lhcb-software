// $Id: TrackKalmanFilter.cpp,v 1.66 2008-09-05 09:49:48 wouter Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// From LHCbMath
#include "LHCbMath/MatrixManip.h"
#include "LHCbMath/MatrixInversion.h"

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
// Fit the nodes
//=========================================================================
StatusCode TrackKalmanFilter::fit( LHCb::Track& track, NodeRange& nodes, const Gaudi::TrackSymMatrix& seedCov) const
{
  StatusCode sc(StatusCode::SUCCESS, true); 
  
  // First node must have a seed state
  // Set the initial state. The vector comes from the first node in
  // the list. The seed covariance from the first node on the tr ack.
  NodeRange::iterator iNode = nodes.begin();
  State state = (*iNode)->state() ;
  state.setCovariance(seedCov) ;

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
      if ( sc.isFailure() ) return Warning( "Fit failure: unable to predict node", StatusCode::FAILURE, 0 );
    }
    
    // save predicted state
    node.setPredictedStateForward( state ) ;

    // filter if there is a measurement
    if ( node.hasMeasurement() ) {
      // Project the reference (only in the forward filter)
      sc = projectReference( node );
      if ( sc.isFailure() ) return Warning( "Fit Failure: unable to project reference", StatusCode::FAILURE, 0 );

      // Filter step
      sc = filter( node, state );
      if ( sc.isFailure() ) return Warning( "Fit failure:unable to filter node", StatusCode::FAILURE, 0 );

      // add the chisquare
      chisq += node.chi2();
      node.setDeltaChi2Forward( node.chi2() );
      ++ndof ;
    } else {
      // this should actually be done somewhere else
      node.setResidual( 0 );
      node.setDeltaChi2Forward( 0 );
      node.setDeltaChi2Backward( 0 ) ;
    }

    // save filtered state
    node.setState( state );

  } // end of prediction and filter

  // Run the bidirectional fit
  NodeRange::reverse_iterator rbegin = NodeRange::reverse_iterator(nodes.end()) ; // nodes.rbegin() ;<-- workaround for LoKi::Range_ bug
  NodeRange::reverse_iterator rend = NodeRange::reverse_iterator(nodes.begin()) ;  // nodes.rend() ;
  if ( m_biDirectionalFit ) {

    if( msgLevel( MSG::VERBOSE ) )
      verbose() << "Running backward filter" << endmsg ;
    
    // Reset the covariance matrix and chisquare
    double chisqreverse(0) ;
    state.setCovariance( seedCov );

    // Loop over the nodes in the revers order (should be sorted) 
    NodeRange::reverse_iterator irNode = rbegin ;
    NodeRange::reverse_iterator irPrevNode = irNode;
    if ( irNode == rend ) return Warning( "Fit failure: zero nodes left" ,StatusCode::FAILURE, 0);
    while ( irNode != rend ) {
      FitNode& node     = dynamic_cast<FitNode&>(**irNode);
      FitNode& prevNode = dynamic_cast<FitNode&>(**irPrevNode);

      // Prediction step to next node
      if ( irPrevNode != irNode ) { // No prediction needed for 1st node
        sc = predictReverseFit( prevNode, node, state );
        if ( sc.isFailure() ) 
          return Warning( "Fit failure unable to predict (reverse fit) node",StatusCode::FAILURE, 0);
      }

      // save predicted state
      node.setPredictedStateBackward( state ) ;

      // filter, if there is a measuerment
      if ( node.hasMeasurement() ) {
        // Filter step
        sc = filter( node, state );
        if ( sc.isFailure() ) return Warning( "Fit Failure unable to filter node!",StatusCode::FAILURE, 1 );
	
        // add the chisquare
        chisqreverse += node.chi2() ;
	node.setDeltaChi2Backward( node.chi2() );
      }
      
      // save filtered state, but not for the first node. we also
      // don't smooth for the first state.
      if( irNode != rbegin ) {
	
	// Smoother step
	if(m_smooth ) {
	  // If this is the last reverse node, the smoothed state is just this state.
	  if( *irNode == nodes.front() ) {
	    node.setState( state );
	  } else {
	    sc = biSmooth( node );
	    if ( sc.isFailure() ) return Warning( "Fit failure: unable to biSmooth node!",StatusCode::FAILURE, 0 );
	  }
	}
      }

      // Whatever the logic before, make sure the residual matches the state
      updateResidual(node) ;

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
    NodeRange::reverse_iterator iPrevNode = rbegin;
    NodeRange::reverse_iterator ithisNode = iPrevNode;
    ++ithisNode;
    updateResidual(dynamic_cast<FitNode&>(**iPrevNode)) ;
    while ( iPrevNode != rend && ithisNode != rend ) {
      FitNode& prevNode = dynamic_cast<FitNode&>(**iPrevNode);
      FitNode& thisNode = dynamic_cast<FitNode&>(**ithisNode);
      // Smoother step
      sc = smooth( thisNode, prevNode ); 
      // Whatever the logic before, make sure the residual matches the state
      updateResidual(thisNode) ;
      if ( sc.isFailure() ) return Warning( "Fit failure: unable to smooth node!", StatusCode::FAILURE, 0 );
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
// Fit the track
//=========================================================================
StatusCode TrackKalmanFilter::fit( Track& track, LHCb::ParticleID ) 
{
  StatusCode sc(StatusCode::SUCCESS, true); 
  
  // The seed covariance comes from the first node
  NodeContainer nodes = track.nodes() ;
  if( nodes.empty() ) return Warning( "Fit failure: track has no nodes", StatusCode::FAILURE,0 );
  
  TrackSymMatrix seedCov = nodes.front()->state().covariance() ;
  
  // First locate the first and last node that actually have information
  NodeContainer::iterator firstMeasurementNode = nodes.begin() ;
  while( firstMeasurementNode!=nodes.end() &&
	 //(*firstMeasurementNode)->type()!=LHCb::Node::Outlier &&
	 (*firstMeasurementNode)->type()!=LHCb::Node::HitOnTrack 
	 ) ++firstMeasurementNode ; 
  
  NodeContainer::iterator lastMeasurementNode = nodes.end() ;
  --lastMeasurementNode ;
  while( lastMeasurementNode!=firstMeasurementNode &&
	 //(*lastMeasurementNode)->type()!=LHCb::Node::Outlier &&
	 (*lastMeasurementNode)->type()!=LHCb::Node::HitOnTrack 
	 ) --lastMeasurementNode ; 
  NodeContainer::iterator endMeasurementNode = lastMeasurementNode ; ++endMeasurementNode;
  
  // Do the fit for these nodes
  NodeRange selectednodes(firstMeasurementNode,endMeasurementNode) ;
  sc = fit(track,selectednodes,seedCov ) ;

  // Now fill up the remaining nodes, just by 'propagating'. 
  if(sc.isSuccess()) {
    // First the nodes at the beginning:
    FitNode* prevNode = dynamic_cast<FitNode*>(*firstMeasurementNode);
    LHCb::State state = prevNode->state() ;
    for( NodeContainer::reverse_iterator inode(firstMeasurementNode) ; 
	 inode != nodes.rend() && sc.isSuccess() ; ++inode) {
      FitNode* node = dynamic_cast<FitNode*>(*inode);
      sc = predictReverseFit( *prevNode, *node, state );
      if(!sc.isSuccess()) warning() << "Unable to predict reverse fit node" << endreq ;
      node->setState(state) ;
      node->setPredictedStateForward( LHCb::State() );
      node->setPredictedStateBackward( LHCb::State() );
      node->setDeltaChi2Forward(0);
      node->setDeltaChi2Backward(0);
      if(node->hasMeasurement()) sc = projectReference(*node) ;
      updateResidual(*node) ;
      prevNode = node ;
    }
    
    // Then the nodes at the end
    state = (*lastMeasurementNode)->state() ;
    for( NodeContainer::iterator inode(endMeasurementNode) ; 
	 inode != nodes.end() && sc.isSuccess() ; ++inode) {
      FitNode* node = dynamic_cast<FitNode*>(*inode);
      sc = predict( *node, state );
      if(!sc.isSuccess()) warning() << "Unable to predict reverse fit node" << endreq ;
      node->setState(state) ;
      node->setPredictedStateForward( LHCb::State() );
      node->setPredictedStateBackward( LHCb::State() );
      node->setDeltaChi2Forward(0);
      node->setDeltaChi2Backward(0);
      if(node->hasMeasurement()) sc = projectReference(*node) ;
      updateResidual(*node) ;
    }
  }     

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
    sc = StatusCode::FAILURE ;
    Warning( "Node without reference", sc, 0 ).ignore();
    debug() << "Node without reference. " << node.measurement().type() << endmsg;
  } else {
    // project the reference state
    Measurement& meas = node.measurement();
    ITrackProjector *proj = m_projectorSelector->projector(meas);
    if ( proj==0 ) {
      sc = StatusCode::FAILURE ;
      Warning( "Could not get projector for measurement", sc, 0 ).ignore();
      debug() << "could not get projector for measurement" << endmsg ;
    } else {
      sc = proj -> project(node.refVector(), meas );
      if ( sc.isFailure() ) {
        Warning( "unable to project statevector", sc, 0 ).ignore();
        debug() << "unable to project this statevector: " << node.refVector() << endmsg ;
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
    Warning( "Z positions of State and Measurement are not equal", StatusCode::FAILURE, 0 ).ignore();
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
	      << " filtered state = " << state << std::endl
	      << " ref residual = " << node.refResidual() 
	      << " type = " 
	      << (node.hasMeasurement() ? node.measurement().type() : 0 )
	      << " err meas = "
	      << (node.hasMeasurement() ? node.measurement().errMeasure() : 0 )
	      << " errMeas2 = " << node.errMeasure2()
	      << " errRes2 = " << errorMeas2 + Similarity(H,C)(0,0) << endmsg ;
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// smooth a node on a track
// Input: node to be smoothed (k) + last node (k+1)
// remember we are now running in the opposite direction to the filter !!
// M. Needham 9/11/99
//----------------------------------------------------------------
StatusCode TrackKalmanFilter::smooth( FitNode& thisNode,
                                      const FitNode& nextNode ) const
{
  // Get the predicted state from the next node
  const TrackVector& nextNodeX    = nextNode.predictedStateForward().stateVector() ;
  const TrackSymMatrix& nextNodeC = nextNode.predictedStateForward().covariance() ;

  // Get the smoothed state of the next node
  const TrackVector& nextNodeSmoothedX = nextNode.state().stateVector();
  const TrackSymMatrix& nextNodeSmoothedC = nextNode.state().covariance();

  // Get the filtered result from this node
  TrackVector& thisNodeX = thisNode.state().stateVector();
  TrackSymMatrix& thisNodeC = thisNode.state().covariance();
  
  // transport
  const TrackMatrix& F = nextNode.transportMatrix();
  
  bool nonZeroNoise = nextNode.noiseMatrix()(2,2)>0 ||nextNode.noiseMatrix()(3,3)>0||nextNode.noiseMatrix()(4,4) > 0 ;
  if(nonZeroNoise) {

    // invert the covariance matrix
    TrackSymMatrix invNextNodeC = nextNodeC;
    if ( !Gaudi::Math::invertPosDefSymMatrix( invNextNodeC ) ) 
      return Warning("Unable to invert matrix in smoother",StatusCode::FAILURE,0) ;
    
    // calculate gain matrix A
    TrackMatrix A = thisNodeC * Transpose( F ) * invNextNodeC;
    thisNode.setSmootherGainMatrix(A) ;
    
    // smooth covariance  matrix
    TrackSymMatrix covUpDate = 
      Similarity<double,TrackMatrix::kRows,TrackMatrix::kCols>
      (A ,  nextNodeSmoothedC - nextNodeC );
    thisNodeC += covUpDate;

    TrackSymMatrix dCov = nextNodeC - nextNodeSmoothedC ;
  } else {
    // if there is no noise, the gain matrix is just the inverse of
    // the transport matrix
    TrackMatrix A = F ;
    A.Invert() ;
    thisNode.setSmootherGainMatrix(A) ;
    // the update of the covariance matrix becomes a lot simpler
    thisNodeC = Similarity( A, nextNodeSmoothedC ) ;
  }

  // smooth the state
  thisNodeX += thisNode.smootherGainMatrix() * (nextNodeSmoothedX - nextNodeX );

  // check that the cov matrix is positive
  if ( !isPositiveMatrix(thisNodeC) ) {
    std::ostringstream mess;
    mess << "Non-positive cov. matrix in smoother for z = "
         << thisNode.z() << std::endl
	 << " thisNodeC = " << thisNodeC << std::endl
	 << " noise = " << nextNode.noiseMatrix() << std::endl ;
    Warning("Problems in smoothing",StatusCode::FAILURE,0).ignore();
    return failure( mess.str() );
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackKalmanFilter::biSmooth( FitNode& thisNode ) const
{
  // Get the predicted state from the reverse fit
  const TrackVector& predRevX = thisNode.predictedStateBackward().stateVector();
  const TrackSymMatrix& predRevC = thisNode.predictedStateBackward().covariance();
  // Get the filtered state from the forward fit
  const TrackVector& filtStateX = thisNode.state().stateVector();
  const TrackSymMatrix& filtStateC = thisNode.state().covariance();
  // Calculate the gain matrix. Start with inverting the cov matrix of the difference.
  TrackSymMatrix R = filtStateC + predRevC ;
  TrackSymMatrix invR = R ;
  if ( !Gaudi::Math::invertPosDefSymMatrix( invR ) ) {
    debug() << "unable to invert matrix in smoother" << invR << endreq ;
    return Warning( "Unable to invert matrix in smoother", StatusCode::FAILURE,0);
  }

  TrackVector    smoothedX ;
  TrackSymMatrix smoothedC ;
  // Now we need to choose wisely which state is the reference. (In
  // a perfect world it would not make a difference.)
  if( filtStateC(0,0) < predRevC(0,0) ) {
    SMatrix<double,5,5> K = filtStateC * invR ;
    smoothedX = filtStateX + K * (predRevX - filtStateX) ;
    // This is the fast expression, but it is not stable enough:
    //
    //     ROOT::Math::AssignSym::Evaluate(smoothedC, K * predRevC ) ; 
    //
    // The inversion of R fails if R has sufficiently large condition
    // number, even if we precondition it (as in invertMatrix). The
    // 'fast' expression for the covariance matrix may then lead to a
    // non-pos-def covariance matrix.  The following works better, but
    // is noticably slower because of the similarity:
    ROOT::Math::AssignSym::Evaluate(smoothedC, -2 * K * filtStateC) ;
    smoothedC += filtStateC + ROOT::Math::Similarity(K,R) ;
  } else {
    SMatrix<double,5,5> K = predRevC * invR ;
    smoothedX    = predRevX + K * (filtStateX - predRevX) ;
    ROOT::Math::AssignSym::Evaluate(smoothedC, -2 * K * predRevC) ;
    smoothedC += predRevC + ROOT::Math::Similarity(K,R) ;
  }

  // this is the simplest check to see that things went really wrong
  if(thisNode.hasMeasurement()) {
    const TrackProjectionMatrix& H  = thisNode.projectionMatrix();
    double V = thisNode.errMeasure2();
    double HCH = Similarity( H, smoothedC )(0,0);
    if( HCH > V ) {
      debug() << "Severe problem after smoother: smoothed error larger than measurement error"
	      << V << " " << HCH << " " <<  endreq ;
      return Warning( "Smoothing error", StatusCode::FAILURE, 0 );
    }
  }
  
  (thisNode.state()).setState( smoothedX );
  (thisNode.state()).setCovariance( smoothedC );

  return StatusCode::SUCCESS;
}

void TrackKalmanFilter::updateResidual(FitNode& node) const
{
  if ( node.hasMeasurement() ) {
    // We should put this inside the Node.
    const TrackProjectionMatrix& H  = node.projectionMatrix();
    const TrackVector& refX         = node.refVector().parameters() ;
    const TrackVector& smoothedX    = node.state().stateVector() ;
    const TrackSymMatrix& smoothedC = node.state().covariance() ;
    node.setResidual( node.refResidual() + (H*(refX - smoothedX))(0)) ;
    double V = node.errMeasure2();
    double HCH = Similarity( H, smoothedC )(0,0);
    double sign = node.type()==LHCb::Node::HitOnTrack? -1 : 1 ;
    double R = V + sign * HCH;
    if ( !(R>0) ) {
      Warning( "Non-positive variance for residual", StatusCode::SUCCESS, 0 ).ignore();
      node.setErrResidual(0);
    } else {
      node.setErrResidual( std::sqrt( R ) ) ;
    }
  } else {
    node.setResidual(0) ;
    node.setErrResidual(0) ;
  }
}

//=========================================================================

