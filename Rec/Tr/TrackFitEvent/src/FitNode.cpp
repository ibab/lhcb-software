// FitNode.cpp,v 1.25 2009-09-25 09:04:35 wouter Exp $
// Include files

// local
#include "Event/FitNode.h"
#include "Event/KalmanFitResult.h"
#include "LHCbMath/Similarity.h"

using namespace Gaudi;
using namespace Gaudi::Math;
using namespace LHCb;
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
    
  void transportcovariance( const Gaudi::TrackMatrix& F,
                            const Gaudi::TrackSymMatrix& origin,
                            Gaudi::TrackSymMatrix& target)
  {
    bool isLine =  F(0,4)==0 ;
    if ( LIKELY( !isLine ) ) {

      // The temporary is actually important here. SMatrix is not
      // computing A*B*C very optimally.
      // static ROOT::Math::SMatrix<double, 5,5> FC ;
      // FC = F*origin ;
      // ROOT::Math::AssignSym::Evaluate(target,FC*Transpose(F)) ;

      // use vectorized similarity transform
      LHCb::Math::Similarity(F,origin,target);

    } else {

      target = origin;
      target(0,0) += 2 * origin(2,0) * F(0,2) + origin(2,2) * F(0,2) * F(0,2);
      target(2,0) += origin(2,2) * F(0,2) ;
      target(1,1) += 2 * origin(3,1) * F(1,3) + origin(3,3) * F(1,3) * F(1,3);
      target(3,1) += origin(3,3) * F(1,3);
      target(1,0) += origin(2,1) * F(0,2) + origin(3,0) * F(1,3) + origin(3,2) * F(0,2) * F(1,3);
      target(2,1) += origin(3,2) * F(1,3);
      target(3,0) += origin(3,2) * F(0,2);

    }
  }

  void inverttransport( const Gaudi::TrackMatrix& F,
			Gaudi::TrackMatrix& Finv )
  {
    // it would save time if qw assume that Finv is either diagonal or filled with 0?
    Finv = F ;
    bool isLine =  F(0,4)==0 ;
    if( isLine ) {
      Finv(0,2) = -F(0,2) ;
      Finv(1,3) = -F(1,3) ;
    } else {
      //Finv(0,0) = Finv(1,1) = Finv(4,4) = 1 ;
      // write
      //      ( 1  0 |  S00 S01 | U0 )
      //      ( 0  1 |  S10 S01 | U1 )
      // F =  ( 0  0 |  T00 T01 | V0 )
      //      ( 0  0 |  T10 T11 | V1 )
      //      ( 0  0 |   0   0  | 1  )
      // then we have
      // Tinv = T^{-1}
      double det = F(2,2)*F(3,3)-F(2,3)*F(3,2) ;
      Finv(2,2) = F(3,3)/det ;
      Finv(3,3) = F(2,2)/det ;
      Finv(2,3) = -F(2,3)/det ;
      Finv(3,2) = -F(3,2)/det ;
      // Vinv = - T^-1 * V
      Finv(2,4) = -Finv(2,2)*F(2,4) -Finv(2,3)*F(3,4) ; 
      Finv(3,4) = -Finv(3,2)*F(2,4) -Finv(3,3)*F(3,4) ; 
      // Uinv = S * T^-1 * V - U = - S * Vinv - U
      Finv(0,4) = -F(0,4) - F(0,2)*Finv(2,4) - F(0,3)*Finv(3,4) ;
      Finv(1,4) = -F(1,4) - F(1,2)*Finv(2,4) - F(1, 3)*Finv(3,4) ;
      // Sinv  = - S * T^{-1}
      Finv(0,2) = - F(0,2)*Finv(2,2) - F(0,3)*Finv(3,2) ;
      Finv(0,3) = - F(0,2)*Finv(2,3) - F(0,3)*Finv(3,3) ;
      Finv(1,2) = - F(1,2)*Finv(2,2) - F(1,3)*Finv(3,2) ;
      Finv(1,3) = - F(1,2)*Finv(2,3) - F(1,3)*Finv(3,3) ;
    }
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
  }
  
  
  
  /// Destructor
  FitNode::~FitNode()
  {
    if( m_prevNode && m_prevNode->m_nextNode==this) m_prevNode->m_nextNode=0 ;
    if( m_nextNode && m_nextNode->m_prevNode==this) m_nextNode->m_prevNode=0 ;
  }
  
  // Clone the node
  LHCb::Node* FitNode::clone() const
  {
    LHCb::FitNode* rc = new LHCb::FitNode(*this) ;
    rc->m_prevNode = rc->m_nextNode = 0 ;
    return rc ;
  }
  
  /// retrieve the state, overloading the inline function in Node
  const LHCb::State& FitNode::state() const
  { 
    if(m_parent->biDirectionnalSmoother()) return biSmoothedState() ;
    else return classicalSmoothedState();
  }

  /// retrieve the residual, overloading the function in Node
  double FitNode::residual() const
  { 
    if(m_parent->biDirectionnalSmoother()) biSmoothedState() ;
    else classicalSmoothedState();
    return m_residual ; 
  }

  /// retrieve the residual, overloading the function in Node
  double FitNode::errResidual() const
  { 
    if(m_parent->biDirectionnalSmoother()) biSmoothedState() ;
    else classicalSmoothedState();
    return m_errResidual ; 
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
    static const TrackSymMatrix unit = ROOT::Math::SMatrixIdentity();
    TrackSymMatrix unbiasedC ;
    ROOT::Math::AssignSym::Evaluate(unbiasedC, (unit + K*H)*biasedC) ;
    return State( unbiasedX, unbiasedC, z(), state().location()) ;
  }
  
  void FitNode::setTransportMatrix( const Gaudi::TrackMatrix& transportMatrix )  {
    m_transportMatrix = transportMatrix;
    // invert the transport matrix. We could save some time by doing this on demand only.
    inverttransport( m_transportMatrix, m_invertTransportMatrix ) ;
    // we still need to think about the logic for setting the initial state
    resetFilterStatus() ;
  }
  
  //=========================================================================
  // Helper function to decide if we need to use the upstream filtered state
  //=========================================================================
  bool FitNode::hasInfoUpstream(int direction) const
  {
    if( m_hasInfoUpstream[direction] == LHCb::FitNode::Unknown ) {
      bool rc=false ;
      const FitNode* prev = prevNode( direction ) ;
      if( prev ) {
	if( prev->type()==HitOnTrack ) rc = true ;
	else rc = prev->hasInfoUpstream(direction) ;
      }
      unConst().m_hasInfoUpstream[direction] = rc ? LHCb::FitNode::True : LHCb::FitNode::False ;
    }
    return (m_hasInfoUpstream[direction] == LHCb::FitNode::True) ;
  } 
  
  void FitNode::resetHasInfoUpstream(int direction)
  {
    m_hasInfoUpstream[direction] = False ;
    if( this->type()!=HitOnTrack ) {
      FitNode* next = const_cast<FitNode*>(nextNode(direction)) ;
      if(next) next->resetHasInfoUpstream(direction) ;
    }
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
      // make sure the KalmanFitResult knows something has changed
      if(m_parent) m_parent->resetCache() ;
      // now make sure others do not rely on this one anymore
      if( !hasInfoUpstream(Forward) ) resetHasInfoUpstream(Forward) ; 
      if( !hasInfoUpstream(Backward) ) resetHasInfoUpstream(Backward) ;
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
	if( m_filterStatus[ Backward ] == Smoothed ) // Note: Backward=Smoothed means 'bi-directional smoother'
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
	   prev->nextNode(Forward)==this ) // Note: Forward=Smoothed means 'classical smoother'
	  const_cast<FitNode*>(prev)->resetFilterStatus(Forward,Filtered ) ;
      }
    }
  }
  
  //=========================================================================
  // Predict the state to this node
  //=========================================================================
  void FitNode::computePredictedState(int direction)
  {
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
          transportcovariance(F,previousState.covariance(),stateCov) ;
          stateCov += this->noiseMatrix();
        } else {
          const TrackMatrix& invF = prevnode->invertTransportMatrix();
          TrackSymMatrix tempCov ;
          tempCov = previousState.covariance() + prevnode->noiseMatrix();
          stateVec = invF * ( previousState.stateVector() - prevnode->transportVector()) ;
          transportcovariance(invF,tempCov,stateCov) ;
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
    
    const LHCb::FitNode* pn = prevNode(direction) ;
    m_totalChi2[direction] = pn ? pn->totalChi2(direction) : LHCb::ChiSquare(0,-m_parent->nTrackParameters()) ;
    m_deltaChi2[direction] = LHCb::ChiSquare() ;
    
    // apply the filter if needed
    if( type() == HitOnTrack ) {

      const TrackProjectionMatrix& H = this->projectionMatrix();
      if( !( std::abs(H(0,0)) + std::abs(H(0,1))>0) ) {
        KalmanFitResult* kfr = this->getParent();
        if (!kfr->inError())
          kfr->setErrorFlag(direction,KalmanFitResult::Filter ,KalmanFitResult::Initialization ) ;
      }
      double chi2 = LHCb::Math::Filter( state.stateVector(), state.covariance(),
                                        this->refVector().parameters(),
                                        this->projectionMatrix(),
                                        this->refResidual(), 
                                        this->errMeasure2() );

      // set the chisquare contribution
      m_deltaChi2[direction] = LHCb::ChiSquare(chi2,1);
    }
    
    m_totalChi2[direction] += m_deltaChi2[direction]  ;
    m_filterStatus[direction] = Filtered ;
    
    if ( !(m_filteredState[direction].covariance()(0,0)>0 && 
	   m_filteredState[direction].covariance()(1,1)>0)   ) {
      KalmanFitResult* kfr = this->getParent();
      if (!kfr->inError())
        kfr->setErrorFlag(direction,KalmanFitResult::Predict ,KalmanFitResult::AlgError ) ;
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
      if( predictedState(Backward).covariance()(0,0) > predictedState(Forward).covariance()(0,0) ) {
        s1 = &( filteredState(Backward) ) ;
        s2 = &( predictedState(Forward) ) ;
      } else {
        s1 = &( filteredState(Forward) ) ;
        s2 = &( predictedState(Backward) ) ;
      }	
      state.setZ( z() ) ; // the disadvantage of having this information more than once

      bool success = LHCb::Math::Average( s1->stateVector(), s1->covariance(),
                                          s2->stateVector(), s2->covariance(),
                                          state.stateVector(), state.covariance() );
      if (!success && !m_parent->inError())
        m_parent->setErrorFlag(2,KalmanFitResult::Smooth ,KalmanFitResult::MatrixInversion ) ;
    }
    if(!isPositiveDiagonal(state.covariance())&& !m_parent->inError()){
      m_parent->setErrorFlag(2,KalmanFitResult::Smooth ,KalmanFitResult::AlgError ) ;
    }
    updateResidual(state) ;
    // bug fix: we cannot set backward to state 'Smoothed', unless we have passed its filter step!
    filteredState(Backward) ;
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
        TrackSymMatrix covUpDate = Similarity(A ,  nextNodeSmoothedC - nextNodeC );
        thisNodeC += covUpDate;
#else
        // The expression above is unstable since you effectively subtract
        // matrices. I found an expression in which you only add things
        // up. The expression is:
        //   C = A ( SmoothedCnext + Q + Q  Finv^T Cfiltered Finv Q ) A^T  
        // It is of course much slower ... but we don't care about that now.
        TrackSymMatrix FCFinv = LHCb::Math::Similarity(F,thisNodeC) ; // is also nextNodeC - Q
        FCFinv.InvertChol();
        TrackSymMatrix sum = nextNodeSmoothedC + Q + LHCb::Math::Similarity(Q,FCFinv);
        LHCb::Math::Similarity(A, sum, thisNodeC);
#endif
      } else {
        // if there is no noise, the gain matrix is just the inverse of
        // the transport matrix
        A = F ;
        A.Invert() ;
        // the update of the covariance matrix becomes a lot simpler
        LHCb::Math::Similarity( A, nextNodeSmoothedC, thisNodeC ) ;
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
      double HCH = LHCb::Math::Similarity( H, state.covariance() );
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

  /*
  double FitNode::chi2LowerLimit() const
  {
    // get a lower limit by smoothing residuals in 1D.
    double rc=0 ;
    if( hasMeasurement() ) {
      const TrackProjectionMatrix& H  = this->projectionMatrix();
      const TrackVector& refX         = this->refVector().parameters() ;
      //  average the predicted residuals. make sure to take
      //  correlation in measurement into account, namely by averaging
      //  before adding that error.
      const LHCb::State& stateA = predictedState(Forward) ;
      const LHCb::State& stateB = predictedState(Backward) ;
      // actually, this just averages the predicted measurement
      double rA   = refResidual() + (H*(refX - stateA.stateVector()))(0) ;
      double HCHA =  Similarity( H, stateA.covariance() )(0,0);
      double rB   = refResidual() + (H*(refX - stateB.stateVector()))(0) ;
      double HCHB =  Similarity( H, stateB.covariance() )(0,0);
      double HCH = 1/(1/HCHA + 1/HCHB) ;
      double r   = HCH*(rA/HCHA + rB/HCHB) ;
      double V   = errMeasure2();
      rc = r*r/(HCH+V) ;
    }
    return rc ;
  }
  */  

  int FitNode::index() const
  {
    int rc = 0 ;
    if( m_prevNode ) rc = m_prevNode->index() +1 ;
    return rc ;
  }
}
