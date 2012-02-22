#ifndef TRACKFITEVENT_FITNODE_H
#define TRACKFITEVENT_FITNODE_H 1

// from TrackEvent
#include "Event/Node.h"
#include "Event/Measurement.h"
#include "Event/ChiSquare.h"
#include "LHCbMath/ValueWithError.h"

#include "GaudiKernel/boost_allocator.h"
// From LHCbMath
#include "LHCbMath/MatrixManip.h"

namespace LHCb {
  class KalmanFitResult ;
}

namespace LHCb
{

  /** @class FitNode FitNode.h Event/FitNode.h
   *
   *  This File contains the declaration of the FitNode.
   *
   *  A FitNode is a basket of objects at a given z position.
   *  The information inside the FitNode has to be sufficient
   *  to allow smoothing and refitting.
   *
   *  At the moment a FitNode contains or allows you to access
   *  info on the the (kth) measurement,
   *  transport from k --> k + 1 , predicted state at k+1
   *  (predicted from filter step)  and the best state at k
   *  (notation note filter procedes from k -> k + 1 -> k + 2 ......)
   *
   *  @author Eduardo Rodrigues 
   *  @date   2005-04-15 (adaptations to the new track event model)
   *
   *  @author Matthew Needham
   *  @date   19-08-1999
   */
  
  // struct UnidirectionalKalmanFilterData {
  //     UnidirectionalKalmanFilterData() :
  //       deltaChi2(0), status(Initialized),hasInfoUpstream(Unknown) {}
  //     enum FilterStatus {Initialized, Predicted, Filtered, Smoothed } ;
  //     enum CachedBool   {False=0, True=1, Unknown=2;} ;
  //     State             predictedState ;  ///< predicted state of forward/backward filter
  //     State             filteredState ;   ///< filtered state of forward filter
  //     double            deltaChi2 ;       ///< chisq contribution in forward filter
  //     FilterStatus      status ;          ///< Status of the Node in the fit process
  //     CachedBool        hasInfoUpstream ; ///< Are the nodes with active measurement upstream of this node?
  //   } ;


  class FitNode: public Node {
  public:
    // important note: for the Forward fit, smoothed means
    // 'classical'. for the backward fit, it means 'bidirectional'.
    enum FilterStatus {Uninitialized, Initialized, Predicted, Filtered, Smoothed } ;
    enum CachedBool   {False=0, True=1, Unknown=2 } ;    
    enum Direction {Forward=0, Backward=1} ;

    /// Default constructor
    FitNode();

    /// Constructor from a z position and a location
    FitNode( double zPos, LHCb::State::Location location = LHCb::State::LocationUnknown );

    /// Constructor from a Measurement
    FitNode(Measurement& meas );   

    /// Destructor
    virtual ~FitNode();

    /// Clone the Node
    virtual Node* clone() const;

    /// retrieve transport matrix
    const Gaudi::TrackMatrix& transportMatrix() const
    { return m_transportMatrix; }

    /// retrieve invert transport matrix
    const Gaudi::TrackMatrix& invertTransportMatrix() const
    { return m_invertTransportMatrix; }

    /// retrieve transport vector
    const Gaudi::TrackVector& transportVector() const
    { return m_transportVector; }

    /// retrieve noise matrix
    const Gaudi::TrackSymMatrix& noiseMatrix() const { return m_noiseMatrix; }

    /// retrieve noise matrix
    Gaudi::TrackSymMatrix& noiseMatrix() { return m_noiseMatrix; }

    /// set transport matrix
    void setTransportMatrix( const Gaudi::TrackMatrix& transportMatrix ); 

    /// set transport vector
    void setTransportVector( const Gaudi::TrackVector& transportVector ) {
      m_transportVector = transportVector;
    }

    /// set noise matrix
    void setNoiseMatrix( const Gaudi::TrackSymMatrix& noiseMatrix ) {
      m_noiseMatrix = noiseMatrix;
    }

    /// set the seed matrix
    void setSeedCovariance(const Gaudi::TrackSymMatrix& cov )
    {
      m_predictedState[Forward].covariance() = cov ;
      m_predictedState[Backward].covariance() = cov ;
    }

    /// update the projection
    void updateProjection( const Gaudi::TrackProjectionMatrix& H,
			   double refresidual, double errmeasure )
    {
      setProjectionMatrix( H ) ;
      setRefResidual( refresidual ) ;
      setErrMeasure( errmeasure ) ;
      setResidual(0) ;
      setErrResidual(0) ;
      resetFilterStatus(Predicted) ;
    }
    
