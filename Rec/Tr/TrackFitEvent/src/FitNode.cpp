// FitNode.cpp,v 1.25 2009-09-25 09:04:35 wouter Exp $
// Include files

// local
#include "Event/FitNode.h"
#include "TriangularInverter.h"

using namespace Gaudi;
using namespace Gaudi::Math;
using namespace LHCb;
using namespace TriangularInversion;
/** @file FitNode.cpp
 *
 *  This File contains the implementation of the FitNode.
 *  A FitNode is a basket of objects at a given z position.
 *  The information inside the FitNode has to be sufficient 
 *  to allow smoothing and refitting.
 *  At the moment a FitNode contains or allows you to access
 *  info on the the (kth) measurement, 
 *  transport from k --> k + 1 , predicted state at k+1  
 *  (predicted from filter step)  and the best state at k  
 *  (notation note filter procedes from k -> k + 1 -> k + 2 ......)
 *  
 *  @author Victor Coco and Wouter Hulsbergen (moved K-math here)
 *  @date   2011-02-01
 *
 *  @author Eduardo Rodrigues (adaptations to the new track event model)
 *  @date   2005-04-15
 *
 *  @author Matt Needham
 *  @date   11-11-1999
 */

namespace {
  template<class Vector, class SymMatrix>
  bool weightedAverage( const Vector& X1, const SymMatrix& C1,
			const Vector& X2, const SymMatrix& C2,
			Vector& X, SymMatrix& C )
  {
    static SymMatrix invR;
    static ROOT::Math::SMatrix<double, Vector::kSize, Vector::kSize> K ;
    invR = C1 + C2 ;
    bool success = invR.InvertChol() ;
    K = C1 * invR ;
    X = X1 + K*(X2 - X1) ;
    ROOT::Math::AssignSym::Evaluate(C, K*C2 ) ; 
    return success ;
  }
    
  template<class Matrix, class SymMatrix>
  void similarity( const Matrix& F,
		   const SymMatrix& origin,
		   SymMatrix& target)
  {
    // The temporary is actually important here. SMatrix is not
    // computing A*B*C very optimally.
    static ROOT::Math::SMatrix<double, 5,5> FC ;
    FC = F*origin ;
    ROOT::Math::AssignSym::Evaluate(target,FC*Transpose(F)) ;
  }
}

namespace LHCb {

  /// Standard constructor, initializes variables
  FitNode::FitNode():
    Node(),
    m_deltaEnergy(0),
    m_refResidual(0),
    m_doca(0),
    m_prevNode(0),
    m_nextNode(0),
    m_parent(0)
  {
    // FitNode default constructor
    m_filterStatus[Forward] = m_filterStatus[Backward] = Uninitialized ;
    m_hasInfoUpstream[Forward] = m_hasInfoUpstream[Backward] = Unknown ;
    m_deltaChi2[Forward] = m_deltaChi2[Backward] = 0 ;
  }
  
  /// Constructor from a z position
  FitNode::FitNode( double zPos, LHCb::State::Location location ):
    Node(zPos,location),
    m_deltaEnergy(0),
    m_refResidual(0),
    m_doca(0),
    m_prevNode(0),
    m_nextNode(0),
    m_parent(0)
  {
    m_filterStatus[Forward] = m_filterStatus[Backward] = Uninitialized ;
    m_hasInfoUpstream[Forward] = m_hasInfoUpstream[Backward] = Unknown ;
    m_deltaChi2[Forward] = m_deltaChi2[Backward] = 0 ;
    m_predictedState[Forward].setLocation(location) ;
    m_predictedState[Backward].setLocation(location) ;
  }
  
  /// Constructor from a Measurement
  FitNode::FitNode(Measurement& aMeas):
    Node(&aMeas),
    m_deltaEnergy(0),
    m_refResidual(0),
    m_doca(0),
    m_prevNode(0),
    m_nextNode(0),
    m_parent(0)
  {
    m_filterStatus[Forward] = m_filterStatus[Backward] = Uninitialized ;
    m_hasInfoUpstream[Forward] = m_hasInfoUpstream[Backward] = Unknown ;
    m_deltaChi2[Forward] = m_deltaChi2[Backward] = 0 ;
  }
  
  
  
  /// Destructor
  FitNode::~FitNode()
  {
  }
  
