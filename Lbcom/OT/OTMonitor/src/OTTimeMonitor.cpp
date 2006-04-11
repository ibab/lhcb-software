// $Id: OTTimeMonitor.cpp,v 1.7 2006-04-11 19:23:16 janos Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// AIDA
#include "AIDA/IHistogram1D.h"

// OTDet
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// Event
#include "Event/OTTime.h"

// MathCore
#include "Kernel/SystemOfUnits.h"

/// BOOST
#include "boost/lexical_cast.hpp"

// local
#include "OTTimeMonitor.h"

/** @file OTTimeMonitor.cpp 
 *
 *  Implementation of OTTimeMonitor algorithm.
 *  @author J. van Tilburg & Jacopo Nardulli
 *  @date   20-07-2004
 */

using namespace LHCb;

/// Declaration of algorithm factory
DECLARE_ALGORITHM_FACTORY( OTTimeMonitor );

OTTimeMonitor::OTTimeMonitor(const std::string& name, 
			     ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator) {
  // constructor
}

OTTimeMonitor::~OTTimeMonitor() {
  // destructor
}

StatusCode OTTimeMonitor::initialize() {

  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) {
    return Error("Failed to initialize", sc);
  }

  if("" == histoTopDir()) setHistoTopDir("OT/");

  // Get OT Geometry from XML
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  m_nStations = m_tracker->nStation();
  m_firstStation = m_tracker->firstStation();

  // intialize histos
  this->initHistograms();

  // Store number of channels (per station, layer etc)
  m_nChannelsTopModule.resize(40, 0);
  m_nChannelsCornerModule.resize(40, 0);
  m_nChannelsSideModule.resize(40, 0);
  m_nChannelsPerLayer.resize(40, 0);
  m_nChannelsPerStation.resize(4, 0);
  std::vector<DeOTModule*> modules = m_tracker->modules();
  std::vector<DeOTModule*>::const_iterator iM;
  for (iM = modules.begin(); iM != modules.end(); ++iM) {
    int iStation = (*iM)->elementID().station();
    int iLayer = (*iM)->elementID().layer();
    int iUniqueLayerNum = 10*iStation+iLayer;
    int nChannels = (*iM)->nChannels();
    int moduleID = (*iM)->elementID().module();
    if ( moduleID == 9 ) m_nChannelsTopModule[iUniqueLayerNum] += nChannels;
    if ( moduleID == 8 ) m_nChannelsCornerModule[iUniqueLayerNum] += nChannels;
    if ( moduleID == 7 ) m_nChannelsSideModule[iUniqueLayerNum] += nChannels;
    m_nChannelsPerLayer[iUniqueLayerNum] += nChannels;
    m_nChannelsPerStation[iStation] += nChannels;
  }

  return StatusCode::SUCCESS;
}

StatusCode OTTimeMonitor::execute() {
  
  // retrieve OTTimes
  OTTimes* times = get<OTTimes>( OTTimeLocation::Default );
 
  // number of times
  m_nTimesHisto->fill(double(times->size()), 1.0);

  // histos per time
  OTTimes::const_iterator iterTime = times->begin();
  for( ; iterTime != times->end(); iterTime++){
    this->fillHistograms(*iterTime);
  } // loop iterDep

  
  return StatusCode::SUCCESS;
}
  
