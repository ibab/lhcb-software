// $Id: TrajMeasurementProvider.cpp,v 1.1 2006-02-23 16:55:20 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from TrackFitEvent
#include "Event/TrajMeasurement.h"

// local
#include "TrajMeasurementProvider.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrajMeasurementProvider
//
// 2006-02-20 : Jose Angel Hernando Morata, Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrajMeasurementProvider>          s_factory ;
const        IToolFactory& TrajMeasurementProviderFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrajMeasurementProvider::TrajMeasurementProvider( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMeasurementProvider>(this);  
}

//=============================================================================
// Destructor
//=============================================================================
TrajMeasurementProvider::~TrajMeasurementProvider() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrajMeasurementProvider::initialize()
{  
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class
  
  // Retrieve the TrajectoryProvider tool
  m_trajProvider = tool<ITrajectoryProvider>( "TrajectoryProvider" );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Load the necessary VeloClusters, STClusters and OTTimes
//=============================================================================
void TrajMeasurementProvider::load()
{  
  // this method is not needed in this case
} 

//=============================================================================
// Load all the Measurements from the list of LHCbIDs on the input Track
//=============================================================================
StatusCode TrajMeasurementProvider::load( Track& track ) 
{
  const std::vector<LHCbID>& ids = track.lhcbIDs();
  for ( std::vector<LHCbID>::const_iterator it = ids.begin();
        it != ids.end(); ++it ) {
    const LHCbID& id = *it;
    // First look if the Measurement corresponding to this LHCbID
    // is already in the Track, i.e. whether it has already been loaded!
    if ( track.isMeasurementOnTrack( id ) ) {
      warning() << "Measurement had already been loaded for the LHCbID"
                << " channelID, detectorType = "
                << id.channelID() << " , " << id.detectorType()
                << "  -> Measurement loading skipped for this LHCbID!"
                << endreq;
      continue;
    }
    Measurement* meas = measurement( id );
    if ( meas != NULL ) track.addToMeasurements( *meas );
    delete meas;
    //if ( meas == NULL ) return StatusCode::FAILURE;
  }
  // Update the status flag of the Track
  track.setStatus( Track::PatRecMeas );
  
  if ( track.nLHCbIDs() != track.nMeasurements() )
    warning() << "-> Track (key=" << track.key()
              << "): loaded successfully only " << track.nMeasurements()
              << " Measurements out of " << track.nLHCbIDs()
              << " LHCbIDs!" << endreq;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Construct a Measurement of the type of the input LHCbID
//=============================================================================
Measurement* TrajMeasurementProvider::measurement ( const LHCbID& id,
                                                    double par )
{
  Measurement* meas = NULL;
  
  Trajectory* traj = m_trajProvider -> trajectory( id, par );
  
  if ( traj == NULL ) {
    error() << "Unable to create the Trajectory!" << endreq
            << "-> unable to create the (Traj)Measurement!" << endreq;
    return meas;
  }
  
  meas = new TrajMeasurement( traj );  // trajectory owned by meas!
  
  if ( meas == NULL )
    error() << "Unable to create TrajMeasurement!" << endreq;
  else
    debug() << "Creating TrajMeasurement of type " << meas -> type()
            << " from LHCbID " << id.channelID()
            << " with parameter " << par << endreq;
  
  return meas;  
}

//=============================================================================
