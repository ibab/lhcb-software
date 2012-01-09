// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

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
  // call the standard tracjectory-doca projector
  StatusCode sc = TrackProjector::project( statevector, meas ) ;
  
  OTMeasurement& nonconstmeas = const_cast<OTMeasurement&>(meas) ;
  nonconstmeas.setDriftTimeStrategy( LHCb::OTMeasurement::DriftTimeIgnored ) ;
  m_errMeasure = meas.module().cellRadius()/std::sqrt(3.0) ;
  
  // Is this a 'prefit'?
  bool prefit = m_prefitStrategy!=NoPrefit && meas.ambiguity() == 0 ;
  
  // set the ambiguity "on the fly"
  if( m_updateAmbiguity )
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
    } else {
      // now, there are two problems. first, we have hits assigned to
      // tracks that are just far away: so we will not use drifttimes
      // if the penalty of the chi2 would be too large. we'll choose
      // '3mm' as the maximum distance. (cell radius is 2.5mm) these
      // hits should neverbe on the track, and they would also be 
      // removed by outlier removal but that's quite
      // expensive. instead, we'll just ignore the drifttime. (need to
      // think more.)
      //
      // second, we have a large fraction of hits with very poor
      // drifttime measurements. if they are more than 3 sigma of the
      // prediction, we'll not use driftttime.

      if( m_maxDriftTimePull>0 ) {
	double predictedradius = std::min( rtr.rmax(), std::abs(m_doca) ) ;      
	OTDet::DriftTimeWithError predictedtime = rtr.drifttimeWithError( predictedradius ) ;
	gooddrifttime = std::abs( measureddrifttime - predictedtime.val ) / predictedtime.err < m_maxDriftTimePull ;
      }
      
      if(gooddrifttime ) {
	if(m_fitDriftTime) {
	  double radius = std::min( rtr.rmax(), std::abs(m_doca) ) ;
	  OTDet::DriftTimeWithError predictedtime = rtr.drifttimeWithError( radius ) ;
	  double dtdr                             = rtr.dtdr( radius ) ;
	  m_residual   = measureddrifttime - predictedtime.val ;
	  m_errMeasure = predictedtime.err ;
	  m_H          *= ( meas.ambiguity() * dtdr ) ;
	  nonconstmeas.setDriftTimeStrategy( LHCb::OTMeasurement::FitTime ) ;
	} else {
	  OTDet::RadiusWithError radiusWithError(meas.driftRadiusWithError(m_sMeas)) ;
	  m_residual = -m_doca + meas.ambiguity() * radiusWithError.val ;
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
                                   << m_useDriftTime << endreq 
                                   << "Fit drifttime residuals = " 
                                   << m_fitDriftTime << endreq 
                                   << "Prefit strategy = " 
                                   << m_prefitStrategy << endreq ;
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