  // Clone the node
  LHCb::Node* FitNode::clone() const
  {
    LHCb::FitNode* rc = new LHCb::FitNode(*this) ;
    rc->m_prevNode = rc->m_nextNode = 0 ;
    return rc ;
  }
  
  
  /// Calculate an unbiased state
  LHCb::State FitNode::unbiasedState() const
  {
    // we can redo this routine by smoothing the unfiltered states
    
    // This performs an inverse kalman filter step.
    // First calculate the gain matrix
    const TrackProjectionMatrix& H = projectionMatrix();
    double r = residual();
    double R = errResidual2() ;
    const TrackSymMatrix& biasedC = state().covariance() ;
    ROOT::Math::SMatrix<double,5,1> K = (biasedC * Transpose(H)) / R;
    // Forwarddate the state vectors
    const TrackVector&  biasedX = state().stateVector();
    TrackVector unbiasedX=biasedX - K.Col(0) * r;
    // Forwarddate the covariance matrix
    static const TrackSymMatrix unit = TrackSymMatrix( ROOT::Math::SMatrixIdentity());
    TrackSymMatrix unbiasedC ;
    ROOT::Math::AssignSym::Evaluate(unbiasedC, (unit + K*H)*biasedC) ;
    return State( unbiasedX, unbiasedC, z(), state().location()) ;
  }
  
  void FitNode::setTransportMatrix( const Gaudi::TrackMatrix& transportMatrix )  {
    m_transportMatrix = transportMatrix;
    // compute and cache the inverse transport matrix
    invertAlmostUpperTriangular( transportMatrix ,  m_invertTransportMatrix , 3 );
    // we still need to think about the logic for setting the initial state
    resetFilterStatus() ;
  }
  
  //=========================================================================
  // Helper function to decide if we need to use the upstream filtered state
  //=========================================================================
  bool FitNode::hasInfoUpstream(int direction) const
  {
    if( m_hasInfoUpstream[direction] == Unknown ) {
      bool rc=false ;
      const FitNode* prev = prevNode( direction ) ;
      if( prev ) {
	if( prev->type()==HitOnTrack ) rc = true ;
	else rc = prev->hasInfoUpstream(direction) ;
      }
      unConst().m_hasInfoUpstream[direction] = rc ? True : False ;
    }
    return m_hasInfoUpstream[direction] == True ;
  } 
  
  //=========================================================================
  // Turn this node into an outlier
  //=========================================================================
  void FitNode::deactivateMeasurement(bool deactivate)
  {
    // only do something if this is actually an active hit
    if( (deactivate && type() == HitOnTrack) || (!deactivate && type()==Outlier) ) {
      // set type to outlier
      setType( deactivate ? LHCb::Node::Outlier : LHCb::Node::HitOnTrack )  ;
      // this will take care of upstream and downstream nodes as well:
      // they will be reset to initialized. we need to check this
      // carefully
      resetFilterStatus( Predicted ) ;
    }
  }

  //=========================================================================
  // Reset the status of this node
  //=========================================================================
  void FitNode::resetFilterStatus( int direction, FilterStatus s) 
  {
    // The logic here is tedious, because of the smoothed states have
    // a strange depence, which depends on the type of smoother.
    if ( m_filterStatus[direction] > s ) {
      m_filterStatus[direction] = s ;
      
      if ( s < Filtered ) {
	// if the backward filter is in 'Smoothed' state, it needs to be
	// reset to filtered, because the bi-directional smoother relies
	// on both filtered states
	if( m_filterStatus[ Backward ] == Smoothed )
	  m_filterStatus[ Backward ] = Filtered ;
	
	// reset the status of any node that depends on this one. now
	// be careful: if this node has been copied it may be pointing
	// to a wrong node.
	const FitNode* next = nextNode(direction) ;
	if(next && next->m_filterStatus[direction] > s && 
	   next->prevNode(direction)==this ) 
	  const_cast<FitNode*>(next)->resetFilterStatus(direction, std::min(s,Initialized) ) ;
      }

      if( direction == Forward ) {
	// for the classical filter, we actually need to put the
	// upstream node back to filtered, if it is in a classicly
	// smoothed status
	const FitNode* prev = prevNode(Forward) ;
	if(prev && prev->m_filterStatus[Forward] == Smoothed &&
	   prev->nextNode(Forward)==this )
	  const_cast<FitNode*>(prev)->resetFilterStatus(Forward,Filtered ) ;
      }
    }
  }
  
