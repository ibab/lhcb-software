// $Id: OTTimeMonitor.h,v 1.9 2009-11-25 15:41:12 cattanem Exp $
#ifndef OTMONITOR_OTTIMEMONITOR_H
#define OTMONITOR_OTTIMEMONITOR_H 1

/// STL
#include <vector>

/// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"

#include "OTDAQ/IOTRawBankDecoder.h"


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
  class IProfile1D ;
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

  /// finalize
  StatusCode finalize();
  
 private:

  /// book the histograms
  void initHistograms();

  /// fill the histograms
  void fillHistograms(const LHCb::OTLiteTime& aTime,const DeOTModule& mod);
 
private:

  // Geometry
  ToolHandle<IOTRawBankDecoder> m_decoder;
  DeOTDetector* m_tracker;
  unsigned int m_nStations;    ///< number of stations (from geometry)
  unsigned int m_firstStation; ///< first OT station   (from geometry)
  unsigned int m_nEvents;      ///< Number of events
    
  // histograms
  /// times per event distribution
  AIDA::IHistogram1D* m_nTimesHisto;           
  /// occupancy per module
  AIDA::IHistogram1D* m_occPerModuleHisto ;
  /// average drifttime per module
  AIDA::IProfile1D* m_calTimePerModulePr ;
  /// tdc time distribution for each station
  std::vector<AIDA::IHistogram1D*> m_tdcTimeHistos;
  /// calibrated time distribution for each station
  std::vector<AIDA::IHistogram1D*> m_calTimeHistos;
};

#endif // OTMONITOR_OTTIMEMONITOR_H
