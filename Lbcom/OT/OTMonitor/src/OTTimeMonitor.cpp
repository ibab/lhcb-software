// $Id: OTTimeMonitor.cpp,v 1.1.1.1 2004-09-03 13:30:48 jnardull Exp $

// OTMonitor
#include "OTMonitor/OTTimeMonitor.h"

static const AlgFactory<OTTimeMonitor> s_Factory;
const IAlgFactory& OTTimeMonitorFactory = s_Factory;

/** @file OTTimeMonitor.cpp 
 *
 *  Implementation of OTTimeMonitor algorithm.
 *  @author J. van Tilburg & Jacopo Nardulli
 *  @date   20-07-2004
 */

OTTimeMonitor::OTTimeMonitor(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  OTMonitorAlgorithm(name, pSvcLocator)
{
  // constructor
}

OTTimeMonitor::~OTTimeMonitor()
{
  // destructor
}

StatusCode OTTimeMonitor::initialize()
{
  // Loading OT Geometry from XML
  SmartDataPtr<DeOTDetector> tracker( detSvc(), "/dd/Structure/LHCb/OT" );
  if ( !tracker ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Unable to retrieve Tracker detector element"
        << " from xml." << endreq;
    return StatusCode::FAILURE;
  }
  m_tracker = tracker;
  m_numStations = m_tracker->numStations();
  m_firstOTStation = m_tracker->firstOTStation();

  // intialize histos
  this->initHistograms();

  // Store number of channels (per station, layer etc)
  m_nChannelsTopModule.resize(40, 0);
  m_nChannelsCornerModule.resize(40, 0);
  m_nChannelsSideModule.resize(40, 0);
  m_nChannelsPerLayer.resize(40, 0);
  m_nChannelsPerStation.resize(4, 0);
  std::vector<DeOTModule*> modules = m_tracker->modules();
  std::vector<DeOTModule*>::const_iterator module;
  for (module = modules.begin(); module != modules.end(); ++module) {
    int iStation = (*module)->stationID();
    int iLayer = (*module)->layerID();
    int iUniqueLayerNum = 10*iStation+iLayer;
    int nChannels = (*module)->nChannels();
    int moduleID = (*module)->moduleID();
    if ( moduleID == 9 ) m_nChannelsTopModule[iUniqueLayerNum] += nChannels;
    if ( moduleID == 8 ) m_nChannelsCornerModule[iUniqueLayerNum] += nChannels;
    if ( moduleID == 7 ) m_nChannelsSideModule[iUniqueLayerNum] += nChannels;
    m_nChannelsPerLayer[iUniqueLayerNum] += nChannels;
    m_nChannelsPerStation[iStation] += nChannels;
  }
  
  // Get the number of events to be processed. Needed for occupancy.
  SmartIF<IProperty> appmgr(IID_IProperty, serviceLocator());
  if( !appmgr.isValid() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Unable to locate the ApplicationMgr" << endmsg;
    m_evtMax = 1;
  }
  else {
    IntegerProperty numEvt;
    numEvt.assign( appmgr->getProperty( "EvtMax" ) );
    m_evtMax = numEvt;
  }

  return StatusCode::SUCCESS;
}


StatusCode OTTimeMonitor::execute()
{
  // execute
  
  // retrieve OTTimes
  SmartDataPtr<OTTimes> times(eventSvc(), OTTimeLocation::Default);
  if (!times){
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Failed to find OTTimes container" << endreq;
    return StatusCode::FAILURE;
  }

  // number of times
  m_nTimesHisto->fill( (double)times->size(), 1.0);

  // histos per time
  OTTimes::const_iterator iterTime;
  for(iterTime = times->begin(); iterTime != times->end(); iterTime++){
    this->fillHistograms(*iterTime);
  } // loop iterDep

  
  return StatusCode::SUCCESS;
}

StatusCode OTTimeMonitor::finalize()
{
  // Finalize
  return StatusCode::SUCCESS;
}

StatusCode OTTimeMonitor::initHistograms()
{
  // Intialize histograms
  std::string tDirPath = this->histoDirPath();

  // number of Times in container
  m_nTimesHisto= histoSvc()->book(tDirPath+"1","num Times",200,0.,20000.);
    
  // number of Times per station
  if ( fullDetail() ) {
    m_nTimesPerStationHisto = histoSvc()->book( tDirPath+"11",
                                                "n Times per stat",
                                                3, 0.5, 3.5);
  }
  
  // number of Times per layer 
  m_nTimesPerLayerHisto = histoSvc()->book( tDirPath+"12","n Times per layer",
                                            25, 9.5, 34.5);
  
  // occupancy per station
  if ( fullDetail() ) {
    m_occPerStationHisto = histoSvc()->book( tDirPath+"21", 
                                             "occupancy per station",
                                             3, 0.5, 3.5);
  }
  
  // occupancy per layer
  m_occPerLayerHisto = histoSvc()->book( tDirPath+"22", "occupancy per layer",
                                         25, 9.5, 34.5);
  
  if ( fullDetail() ) {
    // occupancy in top module per layer 
    m_occTopPerLayerHisto = 
      histoSvc()->book( tDirPath+"23", "occupancy in top module per layer", 
                        25, 9.5, 34.5);
  
    // number of Times in corner module per layer 
    m_occCornerPerLayerHisto = 
      histoSvc()->book(tDirPath+"24", "occupancy in corner module per layer",
                       25, 9.5, 34.5);
    
    // occupancy in side module per layer 
    m_occSidePerLayerHisto = 
      histoSvc()->book(tDirPath+"25", "occupancy in side module per layer",
                       25, 9.5, 34.5);
    
    // occupancy versus x coordinate in T1 layer 1  
    m_occVsxHisto = 
      histoSvc()->book(tDirPath+"31", "occupancy in T1 layer 1 vs x", 
                       50, 0.0, 2984.625);
  }
  
  // histograms per station
  int ID;
  IHistogram1D* aHisto1D;
  for (int iStation = m_firstOTStation ;iStation <= m_numStations; ++iStation){

    if(fullDetail()){
      // Tdc time spectra
      ID = 100 + iStation;
      std::string aString= this->intToString(ID);
      aHisto1D = histoSvc()->book(tDirPath+this->intToString(ID),
                                  "Bin - time station "
                                  +this->intToString(iStation),
                                  250, -50.0*ns, 200.0*ns);
      m_tdcTimeHistos.push_back(aHisto1D);
    }
    // Calibrated time spectra
    ID = 300 + iStation;
    std::string aString = this->intToString(ID);
    aHisto1D = histoSvc()->book(tDirPath+this->intToString(ID),
                                "Calibrated time station "
                                +this->intToString(iStation),
                                250, -50.0*ns, 200.0*ns);
    m_calTimeHistos.push_back(aHisto1D);
    
  } // loop station
  
  return StatusCode::SUCCESS;
}

StatusCode OTTimeMonitor::fillHistograms(OTTime* aTime)
{
  // Initialize

  // Times and occupancy per station
  OTChannelID channel = aTime->channel();
  const int iStation = channel.station();
  double weight = 100./(m_evtMax * m_nChannelsPerStation[iStation]);
  if ( fullDetail() ) {
    m_nTimesPerStationHisto->fill((double)iStation, 1.0);
    m_occPerStationHisto->fill((double)iStation, weight);
  }

  // times and occupancies per layer (for all, top, corner and side modules)
  const int iLayer = channel.layer();
  int iUniqueLayerNum = 10*iStation + iLayer;
  weight = 100./(m_evtMax * m_nChannelsPerLayer[iUniqueLayerNum]);
  m_nTimesPerLayerHisto->fill((double)iUniqueLayerNum, 1.);
  m_occPerLayerHisto->fill((double)iUniqueLayerNum, weight);

  if ( fullDetail() ) {
    unsigned int iModule = channel.module();
    if ( iModule == 9 ) {
      weight = 100./(m_evtMax * m_nChannelsTopModule[iUniqueLayerNum] );
      m_occTopPerLayerHisto->fill((double)iUniqueLayerNum, weight);
    }
    if (iModule == 8 ) {
      weight = 100./(m_evtMax * m_nChannelsCornerModule[iUniqueLayerNum] );    
      m_occCornerPerLayerHisto->fill((double)iUniqueLayerNum, weight);
    }
    if ( iModule == 7 ) {
      weight = 100./(m_evtMax * m_nChannelsSideModule[iUniqueLayerNum] );
      m_occSidePerLayerHisto->fill((double)iUniqueLayerNum, weight);
    }

    // number of times versus x-coordinate for T1 layer 1.
    if (iStation == 3 && iLayer == 1) {
      DeOTModule* module = m_tracker->module(channel);
      double channelsPerBin = m_nChannelsPerLayer[iUniqueLayerNum]/50.;
      weight = 100 / ( m_evtMax * channelsPerBin );
      HepPoint3D hit = module->centerOfStraw(channel.straw());
      m_occVsxHisto->fill(fabs(hit.x()), weight);
    }
  
    // Bin time for every station
    int tdcTime = channel.tdcTime();
    double tdc = (double) (tdcTime);
    m_tdcTimeHistos[iStation-m_firstOTStation]->fill( tdc, 1.);
  }
  
  // calibrated time for every station
  m_calTimeHistos[iStation-m_firstOTStation]->fill(aTime->calibratedTime(), 1.);

  // end
  return StatusCode::SUCCESS;
}
