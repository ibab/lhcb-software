#ifndef TRACKEXTRAPOLATORS_TrackRungeKuttaExtrapolator_H
#define TRACKEXTRAPOLATORS_TrackRungeKuttaExtrapolator_H

// STL
#include <vector>
#include <array>

#include "TrackFieldExtrapolatorBase.h"

// Eigen Objects
#include "LHCbMath/EigenTypes.h"

class TrackRungeKuttaExtrapolator: public TrackFieldExtrapolatorBase
{

private:

  /// The float point precision to use with the Eigen types
  typedef double FloatType;

  /// Basically just a wrapper around the Eigen class, but Zero default constructed...
  template< typename TYPE, int ROWS, int COLUMNS >
  class RKMatrix : public ::Eigen::Matrix<TYPE,ROWS,COLUMNS>
  {
  private:
    typedef ::Eigen::Matrix<TYPE,ROWS,COLUMNS> Base;
  public:
    /// Default constructor adds zero initialisation
    RKMatrix() : Base( Base::Zero() ) { }
    /// Capture all other constructors and just forward them on to the base
    template < typename... Args >
    RKMatrix( Args&&... args ) : Base( std::forward<Args>(args)... ) { }
  };

  /// Type for a 4-vector
  template < typename TYPE = FloatType >
  using RKVec4     = RKMatrix<TYPE,4,1>;

  /// Type for a 4 by 3 Matrix
  template < typename TYPE = FloatType >
  using RKMatrix43 = RKMatrix<TYPE,4,3>;

  /// Represenation of a State
  template < typename TYPE = FloatType >
  class RKState
  {
  public:
    RKState() { }
    RKState( const RKVec4<TYPE>& _vec, const FloatType _qop, const FloatType _z )
      : parameters(_vec), qop(_qop), z(_z) { }
    RKVec4<TYPE> parameters;
    FloatType qop{0} ;
    FloatType z  {0} ;
    TYPE&  x()       noexcept { return parameters(0) ; }
    TYPE&  y()       noexcept { return parameters(1) ; }
    TYPE& tx()       noexcept { return parameters(2) ; }
    TYPE& ty()       noexcept { return parameters(3) ; }
    TYPE  tx() const noexcept { return parameters(2) ; }
    TYPE  ty() const noexcept { return parameters(3) ; }
  } ;

  template < typename TYPE = FloatType >
  class RKStage
  {
  public:
    RKState<TYPE> state ;
    RKState<TYPE> derivative ;
    TrackFieldExtrapolatorBase::FieldVector Bfield ;
  } ;

  template < typename TYPE = FloatType >
  class RKCache
  {
  public:
    RKStage<TYPE> stage[7] ;
    int laststep{-1} ;
    int step{0} ;
  } ;

  template < typename TYPE = FloatType >
  class RKJacobian
  {
  public:

    RKMatrix43<TYPE> matrix;

    TYPE& dXdTx0()  noexcept { return matrix(0,0) ; }
    TYPE& dYdTx0()  noexcept { return matrix(1,0) ; }
    TYPE& dTxdTx0() noexcept { return matrix(2,0) ; }
    TYPE& dTydTx0() noexcept { return matrix(3,0) ; }

    TYPE& dXdTy0()  noexcept { return matrix(0,1) ; }
    TYPE& dYdTy0()  noexcept { return matrix(1,1) ; }
    TYPE& dTxdTy0() noexcept { return matrix(2,1) ; }
    TYPE& dTydTy0() noexcept { return matrix(3,1) ; }

    TYPE& dXdQoP0()  noexcept { return matrix(0,2) ; }
    TYPE& dYdQoP0()  noexcept { return matrix(1,2) ; }
    TYPE& dTxdQoP0() noexcept { return matrix(2,2) ; }
    TYPE& dTydQoP0() noexcept { return matrix(3,2) ; }

    TYPE dTxdTx0() const noexcept { return matrix(2,0) ; }
    TYPE dTydTx0() const noexcept { return matrix(3,0) ; }

    TYPE dTxdTy0() const noexcept { return matrix(2,1) ; }
    TYPE dTydTy0() const noexcept { return matrix(3,1) ; }

    TYPE dTxdQoP0() const noexcept { return matrix(2,2) ; }
    TYPE dTydQoP0() const noexcept { return matrix(3,2) ; }
  } ;

  template < typename TYPE = FloatType >
  inline void addVector4( RKVec4<TYPE>& lhs,
                          const FloatType a,
                          const RKVec4<TYPE>& rhs) const noexcept
  {
    lhs += a * rhs;
  }

