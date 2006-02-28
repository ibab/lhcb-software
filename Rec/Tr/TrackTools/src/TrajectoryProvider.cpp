// $Id: TrajectoryProvider.cpp,v 1.3 2006-02-28 19:12:14 jvantilb Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackEvent
#include "Event/StateTraj.h"

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

// Declaration of the Tool Factory
static const  ToolFactory<TrajectoryProvider>          s_factory ;
const        IToolFactory& TrajectoryProviderFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrajectoryProvider::TrajectoryProvider( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrajectoryProvider>(this);

  declareProperty( "VeloPositionTool",
                   m_veloPositionToolName = "VeloClusterPosition" );

  declareProperty( "STPositionTool",
                   m_stPositionToolName = "STOfflinePosition" );

  declareProperty( "VeloGeometryPath",
                   m_veloDetPath = "/dd/Structure/LHCb/Velo" );
  
  declareProperty( "TTGeometryPath",
                   m_ttDetPath = DeSTDetLocation::location("TT") );
  declareProperty( "ITGeometryPath",
                   m_itDetPath = DeSTDetLocation::location("IT") );
  
  declareProperty( "OTGeometryPath",
                   m_otDetPath = DeOTDetectorLocation::Default );
  
  declareProperty( "MagneticFieldService",
                   m_magsvcname = "MagneticFieldSvc" );

  m_dets.clear();
};

//=============================================================================
// Destructor
//=============================================================================
TrajectoryProvider::~TrajectoryProvider() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrajectoryProvider::initialize() {
  
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // Retrieve the Cluster Position tools
  m_veloPositionTool = tool<IVeloClusterPosition>( m_veloPositionToolName );
  m_stPositionTool   = tool<ISTClusterPosition>( m_stPositionToolName );

  // Retrieve the Velo, ST and OT detector elements
  //TODO: useful in trajectory( const LHCbID& id, double offset )
  //      once some new stuff is implemented (see comments in this method)
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
};

//=============================================================================
// Return a "Measurement Trajectory" from a Measurement
//=============================================================================
Trajectory* TrajectoryProvider::trajectory( Measurement& meas )
{
  const Trajectory& traj = meas.trajectory();
  return traj.clone();
};

//=============================================================================
// Return a "Measurement trajectory" from an LHCbID and an offset
//=============================================================================
Trajectory* TrajectoryProvider::trajectory( const LHCbID& id,
                                            double offset )
{
  Trajectory* traj = NULL;

  //TODO: the following switch block could be replaced by this very simple
  //      and single line if the DetectorElement class had
  // LHCb::Trajectory* trajectory( const LHCb::LHCbID& id, double offset);
  // inline  conditionBegin() const
  //m_dets[ id.detectorType() ] -> trajectory( id, offset );

  switch ( id.detectorType() )
  {
  case LHCbID::Velo :
		traj = m_veloDet -> trajectory( id, offset );
		break;
  case LHCbID::TT :
		traj = m_ttDet -> trajectory( id, offset );
		break;
  case LHCbID::IT :
		traj = m_itDet -> trajectory( id, offset );
		break;
  case LHCbID::OT :
		traj = m_otDet -> trajectory( id );
		break;
  default:
		error() << "LHCbID is of unknown type!"
            << " (type is " << id.detectorType() << ")" << endreq
            << " -> do not know how to create a Trajectory!" << endreq;
		return NULL;
  }
  
  if ( traj == NULL )
    error() << "Unable to create the 'Measurement' Trajectory of type "
            << id.detectorType() << endreq;
  else
    debug() << "Created 'Measurement' Trajectory of type "
            << id.detectorType() << endreq;
  
  return traj;
};

//=============================================================================
// Return a "State trajectory" from a State
//=============================================================================
Trajectory* TrajectoryProvider::trajectory( State& state )
{
  XYZVector bField;
  m_magsvc -> fieldVector( state.position(), bField );
  
  return new StateTraj( state, bField );
};

//=============================================================================
// Return a "State trajectory" from a State vector and a z-position
//=============================================================================
Trajectory* TrajectoryProvider::trajectory( TrackVector& stateVector,
                                            double z )
{
  XYZVector bField;
  m_magsvc -> fieldVector( XYZPoint( stateVector(0),stateVector(1), z ),
                           bField );
  
  return new StateTraj( stateVector, z, bField );
};

//=============================================================================