  //=========================================================================
  // Predict the state to this node
  //=========================================================================
  void FitNode::computePredictedState(int direction)
  {
    //std::cout << "Predicting node " << z() << " " << index() << " " << direction << std::endl ;
    
    // get the filtered state from the previous node. if there wasn't
    // any, we will want to copy the reference vector and leave the
    // covariance the way it is
    const FitNode* prevnode = prevNode(direction) ;
    m_predictedState[direction].setZ( z() ) ;
    TrackVector&    stateVec = m_predictedState[direction].stateVector();
    TrackSymMatrix& stateCov = m_predictedState[direction].covariance();
    // start by copying the refvector. that's always the starting point
    stateVec = refVector().parameters() ;
    if(prevnode) {
      const LHCb::State& previousState = prevnode->filteredState(direction) ;
      if( !hasInfoUpstream(direction) ) {
        // just _copy_ the covariance matrix from upstream, assuming
        // that this is the seed matrix. (that saves us from copying
        // the seed matrix to every state from the start.
        stateCov = previousState.covariance() ;
        // new: start the backward filter from the forward filter 
        if( direction==Backward ) 
          stateVec = filteredState(Forward).stateVector();
      	//std::cout << "no information upstream. copying seed." << index() << std::endl ;
      } else {
        if(direction==Forward) {
          const TrackMatrix& F = transportMatrix() ;
          stateVec = F * previousState.stateVector() + transportVector() ;
          //similarityAlmostUpperTriangular( F, previousState.covariance() , stateCov , 3 );
          //stateCov = Similarity( F,previousState.covariance() ) ;
          similarity(F,previousState.covariance() , stateCov) ;
          stateCov += this->noiseMatrix();
        } else {
          const TrackMatrix& invF = prevnode->invertTransportMatrix();
          static TrackSymMatrix tempCov ;
          tempCov = previousState.covariance() + prevnode->noiseMatrix();
          stateVec = invF * ( previousState.stateVector() - prevnode->transportVector()) ;
          //stateCov = Similarity( invF, tempCov );
          //similarityAlmostUpperTriangular( invF, tempCov , stateCov , 3 );
          similarity(invF,tempCov,stateCov) ;
        }
      }
    } else {
      if( !( stateCov(0,0)>0 ) ) {
        KalmanFitResult* kfr = this->getParent();
        if (!kfr->inError())
          kfr->setErrorFlag(direction,KalmanFitResult::Predict ,KalmanFitResult::Initialization ) ;
      }
    }
    // update the status flag
    m_filterStatus[ direction ] = Predicted ;
 
    if ( !(m_predictedState[direction].covariance()(0,0)>0) ) {
      KalmanFitResult* kfr = this->getParent();
      if (!kfr->inError())
        kfr->setErrorFlag(direction,KalmanFitResult::Predict ,KalmanFitResult::AlgError ) ;
    }
  } 
  

  //=========================================================================
  // Filter this node
  //=========================================================================
  void FitNode::computeFilteredState( int direction )
  {
    //std::cout << "Filtering node " << z() << " " << index() << " " << direction << std::endl ;
    // get the predicted state
    LHCb::State& state = m_filteredState[direction] ;
    // copy the predicted state
    state = predictedState(direction) ;
    
    m_deltaChi2[direction] = 0 ;
    
    // apply the filter if needed
    if( type() == HitOnTrack ) {
      //std::cout << "Filtering a hit on track." << std::endl ;
      // get reference to the state vector and cov
      TrackVector&    X = state.stateVector();
      TrackSymMatrix& C = state.covariance();
      
      // calculate the linearized residual of the prediction and its error
      const TrackProjectionMatrix& H = this->projectionMatrix();
      const double errorMeas2 = this->errMeasure2();
      double res        = this->refResidual() + ( H * (this->refVector().parameters() - X) ) (0) ;
      double errorRes2  = errorMeas2 + Similarity(H,C)(0,0) ;  
      if( !( std::abs(H(0,0)) + std::abs(H(0,1))>0) ) {
        KalmanFitResult* kfr = this->getParent();
        if (!kfr->inError())
          kfr->setErrorFlag(direction,KalmanFitResult::Filter ,KalmanFitResult::Initialization ) ;
        //std::cout << "Error: projection is not set! " << index() << " " << H << std::endl ;
      }
      // calculate gain matrix K
      static SMatrix<double,5,1> CHT, K ;
      CHT = C * Transpose(H) ;
      K = CHT / errorRes2;
      
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
      static TrackSymMatrix tmp ;
      ROOT::Math::AssignSym::Evaluate(tmp, -2 * K * Transpose(CHT) ) ;
      C += tmp + Similarity(K,R) ;
      
      // set the chisquare contribution
      m_deltaChi2[direction] = res*res / errorRes2 ;
    }
    m_filterStatus[direction] = Filtered ;
    
    if ( !(m_filteredState[direction].covariance()(0,0)>0)   ) {
      KalmanFitResult* kfr = this->getParent();
      if (!kfr->inError())
        kfr->setErrorFlag(direction,KalmanFitResult::Predict ,KalmanFitResult::AlgError ) ;
      //       std::cout << "ERRRORRR: something goes wrong in the filter."
      // 		<< m_filteredState[direction].covariance() << std::endl 
      // 		<< predictedState(direction) << std::endl ;
    }
  }
  

    
  