    /// Retrieve the projection term (obsolete)
    double projectionTerm() const 
    { return m_refResidual + (projectionMatrix()*refVector().parameters())(0) ; }
    
    /// retrieve state predicted by the kalman filter step
    const State& predictedStateForward() const { return predictedState(Forward) ; }

    /// retrieve predicted state from backward filter
    const State& predictedStateBackward() const  { return predictedState(Backward) ; }

    /// retrieve state filtered by the kalman filter step
    const State& filteredStateForward() const { return filteredState(Forward) ; }

    /// retrieve state filtered by the kalman filter step
    const State& filteredStateBackward() const { return filteredState(Backward) ; }

    /// retrieve the state, overloading the inline function in Node
    virtual const State& state() const ;

    /// retrieve the residual, overloading the function in Node
    virtual double residual() const ;

    /// retrieve the residual, overloading the function in Node
    virtual double errResidual() const ;
    
    /// retrieve unbiased residual
    double unbiasedResidual() const { 
      double r = residual() ; // this will trigger the computation
      return type()==HitOnTrack ? r * errMeasure2()/(m_errResidual*m_errResidual) : r ; }
    
    /// retrieve error on unbiased residual
    double errUnbiasedResidual() const {
      double errRes = errResidual() ;  // this will trigger the computation
      return type()==HitOnTrack ? errMeasure2()/errRes : errRes ; }
    
    /// retrieve the unbiased smoothed state at this position
    State unbiasedState() const ;
    
    /// retrieve chisq contribution in upstream filter
    const LHCb::ChiSquare& deltaChi2Forward() const { filteredStateForward(); return m_deltaChi2[Forward] ; }

    /// retrieve chisq contribution in downstream filter
    const LHCb::ChiSquare& deltaChi2Backward() const { filteredStateBackward(); return m_deltaChi2[Backward] ; }

    /// retrieve the total chi2 of the filter including this node
    const LHCb::ChiSquare& totalChi2(int direction) const { filteredState(direction) ; return m_totalChi2[direction%2] ; }

    /// set the residual of the reference
    void setRefResidual( double res ) { m_refResidual = res ; }
    
    /// retrieve the residual of the reference
    double refResidual() const { return m_refResidual ; }
    
    /// set the delta-energy
    void setDeltaEnergy( double e) { m_deltaEnergy = e; }

    /// get the delta-energy
    double deltaEnergy() const { return m_deltaEnergy ; }

    // get the smoother gain matrix
    const Gaudi::TrackMatrix& smootherGainMatrix() const { return m_smootherGainMatrix ; }

    /// set the poca vector
    void setPocaVector( const Gaudi::XYZVector& v ) { m_pocaVector = v ; }

    /// get the poca vector
    const Gaudi::XYZVector& pocaVector() const { return m_pocaVector ; }

    /// set the delta-energy
    void setDoca( double e) { m_doca = e; }

    /// get the delta-energy
    double doca() const { return m_doca ; }

    /// get the filter status (only useful for debugging)
    FilterStatus filterStatus( int direction ) const { return m_filterStatus[direction] ; }

    /// Deactivate this node (outlier)
    void deactivateMeasurement(bool deactivate = true) ;
    
    /// Get the index of this node. For debugging only.
    int index() const ;

    /// set previous node
    void setPreviousNode( FitNode* previousNode ) { 
      m_prevNode = previousNode ; 
      if( m_prevNode ) m_prevNode->m_nextNode = this ;
    }

    /// Unlink this node
    void unLink() {
      m_prevNode = m_nextNode = 0 ;
      m_parent = 0 ;
    }
    
    /// set the parent
    void setParent( KalmanFitResult* p) { m_parent = p ; }

    /// get the parent
    KalmanFitResult* getParent(){ return m_parent ; }
       
    /// update node residual using a smoothed state
    Gaudi::Math::ValueWithError computeResidual(const LHCb::State& state, bool biased) const ;
    
#ifndef GOD_NOALLOC
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(FitNode) == size ? 
               boost::singleton_pool<FitNode, sizeof(FitNode)>::malloc() :
               ::operator new(size) );
    }
  
    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new (size,pObj);
    }
  
    /// operator delete
    static void operator delete ( void* p )
    {
      boost::singleton_pool<FitNode, sizeof(FitNode)>::is_from(p) ?
      boost::singleton_pool<FitNode, sizeof(FitNode)>::free(p) :
      ::operator delete(p);
    }
  
    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
