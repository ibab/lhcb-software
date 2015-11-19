// Include files

// from TrackFitEvent
#include "Event/OTMeasurement.h"
#include "Event/StateVector.h"

// local
#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:654)  // ITrackProjector::project" only partially overridden
  #pragma warning(push)
#endif
#include "TrajOTProjector.h"
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 654
  #pragma warning(pop)
#endif

// Namespaces
using namespace Gaudi;
using namespace LHCb;
using ROOT::Math::SMatrix;

DECLARE_TOOL_FACTORY( TrajOTProjector )

StatusCode TrajOTProjector::project( const StateVector& statevector,
                                     const Measurement& meas )
{
  return meas.checkType( Measurement::OT ) ?
    project(statevector, static_cast<const OTMeasurement&>(meas) ) :
    StatusCode::FAILURE;
}

//-----------------------------------------------------------------------------
/// Project a state onto a measurement
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrajOTProjector::project( const LHCb::StateVector& statevector,
                                     const OTMeasurement& meas )
{
  // call the standard tracjectory-doca projector
  StatusCode sc = TrackProjector::project( statevector, meas ) ;

  OTMeasurement& nonconstmeas = const_cast<OTMeasurement&>(meas) ;
  nonconstmeas.setDriftTimeStrategy( LHCb::OTMeasurement::DriftTimeIgnored ) ;
  m_errMeasure = meas.module().cellRadius()/std::sqrt(3.0) ;

  // Is this a 'prefit'?
  bool prefit = m_prefitStrategy!=NoPrefit && meas.ambiguity() == 0 ;

  // is the ambiguity set by the LR sign tool?
  bool ambiguityIsFixed = std::abs( meas.ambiguity() ) == 2 ;

  // if not, set the ambiguity "on the fly"
  if( !ambiguityIsFixed && m_updateAmbiguity )
    nonconstmeas.setAmbiguity( m_doca > 0 ? 1 : -1 ) ;

  // check that the drifttime is not out of range
  double measureddrifttime = meas.driftTime(m_sMeas) ;
  const OTDet::RtRelation& rtr = meas.module().rtRelation() ;
  bool gooddrifttime = useDriftTime() &&
    measureddrifttime > -m_outOfTimeTolerance &&
    measureddrifttime < rtr.tmax() + m_outOfTimeTolerance ;

  if ( gooddrifttime ) {
    // a zero ambiguity may indicate that we should not use the drifttime
    if(prefit) {
      if(m_prefitStrategy==TjeerdKetel) {
      nonconstmeas.setDriftTimeStrategy( LHCb::OTMeasurement::PreFit ) ;
      // This is the implementation of a proprosal by Tjeerd: assign
      // error based on the driftdistance
      OTDet::RadiusWithError radiusWithError = meas.driftRadiusWithError( m_sMeas  ) ;
      m_errMeasure = std::sqrt( radiusWithError.val * radiusWithError.val +
				radiusWithError.err * radiusWithError.err ) ;
      }
    } else if(meas.ambiguity()!=0) {
      // we have a large fraction of hits with very poor drifttime
      // measurements. if they are more than 3 sigma of the
      // prediction, we'll not use driftttime.

      if( m_maxDriftTimePull>0 ) {
	decltype(rtr.rmax()) predictedradius = std::min( rtr.rmax(), std::abs(static_cast<decltype(rtr.rmax())>(m_doca)) ) ;
	OTDet::DriftTimeWithError predictedtime = rtr.drifttimeWithError( predictedradius ) ;
	gooddrifttime = std::abs( measureddrifttime - predictedtime.val ) / predictedtime.err < m_maxDriftTimePull ;
      }

      if(gooddrifttime ) {
	int lrsign =  meas.ambiguity() > 0 ? 1 : -1 ;
	if(m_fitDriftTime) {
	  decltype(rtr.rmax()) radius = std::min( rtr.rmax(), std::abs(static_cast<decltype(rtr.rmax())>(m_doca)) ) ;
	  OTDet::DriftTimeWithError predictedtime = rtr.drifttimeWithError( radius ) ;
	  float dtdr                             = rtr.dtdr( radius ) ;
	  m_residual   = measureddrifttime - predictedtime.val ;
	  m_errMeasure = predictedtime.err ;
	  m_H          *= ( lrsign * dtdr ) ;
	  nonconstmeas.setDriftTimeStrategy( LHCb::OTMeasurement::FitTime ) ;
	} else {
	  OTDet::RadiusWithError radiusWithError(meas.driftRadiusWithError(m_sMeas)) ;
	  m_residual = -m_doca + lrsign * radiusWithError.val ;
	  m_errMeasure = radiusWithError.err ;
	  nonconstmeas.setDriftTimeStrategy( LHCb::OTMeasurement::FitDistance ) ;
	}
      }
    }
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
  if(msgLevel(MSG::DEBUG)) debug() << "Use drifttime           = "
                                   << m_useDriftTime << endmsg
                                   << "Fit drifttime residuals = "
                                   << m_fitDriftTime << endmsg
                                   << "Prefit strategy = "
                                   << m_prefitStrategy << endmsg ;
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
  declareProperty( "UpdateAmbiguity", m_updateAmbiguity = true ) ;
  declareProperty( "PrefitStrategy", m_prefitStrategy = TjeerdKetel ) ;
  declareProperty( "OutOfTimeTolerance", m_outOfTimeTolerance = 6 ) ;
  declareProperty( "MaxDriftTimePull", m_maxDriftTimePull = 3 ) ;
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrajOTProjector::~TrajOTProjector() {}
