
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"

// from OTDet
#include"OTDet/DeOTDetector.h"

// from TrackFitEvent
#include "Event/OTMeasurement.h"
#include "Event/StateVector.h"

// from TrackKernel
#include "TrackKernel/StateZTraj.h"

// from TrackInterfaces
#include "Kernel/ITrajPoca.h"

// from LHCbKernel
#include "Kernel/AlignTraj.h"

// local
#include "TrajOTProjector.h"

// Namespaces
using namespace Gaudi;
using namespace LHCb;
using ROOT::Math::SMatrix;

DECLARE_TOOL_FACTORY( TrajOTProjector );

StatusCode TrajOTProjector::project( const StateVector& statevector,
                                     const Measurement& meas )
{
  return meas.checkType( Measurement::OT ) ?
    project(statevector, dynamic_cast<const OTMeasurement&>(meas) ) :
    StatusCode::FAILURE;
}

//-----------------------------------------------------------------------------
/// Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrajOTProjector::project( const LHCb::StateVector& statevector, 
                                     const OTMeasurement& meas )
{
  StatusCode sc = TrackProjector::project( statevector, meas ) ;
  // set the ambiguity "on the fly"
  (const_cast<OTMeasurement&>(meas)).setAmbiguity( m_doca > 0 ? 1 : -1 ) ;
  
  if (m_useDriftTime) {
    if(m_fitDriftTime) {
      const OTDet::RtRelation& rtr = meas.module().rtRelation() ;
      double radius = std::min( rtr.rmax(), std::abs(m_doca) ) ;
      OTDet::DriftTimeWithError time = rtr.drifttimeWithError( radius ) ;
      double dtdr                    = rtr.dtdr( radius ) ;
      m_residual   = meas.driftTime(m_sMeas) - time.val ;
      m_errMeasure = time.err ;
      m_H          *= ( meas.ambiguity() * dtdr ) ;
    } else {
      OTDet::RadiusWithError radiusWithError(meas.driftRadiusWithError(m_sMeas)) ;
      m_residual = -m_doca + meas.ambiguity() * radiusWithError.val ;
      m_errMeasure = radiusWithError.err ;
     }
  } else {
    m_errMeasure = meas.module().cellRadius()/std::sqrt(3.0) ;
  }
  m_errResidual = m_errMeasure ;
  
  return sc;
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrajOTProjector::initialize()
{
  StatusCode sc = TrackProjector::initialize();
  info() << "Use drifttime           = " << m_useDriftTime << endreq ;
  info() << "Fit drifttime residuals = " << m_fitDriftTime << endreq ;
  return sc;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrajOTProjector::TrajOTProjector( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TrackProjector( type, name, parent )
{
  declareInterface<ITrackProjector>(this);
  m_tolerance = 0.001 ;
  declareProperty( "UseDrift", m_useDriftTime = true );
  declareProperty( "FitDriftTime", m_fitDriftTime = false );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrajOTProjector::~TrajOTProjector() {}
