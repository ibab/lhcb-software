// Include files 
// -------------
// from TrackKernel
#include "TrackKernel/StateTraj.h"

// from TrackFitEvent
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/STMeasurement.h"

// local
#include "TrajectoryProvider.h"

using namespace Gaudi;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TrajectoryProvider
//
// 2006-02-17 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrajectoryProvider )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrajectoryProvider::TrajectoryProvider( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrajectoryProvider>(this);

  declareProperty( "VeloGeometryPath",
                   m_veloDetPath = DeVeloLocation::Default );
  
  declareProperty( "TTGeometryPath",
                   m_ttDetPath = DeSTDetLocation::location("TT") );
  declareProperty( "ITGeometryPath",
                   m_itDetPath = DeSTDetLocation::location("IT") );
  
  declareProperty( "OTGeometryPath",
                   m_otDetPath = DeOTDetectorLocation::Default );
  
  declareProperty( "MagneticFieldService",
                   m_magsvcname = "MagneticFieldSvc" );

  //m_dets.clear();
}

//=============================================================================
// Destructor
//=============================================================================
TrajectoryProvider::~TrajectoryProvider() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrajectoryProvider::initialize() {
  
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // Retrieve the Velo, ST and OT detector elements
  //m_dets[LHCbID::Velo] = getDet<DeVelo>( m_veloDetPath );
  //m_dets[LHCbID::TT]   = getDet<DeSTDetector>( m_ttDetPath );
  //m_dets[LHCbID::IT]   = getDet<DeSTDetector>( m_itDetPath );
  //m_dets[LHCbID::OT]   = getDet<DeOTDetector>( m_otDetPath );

  m_veloDet = getDet<DeVelo>( m_veloDetPath );
  
  m_ttDet   = getDet<DeSTDetector>( m_ttDetPath );
  m_itDet   = getDet<DeSTDetector>( m_itDetPath );
  
  m_otDet   = getDet<DeOTDetector>( m_otDetPath );
  
  // Retrieve the Magnetic Field Service
  m_magsvc = svc<IMagneticFieldSvc>( m_magsvcname, true );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Return a "Measurement Trajectory" from a Measurement
//=============================================================================
const Trajectory* TrajectoryProvider::trajectory( const Measurement& meas )
{
  return &meas.trajectory();
}

//=============================================================================
// Return a "Measurement trajectory" from an LHCbID and an offset
//=============================================================================
std::auto_ptr<Trajectory> TrajectoryProvider::trajectory( const LHCbID& id,
                                                          const double offset )
{
  //TODO: the following switch block could be replaced by this very simple
  //      and single line if the DetectorElement class had
  // LHCb::Trajectory* trajectory( const LHCb::LHCbID& id, double offset);
  //m_dets[ id.detectorType() ] -> trajectory( id, offset );

  switch ( id.detectorType() )
  {
  case LHCbID::Velo :
    return m_veloDet -> trajectory( id, offset );
  case LHCbID::TT :
    return m_ttDet -> trajectory( id, offset );
  case LHCbID::IT :
    return m_itDet -> trajectory( id, offset );
  case LHCbID::OT :
    return m_otDet -> trajectory( id, offset );
  default:
		error() << "LHCbID is of unknown type!"
            << " (type is " << id.detectorType() << ")" << endmsg
            << " -> do not know how to create a Trajectory!" << endmsg;
		return std::auto_ptr<Trajectory>( NULL );
  }
}

//=============================================================================
// Return a "State trajectory" from a State
//=============================================================================
std::auto_ptr<Trajectory> TrajectoryProvider::trajectory( const State& state )
{
  XYZVector bField;
  m_magsvc -> fieldVector( state.position(), bField );
  
  return std::auto_ptr<Trajectory>(new StateTraj( state, bField ));
}

//=============================================================================
// Return a "State trajectory" from a State vector and a z-position
//=============================================================================
std::auto_ptr<Trajectory> TrajectoryProvider::trajectory( const Gaudi::TrackVector& stateVector,
                                                          const double z )
{
  XYZVector bField;
  m_magsvc -> fieldVector( XYZPoint( stateVector(0),stateVector(1), z ),
                           bField );
  
  return std::auto_ptr<Trajectory>(new StateTraj( stateVector, z, bField ));
}

//=============================================================================
