#ifndef TRACKEXTRAPOLATORS_TRACKRUNGEKUTTAEXTRAPOLATOR_H
#define TRACKEXTRAPOLATORS_TRACKRUNGEKUTTAEXTRAPOLATOR_H

#include <vector>
#include "TrackFieldExtrapolatorBase.h"

// Helper classes defined in cpp file
namespace {
  typedef class ROOT::Math::SVector<double, 4u> RKTrackVector ;
  struct RKState ;
  struct RKJacobian ;
  struct RKCache ;
}

class TrackRungeKuttaExtrapolator: public TrackFieldExtrapolatorBase
{

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
public:
  struct RKStatistics
  {
    RKStatistics() : sumstep(0), minstep( 1e9 ), maxstep(0), numstep(0), numfailedstep(0), numincreasedstep(0) {}
    RKStatistics& operator+=( const RKStatistics& rhs ) {
      minstep = std::min(minstep,rhs.minstep) ;
      maxstep = std::max(maxstep,rhs.maxstep) ;
      err += rhs.err ;
      numstep += rhs.numstep ;
      numfailedstep += rhs.numfailedstep ;
      numincreasedstep += rhs.numincreasedstep ;
      sumstep += rhs.sumstep ;
      return *this ;
    }
    double sumstep ;
    double minstep ;
    double maxstep ;
    size_t numstep ;
    size_t numfailedstep ;
    size_t numincreasedstep ;
    RKTrackVector err ;
  } ;
  const RKStatistics& statistics() const { return m_stats ; }

private:
  RKErrorCode extrapolate( RKState& state, double zout, RKJacobian* jacobian, std::vector<double>* stepvector=0) const ;
  RKErrorCode extrapolateNumericalJacobian( RKState& state, double zout, RKJacobian& jacobian) const ;

  void evaluateRKStep( double dz, RKState& pin, RKTrackVector& err, RKCache& cache) const ;
  void evaluateRKStepJacobian( double dz,RKJacobian& jacobian,const RKCache& cache) const ;  
  void evaluateDerivatives(const RKState& state,const FieldVector& field,RKState& deriv ) const ;
  void evaluateJacobianDerivatives( const RKState& state, const RKJacobian& jacobian,
				    const FieldVector& field, RKJacobian& jacobianderiv ) const ;
private:
  // tool properties
  double m_toleranceX ;   /// required absolute position resolution
  double m_relToleranceTx ;  /// required relative curvature resolution
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
  double* m_a ;
  double* m_b4 ;
  double* m_b5 ;
  bool    m_firstSameAsLast ;
  size_t  m_numStages ;

  // keep statistics for monitoring
  mutable size_t m_numcalls ;
  mutable RKStatistics m_totalstats ; // sum of stats for all calls
  mutable RKStatistics m_stats ;      // rkstats for the last call

};


#endif
