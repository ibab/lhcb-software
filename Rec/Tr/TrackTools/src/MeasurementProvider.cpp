// $Id: MeasurementProvider.cpp,v 1.24 2006-05-19 11:55:51 dhcroft Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from TrackFitEvent
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/STMeasurement.h"
#include "Event/OTMeasurement.h"

// local
#include "MeasurementProvider.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MeasurementProvider
//
// 2005-04-14 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MeasurementProvider );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MeasurementProvider::MeasurementProvider( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMeasurementProvider>(this);

  declareProperty( "TTClusterPositionTool",
                   m_ttPositionToolName = "STOfflinePosition" );
  declareProperty( "ITClusterPositionTool",
                   m_itPositionToolName = "STOfflinePosition/ITClusterPosition" );

  declareProperty( "VeloPositionTool",
                   m_veloPositionToolName = "VeloClusterPosition" );

  declareProperty( "VeloGeometryPath",
                   m_veloDetPath = DeVeloLocation::Default );

  declareProperty( "TTGeometryPath",
                   m_ttDetPath = DeSTDetLocation::location("TT") );
  declareProperty( "ITGeometryPath",
                   m_itDetPath = DeSTDetLocation::location("IT") );

  declareProperty( "OTGeometryPath",
                   m_otDetPath = DeOTDetectorLocation::Default );

  declareProperty( "IgnoreVelo", m_ignoreVelo = false );
  declareProperty( "IgnoreTT",   m_ignoreTT   = false );
  declareProperty( "IgnoreIT",   m_ignoreIT   = false );
  declareProperty( "IgnoreOT",   m_ignoreOT   = false );

}

//=============================================================================
// Destructor
//=============================================================================
MeasurementProvider::~MeasurementProvider() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode MeasurementProvider::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class

  // Retrieve the STClusterPosition tools
  m_ttPositionTool = tool<ISTClusterPosition>( m_ttPositionToolName );
  m_itPositionTool = tool<ISTClusterPosition>( m_itPositionToolName );

  // Retrieve the VeloClusterPosition tool
  m_veloPositionTool = tool<IVeloClusterPosition>( m_veloPositionToolName );

  // Retrieve the Velo, ST and OT detector elements
  if ( !m_ignoreVelo ) m_veloDet = getDet<DeVelo>( m_veloDetPath );

  if ( !m_ignoreTT ) m_ttDet   = getDet<DeSTDetector>( m_ttDetPath );
  if ( !m_ignoreIT ) m_itDet   = getDet<DeSTDetector>( m_itDetPath );

  if ( !m_ignoreOT ) m_otDet   = getDet<DeOTDetector>( m_otDetPath );
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Load the necessary VeloClusters, STClusters and OTTimes
//=============================================================================
void MeasurementProvider::load()
{
  if ( !m_ignoreVelo ) {
    if ( exist<VeloClusters>( VeloClusterLocation::Default ) )
      m_veloClusters = get<VeloClusters>( VeloClusterLocation::Default );
    else
      error() << "VeloClusters asked to be loaded but not present!" << endreq;
  }
  
  if ( !m_ignoreTT ) {
    if ( exist<STClusters>( STClusterLocation::TTClusters ) )
      m_ttClusters = get<STClusters>( STClusterLocation::TTClusters );
    else
      error() << "STClusters in TT asked to be loaded but not present!"
              << endreq;
  }
  
  if ( !m_ignoreIT ) {
    if ( exist<STClusters>( STClusterLocation::ITClusters ) )
      m_itClusters = get<STClusters>( STClusterLocation::ITClusters );
    else
      error() << "STClusters in IT asked to be loaded but not present!"
              << endreq;
  }
  
  if ( !m_ignoreOT ) {
    if ( exist<OTTimes>( OTTimeLocation::Default ) )
      m_otTimes = get<OTTimes>( OTTimeLocation::Default );
    else
      error() << "OTTimes asked to be loaded but not present!" << endreq;
  }
}

//=============================================================================
// Load all the Measurements from the list of LHCbIDs on the input Track
//=============================================================================
StatusCode MeasurementProvider::load( Track& track ) 
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
Measurement* MeasurementProvider::measurement ( const LHCbID& id,
                                                double par )
{
  Measurement* meas = NULL;
  if ( id.isVelo() && !m_ignoreVelo ) {
    VeloChannelID vid = id.veloID();
    VeloCluster* clus = m_veloClusters->object( vid );
    if (clus != NULL) {
      if (vid.isRType()) {
        meas = new VeloRMeasurement( *clus, *m_veloDet, *m_veloPositionTool );
      } else {
        meas = new VeloPhiMeasurement(*clus, *m_veloDet, *m_veloPositionTool );
      }
    }
    else {
      error() << "VeloCluster is NULL! No correspondence to VeloChannelID = "
              << vid << endreq;
    }
    if ( meas == NULL )
      error() << "Unable to create Velo measurement!" << endreq;
  }
  else if ( id.isTT() && !m_ignoreTT ) {
    STChannelID sid = id.stID();
    STCluster* clus = m_ttClusters->object(sid);
    if (clus != NULL)
      meas = new STMeasurement( *clus, *m_ttDet, *m_ttPositionTool );
    else {
      error() << "STCluster of type TT is NULL! No correspondence to "
              << "STChannelID = " << sid << endreq;
    }
    if ( meas == NULL )
      error() << "Unable to create TT measurement!" << endreq;
  }
  else if ( id.isIT() && !m_ignoreIT ) {
    STChannelID sid = id.stID();
    STCluster* clus = m_itClusters->object(sid);
    if (clus != NULL)
      meas = new STMeasurement( *clus, *m_itDet, *m_itPositionTool );
    else {
      error() << "STCluster of type IT is NULL! No correspondence to "
              << "STChannelID = " << sid << endreq;
    }
    if ( meas == NULL )
      error() << "Unable to create IT measurement!" << endreq;
  }
  else if ( id.isOT() && !m_ignoreOT ) {
    OTChannelID oid = id.otID();
    OTTime* clus = m_otTimes->object(oid);
    if (clus != NULL) {
      meas = new OTMeasurement( *clus, *m_otDet, (int) par );
    }
    else {
      error() << "OTTime is NULL! No correspondence to OTChannelID = "
              << oid << endreq;
    }
    if ( meas == NULL )
      error() << "Unable to create OT measurement!" << endreq;
  }
  else {
    error() << "LHCbID is not of type Velo, OT, or ST"
            << " (type is " << id.detectorType() << ")" << endreq
            << " -> do not know how to create a Measurement!" << endreq;
  }

  if ( meas != NULL )
    debug() << "Creating measurement of type " << meas -> type()
            << " channel " << id.channelID() 
            << " parameter : " << par << endreq;

  return meas;  
}

//=============================================================================
