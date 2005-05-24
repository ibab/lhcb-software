// $Id: MeasurementProvider.cpp,v 1.1 2005-05-24 09:52:44 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from TrackFitEvent
#include "Event/ITMeasurement.h"
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
  declareInterface<MeasurementProvider>(this);

  declareProperty( "OTGeometryPath",
                   m_otDetPath = DeOTDetectorLocation::Default );
  declareProperty( "ITGeometryPath",
                   m_itDetPath = DeSTDetectorLocation::Default );
  declareProperty( "VeloGeometryPath",
                   m_otDetPath = "/dd/Structure/LHCb/Velo" );

  declareProperty( "MeasLocation" ,
                   m_measLocation = "/Event/Rec/Tr/Measurements" );
}

//=============================================================================
// Destructor
//=============================================================================
MeasurementProvider::~MeasurementProvider() {};

//=============================================================================
// 
//=============================================================================
StatusCode MeasurementProvider::initialize() {

  m_otDet = getDet<DeOTDetector>( m_otDetPath );
  
  m_itDet = getDet<DeSTDetector>( m_itDetPath );
  
  m_veloDet = getDet<DeVelo>( m_veloDetPath );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// 
//=============================================================================
void MeasurementProvider::load() {

  m_otTimes = get<OTTimes>( OTTimeLocation::Default );

  m_itClusters = get<ITClusters>( ITClusterLocation::Default );

  m_veloClusters = get<VeloClusters>( VeloClusterLocation::Default);
} 

//=============================================================================
// 
//=============================================================================
StatusCode MeasurementProvider::load(Track& track) 
{
  const std::vector<LHCbID>& ids = track.lhcbIDs();
  for (std::vector<LHCbID>::const_iterator it = ids.begin();
       it != ids.end(); it++) {
    const LHCbID& id = *it;
    Measurement& meas = measurement(id);
    track.addToMeasurements(meas);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// 
//=============================================================================
Measurement& MeasurementProvider::measurement ( const LHCbID& id,
                                                double par0,
                                                double par1 ) {

  // TODO first look if it is in the list already :)
  Measurement* meas = NULL;
  if (id.isVelo()) {
    VeloChannelID vid = id.veloID();
    VeloCluster* clus = m_veloClusters->object(vid);
    if (clus != NULL) {    
      if (vid.isRType()) {
        meas = new VeloRMeasurement(*clus,*m_veloDet, par0);
      } else {
        meas = new VeloPhiMeasurement(*clus,*m_veloDet);
      }
    }
  } else if (id.isST()) {
    ITChannelID sid = id.stID();
    ITCluster* clus = m_itClusters->object(sid);
    if (clus != NULL)
      meas = new ITMeasurement(*clus,*m_itDet);
  } else if (id.isOT()) {
    OTChannelID oid = id.otID();
    OTTime* clus = m_otTimes->object(oid);
    if (clus != NULL) {
      if (par0 == 999.) par0 = 0.;
      meas = new OTMeasurement(*clus,*m_otDet, (int) par0, par1);
    }
    
  }

  if (meas != NULL) {
    //meas->setLhcbID(id);
  } else {
    error() << " not able to create measurement " << endreq;
  }

  debug() << " creating measurement " << id.detectorType() 
          << " channel " << id.channelID() 
          << " pars : " << par0 << ","<< par1 << endreq;

  return *meas;  
}

//=============================================================================