  //=========================================================================
  // Bi-directional smoother
  //=========================================================================
  void FitNode::computeBiSmoothedState() 
  {
    LHCb::State& state = m_state ;
    if( ! hasInfoUpstream(Forward) ) {
      // last node in backward direction
      state = filteredState( Backward ) ;
    } else if( ! hasInfoUpstream(Backward) ) {
      // last node in forward direction
      state = filteredState( Forward ) ;
    } else {
      // Take the weighted average of two states. We now need to
      // choose for which one we take the filtered state. AFAIU the
      // weighted average behaves better if the weights are more
      // equal. So, we filter the 'worst' prediction. In the end, it
      // all doesn't seem to make much difference.
      
      const LHCb::State *s1, *s2 ;
      if( predictedState(Backward).covariance()(0,0) >
	  predictedState(Forward).covariance()(0,0) ) {
	s1 = &( filteredState(Backward) ) ;
	s2 = &( predictedState(Forward) ) ;
      } else {
	s1 = &( filteredState(Forward) ) ;
	s2 = &( predictedState(Backward) ) ;
      }	
      
      const TrackVector&    X1 = s1->stateVector();
      const TrackSymMatrix& C1 = s1->covariance();
      const TrackVector&    X2  = s2->stateVector();
      const TrackSymMatrix& C2  = s2->covariance();
      
      state = filteredState(Forward) ;
      TrackVector&    X = state.stateVector() ; 
      TrackSymMatrix& C = state.covariance() ;
      
      // compute the inverse of the covariance in the difference: R=(C1+C2)
      static TrackSymMatrix invR;
      invR = C1 + C2 ;
      bool success = invR.InvertChol() ;
      if (!success && !m_parent->inError())
        m_parent->setErrorFlag(2,KalmanFitResult::Smooth ,KalmanFitResult::MatrixInversion ) ;
      // compute the gain matrix:
      static ROOT::Math::SMatrix<double,5,5> K ;
      K = C1 * invR ;
      X = X1 + K*(X2 - X1) ;
      ROOT::Math::AssignSym::Evaluate(C, K*C2 ) ; 
      // the following used to be more stable, but isn't any longer, it seems:
      //ROOT::Math::AssignSym::Evaluate(C, -2 * K * C1) ;
      //C += C1 + ROOT::Math::Similarity(K,R) ;
      
    }
    if(!isPositiveDiagonal(state.covariance())&& !m_parent->inError()){
      m_parent->setErrorFlag(2,KalmanFitResult::Smooth ,KalmanFitResult::AlgError ) ;
    }
    updateResidual(state) ;
    m_filterStatus[Backward] = Smoothed ;

    
  }
  
