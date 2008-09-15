
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/OTMeasurement.h"
#include "Event/StateVector.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "TrajOTProjector.h"

class TrackOTCosmicsProjector : public TrajOTProjector 
{
public:

  /// Standard constructor
  TrackOTCosmicsProjector( const std::string& type, 
		      const std::string& name,
		      const IInterface* parent );
  
  /// Destructor
  virtual ~TrackOTCosmicsProjector();

  /// The only overloaded call
  StatusCode project( const LHCb::StateVector& state, const LHCb::Measurement& meas);

private:
  double m_tofReferenceZ ;
  bool m_fitEventT0 ;
};

//----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackOTCosmicsProjector );

//-----------------------------------------------------------------------------
/// Project a state onto a measurement
//-----------------------------------------------------------------------------
StatusCode TrackOTCosmicsProjector::project( const LHCb::StateVector& statevector, 
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
    double L0 = ( m_tofReferenceZ - meas.z())*std::sqrt( 1 + statevector.tx()*statevector.tx() + statevector.ty()*statevector.ty()) ;
    double tof = (statevector.ty() > 0 ? -1 : 1) * L0/Gaudi::Units::c_light ;
    // should we subtract a reference time-of-flight? 
    //   tof -= fabs( m_tofReferenceZ - meas.z() ) / Gaudi::Units::c_light ;
    // add this to the measurement, including the phase. we'll use the
    // qop-entry of the statevector to store the phase.
    double eventt0 = statevector.parameters()[4] ;
    // ugly const-cast to update the measurement's time-of-flight
    (const_cast< LHCb::OTMeasurement&>(meas)).setDeltaTimeOfFlight( tof + eventt0 ) ;
    
    // call the standard projector (which uses the time-of-flight)
    sc = TrajOTProjector::project( statevector, meas ) ;
    double distToWire = m_dist.Dot( m_unitPocaVector ) ;
    // update the projection matrix with the derivative to event-t0.
    if( useDrift() && m_fitEventT0 ) {
      // get the drift velocity. for the linearization it is best to
      // have something as closest to the truth as possible. that's not
      // the drift time. so, instead, we compute the inverse:
      double dtdr = meas.module().rtRelation().dtdr( fabs(distToWire) ) ;
      // now we just need to get the sign right. I think that H is MINUS
      // the derivative of the residual in Fruhwirth's languague
      m_H(0,4) = meas.ambiguity() / dtdr ;
    }
  }
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrackOTCosmicsProjector::TrackOTCosmicsProjector( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TrajOTProjector( type, name, parent )
{
  declareProperty( "TofReferenceZ", m_tofReferenceZ = 11*Gaudi::Units::m) ;
  declareProperty( "FitEventT0", m_fitEventT0 = false ) ;
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrackOTCosmicsProjector::~TrackOTCosmicsProjector() {}