  template < typename TYPE = FloatType >
  inline void assignVector4( RKVec4<TYPE>& lhs,
                             const FloatType a,
                             const RKVec4<TYPE>& rhs) const noexcept
  {
    lhs = a * rhs;
  }

  template < typename TYPE = FloatType >
  inline void addMatrix43( RKMatrix43<TYPE>& lhs,
                           const FloatType a,
                           const RKMatrix43<TYPE>& rhs) const noexcept
  {
    lhs += a * rhs;
  }

  template < typename TYPE = FloatType >
  inline void assignMatrix43( RKMatrix43<TYPE>& lhs,
                              const FloatType a,
                              const RKMatrix43<TYPE>& rhs) const noexcept
  {
    lhs = a * rhs;
  }

public:

  /// enums
  //enum RKScheme { CashKarp, DormandPrince, Fehlberg, BogackiShampine } ;
  enum RKErrorCode { RKSuccess, RKOutOfTolerance, RKCurling, RKExceededMaxNumSteps } ;

  /// Constructor
  TrackRungeKuttaExtrapolator( const std::string& type,
                               const std::string& name,
                               const IInterface* parent );

  /// destructor
  virtual ~TrackRungeKuttaExtrapolator();

  /// initialize
  StatusCode initialize();

  /// initialize
  StatusCode finalize();

  using TrackExtrapolator::propagate;

  /// Propagate a state vector from zOld to zNew
  /// Transport matrix is calulated when transMat pointer is not NULL
  StatusCode propagate( Gaudi::TrackVector& stateVec,
                        double zOld,
                        double zNew,
                        Gaudi::TrackMatrix* transMat,
                        LHCb::ParticleID pid = LHCb::ParticleID(211) ) const ;

  // public methods that are not in the interface. used for debugging with the extrapolator tester

private:

  struct RKStatistics
  {
    RKStatistics() {}
    RKStatistics& operator+=( const RKStatistics& rhs )
    {
      minstep = std::min(minstep,rhs.minstep) ;
      maxstep = std::max(maxstep,rhs.maxstep) ;
      err += rhs.err ;
      numstep += rhs.numstep ;
      numfailedstep += rhs.numfailedstep ;
      numincreasedstep += rhs.numincreasedstep ;
      sumstep += rhs.sumstep ;
      return *this ;
    }
    double sumstep{0} ;
    double minstep{1e9} ;
    double maxstep{0} ;
    size_t numstep{0} ;
    size_t numfailedstep{0} ;
    size_t numincreasedstep{0} ;
    RKVec4<> err;
  } ;
  const RKStatistics& statistics() const { return m_stats ; }

private:

  RKErrorCode extrapolate( RKState<>& state, double zout,
                           RKJacobian<>* jacobian, std::vector<double>* stepvector = nullptr ) const ;
  RKErrorCode extrapolateNumericalJacobian( RKState<>& state, double zout, RKJacobian<>& jacobian) const ;

  void evaluateRKStep( double dz, RKState<>& pin, RKVec4<>& err, RKCache<>& cache) const ;
  void evaluateRKStepJacobian( double dz,RKJacobian<>& jacobian,const RKCache<>& cache) const ;
  void evaluateDerivatives(const RKState<>& state,const FieldVector& field,RKState<>& deriv ) const ;
  void evaluateJacobianDerivatives( const RKState<>& state, const RKJacobian<>& jacobian,
                                    const FieldVector& field, RKJacobian<>& jacobianderiv ) const ;

private:

  // tool properties
  double m_toleranceX ;      ///< required absolute position resolution
  double m_relToleranceTx ;  ///< required relative curvature resolution
  double m_minRKStep ;
  double m_maxRKStep ;
  double m_initialRKStep ;
  double m_sigma ;
  double m_minStepScale ;
  double m_maxStepScale ;
  double m_safetyFactor ;
  std::string m_rkscheme ;
  size_t m_maxNumRKSteps ;
  bool   m_correctNumSteps ;
  bool m_numericalJacobian ;
  double m_maxSlope ;
  double m_maxCurvature ;

  // parameters of the runge-kutta (depend on rk-scheme)
  const double* m_a  = nullptr;
  const double* m_b4 = nullptr;
  const double* m_b5 = nullptr;
  bool    m_firstSameAsLast ;
  size_t  m_numStages ;

  // keep statistics for monitoring
  mutable unsigned long long m_numcalls{0} ;
  mutable RKStatistics m_totalstats ; // sum of stats for all calls
  mutable RKStatistics m_stats ;      // rkstats for the last call

};


#endif
