
// Include files 

#include "Event/OTMeasurement.h"
#include "Event/StateVector.h"
#include "GaudiKernel/PhysicalConstants.h"
#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:654)  // ITrackProjector::project" only partially overridden
  #pragma warning(push)
#endif
#include "TrajOTProjector.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 654
  #pragma warning(pop)
#endif

class TrajOTCosmicsProjector : public TrajOTProjector 
{
public:

  /// Standard constructor
  TrajOTCosmicsProjector( const std::string& type, 
		      const std::string& name,
		      const IInterface* parent );
  
  /// Destructor
  virtual ~TrajOTCosmicsProjector();
  StatusCode initialize() ;

  using TrajOTProjector::project;
  /// The only overloaded call
  StatusCode project( const LHCb::StateVector& state, const LHCb::Measurement& meas);

private:
  double m_tofReferenceZ ;
  bool m_fitEventT0 ;
  bool m_useConstantDriftVelocity ;
};

//----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrajOTCosmicsProjector )

StatusCode TrajOTCosmicsProjector::initialize()
{
  StatusCode sc = TrajOTProjector::initialize() ;
  info() << "Fit event t0 = " << m_fitEventT0 << endmsg ;
  info() << "Use const v_drift = " << m_useConstantDriftVelocity << endmsg ;
  return sc ;
}

//-----------------------------------------------------------------------------
/// Project a state onto a measurement
//-----------------------------------------------------------------------------
StatusCode TrajOTCosmicsProjector::project( const LHCb::StateVector& statevector, 
					    const LHCb::Measurement& ameas )
{
  // (Decided to catch this particlar project call such that I do no
  // need to introduce new virtual function hooks in default
  // projector.)
  StatusCode sc = StatusCode::FAILURE ;
  if ( ameas.checkType( LHCb::Measurement::OT ) ) {
    // typecast
    const LHCb::OTMeasurement& meas =  dynamic_cast<const LHCb::OTMeasurement&>(ameas) ;
    
    // compute the tof correction relative to a reference z.
    double L0 = (meas.z() - m_tofReferenceZ)*std::sqrt( 1 + statevector.tx()*statevector.tx() + statevector.ty()*statevector.ty()) ;
    bool forward = statevector.ty() < 0 ;
    double tof = (forward ? 1 : -1) * L0/Gaudi::Units::c_light ;
    // should we subtract a reference time-of-flight? 
    //   tof -= fabs( m_tofReferenceZ - meas.z() ) / Gaudi::Units::c_light ;
    // add this to the measurement, including the phase. we'll use the
    // qop-entry of the statevector to store the phase.
    double eventt0 = statevector.parameters()[4] ;
    // ugly const-cast to update the measurement's time-of-flight
    (const_cast< LHCb::OTMeasurement&>(meas)).setDeltaTimeOfFlight( (float) (tof + eventt0) ) ;
    
    // need to test ambiguity before calling projector!-(
    // call the standard projector (which uses the time-of-flight)
    sc = TrajOTProjector::project( statevector, meas ) ;
    bool usedrifttime = 
      ( meas.driftTimeStrategy() == LHCb::OTMeasurement::FitTime ||
	meas.driftTimeStrategy() == LHCb::OTMeasurement::FitDistance ) ;
    
    // update the projection matrix with the derivative to event-t0.
    if( usedrifttime && m_fitEventT0 ) {
      if ( fitDriftTime() ) {
	m_H(0,4) = 1 ;
      } else {
	double vdrift = meas.module().rtRelation().drdt() ;
	if( !m_useConstantDriftVelocity ) {
	  // get the drift velocity. for the linearization it is best to
	  // have something as closest to the truth as possible. that's not
	  // the drift time. so, instead, we compute the inverse:
	  double dtdr = meas.module().rtRelation().dtdr( std::abs(m_doca) ) ;
	  vdrift = 1/dtdr ;
	}
	// now we just need to get the sign right. I think that H is MINUS
	// the derivative of the residual in Fruhwirth's languague
	m_H(0,4) = meas.ambiguity() * vdrift ;
      }
    }
  }
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrajOTCosmicsProjector::TrajOTCosmicsProjector( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TrajOTProjector( type, name, parent )
{
  declareProperty( "TofReferenceZ", m_tofReferenceZ = 12.8*Gaudi::Units::m) ;
  declareProperty( "FitEventT0", m_fitEventT0 = true ) ;
  declareProperty( "UseConstantDriftVelocity", m_useConstantDriftVelocity = true ) ;
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrajOTCosmicsProjector::~TrajOTCosmicsProjector() {}
