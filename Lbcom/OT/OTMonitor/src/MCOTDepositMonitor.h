// $Id: MCOTDepositMonitor.h,v 1.1 2004-09-10 13:13:49 cattanem Exp $
#ifndef OTMONITOR_MCOTDEPOSITMONITOR_H
#define OTMONITOR_MCOTDEPOSITMONITOR_H 1

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// Event
#include "Event/MCOTDeposit.h"

// local
#include "OTMonitorAlgorithm.h"

class MCOTDeposit;


/** @class MCOTDepositMonitor MCOTDepositMonitor.h \ 
 *         "OTMonitor/MCOTDepositMonitor.h"
 *
 *  Monitoring algorithm for the MCOTDeposits of the Outer Tracker.
 *
 *  @author M.Needham
 *  @date   21/4/2001
 *  @Adapted J.Nardulli
 *  @date   20/07/2004
 */

class MCOTDepositMonitor : public OTMonitorAlgorithm {

public:
 
  /// constructor
  MCOTDepositMonitor(const std::string& name, 
                     ISvcLocator *svcloc );

  /// destructor
  virtual ~MCOTDepositMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

  /// finalize
  StatusCode finalize();

private:

  /// book the histograms
  StatusCode initHistograms();

  /// fill the histograms
  StatusCode fillHistograms(MCOTDeposit* aDeposit);

  int m_nCrossTalkHits;  ///< count the number of XTalk hits
  int m_numStations;     ///< number of stations (from geometry)
  int m_firstOTStation;  ///< first OT station   (from geometry)

  /// histograms
  IHistogram1D* m_nDepositsHisto;       ///< Deposits distribution
  IHistogram1D* m_nHitsPerStationHisto; ///< Hits per station 
  IHistogram1D* m_nHitsPerLayerHisto;   ///< Hits per layer
  /// drift time histo for each station
  std::vector<IHistogram1D*> m_driftTimeHistos;
  /// x vs y of hits for each station 
  std::vector<IHistogram2D*> m_xvsyHistos;
  IHistogram1D* m_driftDistHisto;  ///< drift distance distribution
  IHistogram1D* m_nCrossTalkHisto; ///< cross talk level
  
};

#endif // OTMONITOR_MCOTDEPOSITMONITOR_H