  //=========================================================================
  // Classical smoother
  //=========================================================================
  void FitNode::computeClassicalSmoothedState() 
  {
    // note this relies _only_ on the forward filter
    LHCb::State& state = m_classicalSmoothedState ;
  
    if( ! hasInfoUpstream(Backward) ) {
      // last node in forward direction
      state = filteredState( Forward ) ;
    } else {
      // Get the predicted state from the next node
      const FitNode* nextnode = nextNode(Forward) ;
      if(nextnode == NULL){
        KalmanFitResult* kfr = this->getParent();
        if (!kfr->inError())
          kfr->setErrorFlag(KalmanFitResult::BiDirection,KalmanFitResult::Smooth ,KalmanFitResult::Other ) ;
        return ;
      }
      
      const LHCb::State& nextPredictedState = nextnode->predictedState(Forward) ;
      const TrackVector& nextNodeX    = nextPredictedState.stateVector() ;
      const TrackSymMatrix& nextNodeC = nextPredictedState.covariance() ;
      
      // Get the smoothed state of the next node
      const LHCb::State& nextSmoothedState = nextnode->classicalSmoothedState() ;
      const TrackVector& nextNodeSmoothedX    = nextSmoothedState.stateVector();
      const TrackSymMatrix& nextNodeSmoothedC = nextSmoothedState.covariance();
      
      // Get the filtered result from this node
      const LHCb::State& filteredstate = filteredState(Forward) ;
      
      // copy the information
      state = filteredstate ;
      
      // define some shortcuts, for convenience
      TrackVector&    thisNodeX = state.stateVector();
      TrackSymMatrix& thisNodeC = state.covariance();
      const TrackMatrix& F = nextnode->transportMatrix();
      TrackMatrix& A = m_smootherGainMatrix ;
      
      const TrackSymMatrix& Q = nextnode->noiseMatrix() ;
      bool nonZeroNoise = (Q(2,2)+Q(3,3)+Q(4,4))>0 ;
      if(nonZeroNoise) {
	
	// invert the covariance matrix
	TrackSymMatrix invNextNodeC = nextNodeC;
	if( !invNextNodeC.InvertChol() ) {
	  KalmanFitResult* kfr = this->getParent();
	  if (!kfr->inError())
	    kfr->setErrorFlag(KalmanFitResult::BiDirection,KalmanFitResult::Smooth ,KalmanFitResult::MatrixInversion ) ;
	}
	
	// calculate gain matrix A. we can make this quicker by epxloiting that F is empty
	A = thisNodeC * Transpose( F ) * invNextNodeC;
	
	// smooth covariance  matrix
#ifdef COMMONEXPRESSION
	TrackSymMatrix covUpDate = 
	  Similarity<double,TrackMatrix::kRows,TrackMatrix::kCols>
	  (A ,  nextNodeSmoothedC - nextNodeC );
	thisNodeC += covUpDate;
#else
	// The expression above is unstable since you effectively subtract
	// matrices. I found an expression in which you only add things
	// up. The expression is:
	//   C = A ( SmoothedCnext + Q + Q  Finv^T Cfiltered Finv Q ) A^T  
	// It is of course much slower ... but we don't care about that now.
	TrackSymMatrix FCFinv = Similarity(F,thisNodeC) ; // is also nextNodeC - Q
	FCFinv.InvertChol() ;
	TrackSymMatrix QFCFQ = Similarity(Q,FCFinv) ;
	TrackSymMatrix sum = nextNodeSmoothedC + Q + QFCFQ ;
	thisNodeC = Similarity(A, sum ) ;
#endif
      } else {
	// if there is no noise, the gain matrix is just the inverse of
	// the transport matrix
	A = F ;
	A.Invert() ;
	// the update of the covariance matrix becomes a lot simpler
	thisNodeC = Similarity( A, nextNodeSmoothedC ) ;
      }
      
      // smooth the state
      thisNodeX += A * (nextNodeSmoothedX - nextNodeX );
    }
    // now we fill the residual
    updateResidual(state) ;

    if( !isPositiveDiagonal(state.covariance())&& !m_parent->inError() ) {
      KalmanFitResult* kfr = this->getParent();
      if (!kfr->inError())
        kfr->setErrorFlag(KalmanFitResult::BiDirection,KalmanFitResult::Smooth ,KalmanFitResult::AlgError ) ;
    }

    // finally update the _Forward_ state
    m_filterStatus[Forward] = Smoothed ;
  }
  
  Gaudi::Math::ValueWithError FitNode::computeResidual( const LHCb::State& state, bool biased ) const
  {
    double r(0),R(0) ;
    if ( this->hasMeasurement() ) {
      // We should put this inside the This->
      const TrackProjectionMatrix& H  = this->projectionMatrix();
      const TrackVector& refX         = this->refVector().parameters() ;
      r = refResidual() + (H*(refX - state.stateVector()))(0) ;
      
      double V = errMeasure2();
      double HCH = Similarity( H, state.covariance() )(0,0);
      double sign = biased ? -1 : 1 ;
      R = V + sign * HCH;
    }
    return Gaudi::Math::ValueWithError(r,R) ;
  }
  
  
  void FitNode::updateResidual( const LHCb::State& smoothedState )
  {
    Gaudi::Math::ValueWithError res ;
    if ( this->hasMeasurement() ) {
      res = computeResidual( smoothedState, this->type()==LHCb::Node::HitOnTrack ) ;
      if( !(res.covariance() > 0) && !m_parent->inError())
        m_parent->setErrorFlag(2,KalmanFitResult::ComputeResidual ,KalmanFitResult::Other ) ;
    }
    setResidual(res.value()) ;
    setErrResidual(res.error()) ;
  }
  
  inline bool FitNode::isPositiveDiagonal( const Gaudi::TrackSymMatrix& mat ) const 
  {
    unsigned int i = 0u;
    for ( ; i < Gaudi::TrackSymMatrix::kRows && mat(i,i) > 0.0 ; ++i ) {}
    return i == Gaudi::TrackSymMatrix::kRows ;
  }
  
  
  int FitNode::index() const
  {
    int rc = 0 ;
    if( m_prevNode ) rc = m_prevNode->index() +1 ;
    return rc ;
  }
}
