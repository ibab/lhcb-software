// $Id: OTTimeMonitor.h,v 1.5 2006-04-11 19:23:16 janos Exp $
#ifndef OTMONITOR_OTTIMEMONITOR_H
#define OTMONITOR_OTTIMEMONITOR_H 1

/// STL
#include <vector>

/// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

/** @class OTTimeMonitor OTTimeMonitor.h "OTMonitor/OTTimeMonitor.h"
 *
 *  Algorithm for monitoring OTTimes for the Outer Tracker.
 *
 *  @author J. van Tilburg & Jacopo Nardulli
 *  @date   20-07-2004
 */

class DeOTDetector;

namespace AIDA 
{
  class IHistogram1D;
}

namespace LHCb 
{
  class OTTime;
}

class OTTimeMonitor : public GaudiHistoAlg {

 public:
  
  /// constructor
  OTTimeMonitor( const std::string& name, ISvcLocator *svcloc );

  /// destructor
  virtual ~OTTimeMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

 private:

  /// book the histograms
  StatusCode initHistograms();

  /// fill the histograms
  StatusCode fillHistograms(LHCb::OTTime* aTime);
 
  /// Geometry
  DeOTDetector* m_tracker;
  unsigned int m_nStations;    ///< number of stations (from geometry)
  unsigned int m_firstStation; ///< first OT station   (from geometry)
    
  /// top module number for every layer
  std::vector<unsigned int> m_nTopModules;
  /// number of channels per station 
  std::vector<unsigned int> m_nChannelsPerStation; 
  /// times per layer distribution 
  std::vector<unsigned int> m_nChannelsPerLayer;
  ///< number of channels in top modules
  std::vector<unsigned int> m_nChannelsTopModule;
  ///< number of channels in corner modules
  std::vector<unsigned int> m_nChannelsCornerModule;
  ///< number of channels in side modules
  std::vector<unsigned int> m_nChannelsSideModule;
    
  /// histograms
  ///< times per event distribution
  AIDA::IHistogram1D* m_nTimesHisto;           
  ///< times per station distribution 
  AIDA::IHistogram1D* m_nTimesPerStationHisto;
  ///< times per layer distribution
  AIDA::IHistogram1D* m_nTimesPerLayerHisto;
  ///< occupancy per station
  AIDA::IHistogram1D* m_occPerStationHisto;
  ///< occupancy per layer
  AIDA::IHistogram1D* m_occPerLayerHisto;      
  /// occupancy in top module per layer distribution
  AIDA::IHistogram1D* m_occTopPerLayerHisto;
  /// occupancy in corner module per layer distribution
  AIDA::IHistogram1D* m_occCornerPerLayerHisto;
  /// occupancy in side module per layer distribution
  AIDA::IHistogram1D* m_occSidePerLayerHisto;
  /// occupancy in T1 layer 1 versus x coordinate 
  AIDA::IHistogram1D* m_occVsxHisto;  
  /// tdc time distribution for each station
  std::vector<AIDA::IHistogram1D*> m_tdcTimeHistos;
  /// calibrated time distribution for each station
  std::vector<AIDA::IHistogram1D*> m_calTimeHistos;

};

#endif // OTMONITOR_OTTIMEMONITOR_H
