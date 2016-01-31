// Include files
// -------------
// from Gaudi

#include "Kernel/ILHCbMagnetSvc.h"
#include "TrackFieldExtrapolatorBase.h"

//=============================================================================
// TrackFieldExtrapolatorBase constructor.
//=============================================================================
TrackFieldExtrapolatorBase::TrackFieldExtrapolatorBase( const std::string& type,
							const std::string& name,
							const IInterface* parent )
  : TrackExtrapolator(type, name, parent) 
{
  declareProperty("FieldSvc", m_fieldSvcName = "MagneticFieldSvc") ;
  declareProperty("UseGridInterpolation", m_useGridInterpolation = true ) ;
}

//=============================================================================
// TrackFieldExtrapolatorBase destructor.
//=============================================================================
TrackFieldExtrapolatorBase::~TrackFieldExtrapolatorBase() = default;

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackFieldExtrapolatorBase::initialize()
{
  StatusCode sc = TrackExtrapolator::initialize();
  
  if( sc.isSuccess() ) {
    m_fieldSvc = service(m_fieldSvcName, true);
    m_fieldGrid = m_fieldSvc->fieldGrid() ;
    
    if( !m_fieldGrid )
      sc = Error("Cannot retrieve field grid from magfieldvsc", StatusCode::FAILURE ) ;
    else {
      m_fieldFunction = m_useGridInterpolation ?
	&LHCb::MagneticFieldGrid::fieldVector :
	&LHCb::MagneticFieldGrid::fieldVectorClosestPoint ;
      
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
        debug() << "UseGridInterpolation: " << m_useGridInterpolation << endmsg ;
        debug() << "Field in center of magnet (in Tesla): "
                << fieldVector( Gaudi::XYZPoint( 0, 0, 5000) ) / Gaudi::Units::tesla << endmsg ;
      }
    }
  }
  m_numFieldCalls = 0 ;

  return sc ;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackFieldExtrapolatorBase::finalize()
{
  m_fieldSvc.reset();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Number of field calls: " << m_numFieldCalls << endmsg ;
  return TrackExtrapolator::finalize();
}
