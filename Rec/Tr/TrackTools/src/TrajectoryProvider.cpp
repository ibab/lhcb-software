// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackEvent
#include "Event/StateTraj.h"

// from TrackFitEvent
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

  // Retrieve the STClusterPosition tool
  m_stPositionTool = tool<ISTClusterPosition>( m_stPositionToolName );

  // Retrieve the Velo, ST and OT detector elements
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
  Trajectory* traj = NULL;
  
  switch ( meas.type() )
  {
  case Measurement::VeloR :
    //TODO: needs trajectory implementation in Velo detector element
		break;
  case Measurement::VeloPhi :
    //TODO: needs trajectory implementation in Velo detector element
		break;
  case Measurement::TT :
    {
      // Get the reference to the STMeasurement
      STMeasurement& stmeas = *( dynamic_cast<STMeasurement*>(&meas) );
      // get all the ST measurement info
      ISTClusterPosition::Measurement measVal =
        m_stPositionTool -> estimate( stmeas.cluster() );
      traj = m_ttDet -> trajectory( measVal.first.first,
                                    measVal.first.second );
      break;
    }
  case Measurement::IT :
    {
      // Get the reference to the STMeasurement
      STMeasurement& stmeas = *( dynamic_cast<STMeasurement*>(&meas) );
      // get all the ST measurement info
      ISTClusterPosition::Measurement measVal =
        m_stPositionTool -> estimate( stmeas.cluster() );
      traj = m_itDet -> trajectory( measVal.first.first,
                                    measVal.first.second );
      break;
    }
  case Measurement::OT :
    traj = trajectory( meas.lhcbID() );
		break;
  case Measurement::Muon :
		break;
  default:
		error() << "Measurement is of unknown type!"
            << " (type is " << meas.type() << ")" << endreq
            << " -> do not know how to create a Trajectory!" << endreq;
		return NULL;
  }
  
  if ( traj == NULL )
    error() << "Unable to create the 'Measurement' Trajectory of type "
            << meas.type() << endreq;
  else
    debug() << "Created 'Measurement' Trajectory of type "
            << meas.type() << endreq;
  
  return traj;  
};

//=============================================================================
// Return a "Measurement trajectory" from an LHCbID and an offset
//=============================================================================
Trajectory* TrajectoryProvider::trajectory( const LHCbID& id,
                                            double offset )
{
  Trajectory* traj = NULL;
  
  switch ( id.detectorType() )
  {
  case LHCbID::Velo :
    //TODO: needs trajectory implementation in Velo detector element
		//traj = m_veloDet -> trajectory( id.veloID(), offset );
		break;
  case LHCbID::TT :
		traj = m_ttDet -> trajectory( id.stID(), offset );
		break;
  case LHCbID::IT :
		traj = m_itDet -> trajectory( id.stID(), offset );
		break;
  case LHCbID::OT :
    //TODO: needs trajectory implementation in OT detector element
		//traj = m_otDet -> trajectory( id.otID(), offset );
		break;
  case LHCbID::Rich :
		break;
  case LHCbID::Calo :
		break;
  case LHCbID::Muon :
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
