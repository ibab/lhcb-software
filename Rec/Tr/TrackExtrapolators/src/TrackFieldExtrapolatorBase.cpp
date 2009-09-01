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
TrackFieldExtrapolatorBase::~TrackFieldExtrapolatorBase() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackFieldExtrapolatorBase::initialize()
{
  StatusCode sc = TrackExtrapolator::initialize();
  
  if( sc.isSuccess() ) {
    m_fieldSvc = svc<ILHCbMagnetSvc>(m_fieldSvcName, true);
    
    m_fieldGrid = m_fieldSvc->fieldGrid() ;
    
    if( m_fieldGrid == 0 )
      sc = Error("Cannot retrieve field grid from magfieldvsc", StatusCode::FAILURE ) ;
    else {
      m_fieldFunction = m_useGridInterpolation ?
	&LHCb::MagneticFieldGrid::fieldVector :
	&LHCb::MagneticFieldGrid::fieldVectorClosestPoint ;
      
      debug() << "UseGridInterpolation: " << m_useGridInterpolation << endreq ;
      debug() << "Field in center of magnet (in Tesla): "
	      << fieldVector( Gaudi::XYZPoint( 0, 0, 5000) ) / Gaudi::Units::tesla << endreq ;
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
  debug() << "Number of field calls: " << m_numFieldCalls << endreq ;
  return TrackExtrapolator::finalize();
}
