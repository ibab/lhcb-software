// $Id: MCOTDepositMonitor.h,v 1.4 2006-03-30 21:51:27 janos Exp $
#ifndef OTMONITOR_MCOTDEPOSITMONITOR_H
#define OTMONITOR_MCOTDEPOSITMONITOR_H 1

// local
#include "OTMonitorAlgorithm.h"

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

namespace AIDA 
{
  class IHistogram1D;
  class IHistogram2D;
}

namespace LHCb
{
  class MCOTDeposit;
}

class MCOTDepositMonitor : public OTMonitorAlgorithm {

public:
 
  /// constructor
  MCOTDepositMonitor( const std::string& name, 
		      ISvcLocator *svcloc );

  /// destructor
  virtual ~MCOTDepositMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  /// book the histograms
  StatusCode initHistograms();

  /// fill the histograms
  StatusCode fillHistograms( LHCb::MCOTDeposit* aDeposit );

  int m_nCrossTalkHits;  ///< count the number of XTalk hits
  int m_nStations;       ///< number of stations (from geometry)
  int m_firstStation;  ///< first OT station   (from geometry)

  /// histograms
  AIDA::IHistogram1D* m_nDepositsHisto;       ///< Deposits distribution
  AIDA::IHistogram1D* m_nHitsPerStationHisto; ///< Hits per station 
  AIDA::IHistogram1D* m_nHitsPerLayerHisto;   ///< Hits per layer
  /// drift time histo for each station
  std::vector<AIDA::IHistogram1D*> m_driftTimeHistos;
  /// x vs y of hits for each station 
  std::vector<AIDA::IHistogram2D*> m_xvsyHistos;
  AIDA::IHistogram1D* m_driftDistHisto;  ///< drift distance distribution
  AIDA::IHistogram1D* m_nCrossTalkHisto; ///< cross talk level
  
};

#endif // OTMONITOR_MCOTDEPOSITMONITOR_H