#endif

    protected:
    
    // ! check that the contents of the cov matrix are fine
    bool isPositiveDiagonal( const Gaudi::TrackSymMatrix& mat ) const;

  public:
    const FitNode* prevNode( int direction ) const { return direction==Forward ? m_prevNode : m_nextNode ; }
    const FitNode* nextNode( int direction ) const { return direction==Forward ? m_nextNode : m_prevNode ; }
    
    /// retrieve the predicted state
    const LHCb::State& predictedState( int direction ) const {
      if(  m_filterStatus[direction] < Predicted ) unConst().computePredictedState(direction) ;
      return m_predictedState[ direction ] ;
    }

    /// retrieve the filtered state
    const LHCb::State& filteredState( int direction ) const {
      if(  m_filterStatus[direction] < Filtered ) unConst().computeFilteredState(direction) ;
      return m_filteredState[ direction ] ;
    }
    
    /// retrieve the bismoothed state
    const LHCb::State& biSmoothedState() const {
      if(  m_filterStatus[Backward] < Smoothed ) unConst().computeBiSmoothedState() ;
      return m_state ;
    }
    
    /// retrieve the classically smoothed state
    const LHCb::State& classicalSmoothedState() const {
      if(  m_filterStatus[Forward] < Smoothed ) unConst().computeClassicalSmoothedState() ;
      return m_classicalSmoothedState ;
    }

    /// This is used from the projectors (or from any set method?)
    void resetFilterStatus(FilterStatus s = Initialized) {
      resetFilterStatus(Forward,s) ;
      resetFilterStatus(Backward,s) ;
    }
    

  private:
    void computePredictedState( int direction ) ;
    void computeFilteredState( int direction ) ;
    void computeBiSmoothedState() ;
    void computeClassicalSmoothedState() ;
    
  private:
    /// update node residual using a smoothed state
    void updateResidual(const LHCb::State& state) ;
    
    /// unconst this node
    FitNode& unConst() const { return const_cast<FitNode&>(*this) ; }
    
    /// helper function that caches whether this node has active nodes upstream
    bool hasInfoUpstream( int direction ) const ;

    /// reset the cache for the previous function
    void resetHasInfoUpstream( int direction ) ;

    /// reset the filter status
    void resetFilterStatus( int direction, FilterStatus s = Initialized) ;

  private:

    Gaudi::TrackMatrix    m_transportMatrix;       ///< transport matrix for propagation from previous node to this one
    Gaudi::TrackMatrix    m_invertTransportMatrix; ///< transport matrix for propagation from this node to the previous one
    Gaudi::TrackVector    m_transportVector;       ///< transport vector for propagation from previous node to this one
    Gaudi::TrackSymMatrix m_noiseMatrix;           ///< noise in propagation from previous node to this one
    double                m_deltaEnergy;           ///< change in energy in propagation from previous node to this one
    double                m_refResidual;           ///< residual of the reference    
    FilterStatus          m_filterStatus[2] ;      ///< Status of the Node in the fit process
    CachedBool            m_hasInfoUpstream[2] ;   ///< Are the nodes with active measurement upstream of this node?
    State                 m_predictedState[2];     ///< predicted state of forward/backward filter
    State                 m_filteredState[2];      ///< filtered state of forward filter
    LHCb::State           m_classicalSmoothedState ;
    LHCb::ChiSquare       m_deltaChi2[2];          ///< chisq contribution in forward filter
    LHCb::ChiSquare       m_totalChi2[2];          ///< total chi2 after this filterstep 
    Gaudi::TrackMatrix    m_smootherGainMatrix ;   ///< smoother gain matrix (smoothedfit only)
    Gaudi::XYZVector      m_pocaVector ;           ///< unit vector perpendicular to state and measurement
    double                m_doca ;                 ///< signed doca (of ref-traj). for ST/velo this is equal to minus (ref)residual 
    FitNode*              m_prevNode;              ///< Previous Node
    FitNode*              m_nextNode;              ///< Next Node
    KalmanFitResult*      m_parent ;               ///< Owner
  };

} // namespace LHCb

#endif // TRACKFITEVENT_FITNODE_H