StatusCode OTTimeMonitor::initHistograms() {
 
  // Intialize histograms
  // number of Times in container
  m_nTimesHisto = book(1, "nTimes", 0., 20000., 200);
  // number of Times per layer 
  m_nTimesPerLayerHisto = book(12, "nTimes per layer", 9.5, 34.5, 25);
  // occupancy per layer
  m_occPerLayerHisto = book(22, "Occupancy per layer", 9.5, 34.5, 25);
  
  // histograms per station
  int id;
  IHistogram1D* aHisto1D;
  
  for (unsigned int iStation = m_firstStation; 
       iStation <= m_nStations; ++iStation) {
    
    std::string stationToString = boost::lexical_cast<std::string>(iStation);
    
    // Calibrated time spectra
    id = 300 + iStation;
    aHisto1D = book(id, "Calibrated time station "+stationToString,
		    -50.0*ns, 200.0*ns, 250);
    m_calTimeHistos.push_back(aHisto1D);
    
  } // loop station
  
  if ( fullDetail() ) {
    // number of Times per station
    m_nTimesPerStationHisto = book(11, "nTimes per stat", 0.5, 3.5, 3);
    // occupancy per station
    m_occPerStationHisto = book(21, "Occupancy per station", 0.5, 3.5, 3);
    // occupancy in top module per layer 
    m_occTopPerLayerHisto = book(23, "Occupancy in top module per layer", 
				 9.5, 34.5, 25);
    // number of Times in corner module per layer 
    m_occCornerPerLayerHisto = book(24, "Occupancy in corner module per layer",
				    9.5, 34.5, 25);
    // occupancy in side module per layer 
    m_occSidePerLayerHisto = book(25, "Occupancy in side module per layer",
				  9.5, 34.5, 25);
    // occupancy versus x coordinate in T1 layer 1  
    m_occVsxHisto = book(31, "Occupancy in T1 layer 1 vs x", 
			 0.0, 2984.625, 50);
    
    // histograms per station
    int id;
    IHistogram1D* aHisto1D;
    
    for (unsigned int iStation = m_firstStation;
	 iStation <= m_nStations; ++iStation) {
      
      std::string stationToString = boost::lexical_cast<std::string>(iStation);
      
      // Tdc time spectra
      id = 100 + iStation;
      aHisto1D = book(id, "Bin - TDC time station "+stationToString,
		      -50.0*ns, 200.0*ns, 250);
      m_tdcTimeHistos.push_back(aHisto1D);
    }
  }
  
  return StatusCode::SUCCESS;
}


StatusCode OTTimeMonitor::fillHistograms(OTTime* aTime) {
  
  // Times and occupancy per station
  OTChannelID channel = aTime->channel();
  const int iStation = channel.station();
  
  // times and occupancies per layer (for all, top, corner and side modules)
  const int iLayer = channel.layer();
  int iUniqueLayerNum = 10*iStation + iLayer;
  double weight = 100./(m_nChannelsPerLayer[iUniqueLayerNum]);
  m_nTimesPerLayerHisto->fill(double(iUniqueLayerNum), 1.);
  m_occPerLayerHisto->fill(double(iUniqueLayerNum), weight);

  if ( fullDetail() ) {

    weight = 100./(m_nChannelsPerStation[iStation]);
    m_nTimesPerStationHisto->fill(double(iStation), 1.0);
    m_occPerStationHisto->fill(double(iStation), weight);
    unsigned int iModule = channel.module();
    if ( iModule == 9 ) {
      weight = 100./(m_nChannelsTopModule[iUniqueLayerNum] );
      m_occTopPerLayerHisto->fill(double(iUniqueLayerNum), weight);
    }
    if (iModule == 8 ) {
      weight = 100./(m_nChannelsCornerModule[iUniqueLayerNum] );    
      m_occCornerPerLayerHisto->fill(double(iUniqueLayerNum), weight);
    }
    if ( iModule == 7 ) {
      weight = 100./(m_nChannelsSideModule[iUniqueLayerNum] );
      m_occSidePerLayerHisto->fill(double(iUniqueLayerNum), weight);
    }

    // number of times versus x-coordinate for T1 layer 1.
    if (iStation == 3 && iLayer == 1) {
      DeOTModule* module = m_tracker->findModule(channel);
      double channelsPerBin = m_nChannelsPerLayer[iUniqueLayerNum]/50.;
      weight = 100. / (channelsPerBin );
      Gaudi::XYZPoint hit = module->centerOfStraw(channel.straw());
      m_occVsxHisto->fill(fabs(hit.x()), weight);
    }
  
    // Bin time for every station
    int tdcTime = channel.tdcTime();
    double tdc = double(tdcTime);
    m_tdcTimeHistos[iStation-m_firstStation]->fill( tdc, 1.);
  }
  
  // calibrated time for every station
  m_calTimeHistos[iStation-m_firstStation]->fill(aTime->calibratedTime(), 1.);

  // end
  return StatusCode::SUCCESS;
}
