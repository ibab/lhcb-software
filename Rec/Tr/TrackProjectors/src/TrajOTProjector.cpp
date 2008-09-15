
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
#include "Event/StateZTraj.h"

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
  // Calculate the reference distance and set the ambiguity "on the fly"
  double distToWire = -m_residual ;
  (const_cast<OTMeasurement&>(meas)).setAmbiguity( distToWire > 0 ? 1 : -1 ) ;
  
  if (m_useDrift) {
    OTDet::RadiusWithError radiusWithError(meas.driftRadiusWithError(m_sMeas)) ;
    //m_residual   = -( distToWire - meas.ambiguity() * radiusWithError.val  ) ;
    m_residual   += meas.ambiguity() * radiusWithError.val ;
    m_errMeasure = radiusWithError.err ;
  } else {
    m_errMeasure = meas.module().cellRadius()/std::sqrt(3.0) ;
  }
  m_errResidual = m_errMeasure ;
  
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrajOTProjector::initialize()
{
  StatusCode sc = TrackProjector::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

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
  m_tolerance = 0.01 ;
  declareProperty( "UseDrift", m_useDrift = true );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrajOTProjector::~TrajOTProjector() {}
