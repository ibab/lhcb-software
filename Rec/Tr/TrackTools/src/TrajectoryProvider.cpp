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
  Trajectory* traj = NULL;
  
  switch ( meas.type() )
  {
  case Measurement::VeloR :
    {
      //TODO: this is not complete ...
      // Get the reference to the STMeasurement
      VeloRMeasurement& vmeas = *( dynamic_cast<VeloRMeasurement*>(&meas) );
      // User defined part of the tool input, first position
      // corresponds to projected angle of the track, the second
      // to the radius of the cluster on the Phi sensor (could be
      // set to 0., in that case error will be calculated for the
      // default radius depending on sensor zone), while there are
      // still no good tuning for the angled track available set the
      // angle value to zero and use default error parametrization
      // for the mean LHCb track angle
      IVeloClusterPosition::Pair input( 0., 0. );
      // VeloCluster* cluster
      // the tool pair is < <VeloChannelID, fracPos>, error >
      IVeloClusterPosition::toolPair pos =
        m_veloPositionTool -> position( vmeas.cluster(), input );
      // Channel for the closest strip to the cluster centre:
      //   LHCb::VeloChannelID channel = pos.first.first
      // Fractional distance to the closest strip to the cluster centre
      // expressed in the units of strip distance (local pitch):
      //   double dist = pos.first.second
      // Position error in the units of strip distance (local pitch):
      //   double posErr = pos.second
      //traj = m_veloDet -> trajectory( pos.first.first,
      //                                pos.first.second );
      break;
    }
  case Measurement::VeloPhi :
    {
      //TODO: this is not complete ...
      // Get the reference to the STMeasurement
      VeloPhiMeasurement& vmeas = *( dynamic_cast<VeloPhiMeasurement*>(&meas) );
      // User defined part of the tool input, first position
      // corresponds to projected angle of the track, the second
      // to the radius of the cluster on the Phi sensor (could be
      // set to 0., in that case error will be calculated for the
      // default radius depending on sensor zone), while there are
      // still no good tuning for the angled track available set the
      // angle value to zero and use default error parametrization
      // for the mean LHCb track angle
      IVeloClusterPosition::Pair input( 0., vmeas.r() );
      // VeloCluster* cluster
      // the tool pair is < <VeloChannelID, fracPos>, error >
      IVeloClusterPosition::toolPair pos =
        m_veloPositionTool -> position( vmeas.cluster(), input );
      // Channel for the closest strip to the cluster centre:
      //   LHCb::VeloChannelID channel = pos.first.first
      // Fractional distance to the closest strip to the cluster centre
      // expressed in the units of strip distance (local pitch):
      //   double dist = pos.first.second
      // Position error in the units of strip distance (local pitch):
      //   double posErr = pos.second
      //traj = m_veloDet -> trajectory( pos.first.first,
      //                                pos.first.second );
      break;
    }
  case Measurement::TT :
    {
      //TODO: have an interface in the Cluster Position tools
      // ISTClusterPosition::Measurement estimate( const LHCb::LHCbID& id ) const 
      // so that the dynamic_cast become unnecessary!
      // if this is not possible then this method may need to be removed
      // eventually ...
      //BTW, such a signature of estimate might be crucial/handy for trigger
      // applications, in particular when using the STOnlinePosition!
      //
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
      //TODO: same comments as above
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
  
  //TODO: the following switch block could be replaced by this very simple
  //      and single line if the DetectorElement class had
  // LHCb::Trajectory* trajectory( const LHCb::LHCbID& id, double offset);
  // inline  conditionBegin() const
  //m_dets[ id.detectorType() ] -> trajectory( id, offset );

  switch ( id.detectorType() )
  {
  case LHCbID::Velo :
    //TODO: needs trajectory implementation in Velo detector element
		//traj = m_veloDet -> trajectory( id.veloID(), offset );
		break;
  case LHCbID::TT :
		traj = m_ttDet -> trajectory( id, offset );
		break;
  case LHCbID::IT :
		traj = m_itDet -> trajectory( id, offset );
		break;
  case LHCbID::OT :
		traj = m_otDet -> trajectory( id.otID(), offset );
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
Trajectory* TrajectoryProvider::trajectory( const State& state )
{
  XYZVector bField;
  m_magsvc -> fieldVector( state.position(), bField );
  
  return new StateTraj( state, bField );
};

//=============================================================================
// Return a "State trajectory" from a State vector and a z-position
//=============================================================================
Trajectory* TrajectoryProvider::trajectory( const TrackVector& stateVector,
                                            double z )
{
  XYZVector bField;
  m_magsvc -> fieldVector( XYZPoint( stateVector(0),stateVector(1), z ),
                           bField );
  
  return new StateTraj( stateVector, z, bField );
};

//=============================================================================
