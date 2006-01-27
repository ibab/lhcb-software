// $Id: MeasurementProvider.cpp,v 1.14 2006-01-27 13:17:19 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from TrackFitEvent
#include "Event/STMeasurement.h"
#include "Event/OTMeasurement.h"
#include "Event/VeloRMeasurement.h"
#include "Event/VeloPhiMeasurement.h"

// local
#include "MeasurementProvider.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MeasurementProvider
//
// 2005-04-14 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MeasurementProvider>          s_factory ;
const        IToolFactory& MeasurementProviderFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MeasurementProvider::MeasurementProvider( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMeasurementProvider>(this);

  declareProperty( "OTGeometryPath",
                   m_otDetPath = DeOTDetectorLocation::Default );
  declareProperty( "ITGeometryPath",
                   m_itDetPath = DeSTDetectorLocation::Default );
  declareProperty( "VeloGeometryPath",
                   m_veloDetPath = "/dd/Structure/LHCb/Velo" );

  declareProperty( "STPositionTool",
                   m_stPositionToolName = "STOfflinePosition" );

  declareProperty( "MeasLocation" ,
                   m_measLocation = "/Event/Rec/Track/Measurements" );
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

  // Retrieve the Velo, IT and OT detector elements
  m_otDet   = getDet<DeOTDetector>( m_otDetPath );

  m_itDet   = getDet<DeSTDetector>( m_itDetPath );
  
  m_veloDet = getDet<DeVelo>( m_veloDetPath );

  // Retrieve the STClusterPosition tool
  m_stPositionTool = tool<ISTClusterPosition>( m_stPositionToolName );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Load the necessary VeloClusters, ITClusters and OTTimes
//=============================================================================
void MeasurementProvider::load() {
  
  m_otTimes      = get<OTTimes>( OTTimeLocation::Default );
  
  m_itClusters   = get<ITClusters>( ITClusterLocation::Default );
  
  m_veloClusters = get<VeloClusters>( VeloClusterLocation::Default );
} 

//=============================================================================
// Load all the Measurements from the list of LHCbIDs on the input Track
//=============================================================================
StatusCode MeasurementProvider::load( Track& track ) 
{
  std::vector<LHCbID>& allids = track.lhcbIDs();
  for ( std::vector<LHCbID>::iterator it2 = allids.begin();
        it2 != allids.end(); ++it2 ) {
    LHCbID& id = *it2;
    // HACK necessary so that the equality !isOnTrack(meas.lhcbID()) is true
    // and the measurement is actually added to the track!
    // This will become obsolete as soon as the LHCbID is simplified ...
    id.setSpareBits( 0 );
  }

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
	 // TODO: use the StatusCode to return more information on what happened:
	 // not all measurements loaded OR some skipped OR ...
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
                                                double par0,
                                                double par1 )
{
  Measurement* meas = NULL;
  if ( id.isVelo() ) {
    VeloChannelID vid = id.veloID();
    VeloCluster* clus = m_veloClusters->object( vid );
    if (clus != NULL) {
      if (vid.isRType()) {
        meas = new VeloRMeasurement(*clus,*m_veloDet, par0);
      } else {
        meas = new VeloPhiMeasurement(*clus,*m_veloDet);
      }
    }
    else {
      error() << "VeloCluster is NULL! No correspondence to VeloChannelID = "
              << vid << endreq;
    }
  }
  else if ( id.isST() ) {
    ITChannelID sid = id.stID();
    ITCluster* clus = m_itClusters->object(sid);
    if (clus != NULL)
      meas = new STMeasurement( *clus, *m_itDet, *m_stPositionTool );
    else {
      error() << "ITCluster of type " << ( sid.isTT() ? "TT" : "IT" )
              << " is NULL! No correspondence to ITChannelID = "
              << sid << endreq;
    }
  }
  else if ( id.isOT() ) {
    OTChannelID oid = id.otID();
    OTTime* clus = m_otTimes->object(oid);
    debug() << "Looking for OTTime of key = " << oid << endreq;
    if (clus != NULL) {
      if (par0 == 999.) par0 = 0.;
      meas = new OTMeasurement(*clus,*m_otDet, (int) par0, par1);
    }
    else {
      error() << "OTTime is NULL! No correspondence to OTChannelID = "
              << oid << endreq;
    }
  }
  else {
    error() << "LHCbID is not of type Velo, OT, or ST"
            << " (type is " << id.detectorType() << ")" << endreq
            << " -> do not know how to create a Measurement!" << endreq;
  }

  if ( meas == NULL )
    error() << "Unable to create measurement!" << endreq;
  else
    debug() << "Creating measurement of type " << meas -> type()
            << " channel " << id.channelID() 
            << " pars : " << par0 << ","<< par1 << endreq;

  return meas;  
}

//=============================================================================
