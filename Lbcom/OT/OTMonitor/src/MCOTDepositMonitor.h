// $Id: MCOTDepositMonitor.h,v 1.6 2007-02-26 15:46:08 cattanem Exp $
#ifndef OTMONITOR_MCOTDEPOSITMONITOR_H
#define OTMONITOR_MCOTDEPOSITMONITOR_H 1

/// STL
#include <vector>

/// Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

/** @class MCOTDepositMonitor MCOTDepositMonitor.h
 *
 *  Monitoring algorithm for the MCOTDeposits of the Outer Tracker.
 *
 *  @author M.Needham
 *  @date   21/4/2001
 *  @author adapted by J.Nardulli
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

class MCOTDepositMonitor : public GaudiHistoAlg {

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
  int m_firstStation;    ///< first OT station   (from geometry)

  /// histograms
  AIDA::IHistogram1D* m_nDepositsHisto;       ///< Deposits distribution
  AIDA::IHistogram1D* m_nHitsPerStationHisto; ///< Hits per station 
  AIDA::IHistogram1D* m_nHitsPerLayerHisto;   ///< Hits per layer
  /// drift time histo for each station
  std::vector<AIDA::IHistogram1D*> m_driftTimeHistos;
  /// y vs x of hits for each station 
  std::vector<AIDA::IHistogram2D*> m_yvsxHistos;
  AIDA::IHistogram1D* m_driftDistHisto;  ///< drift distance distribution
  AIDA::IHistogram1D* m_nCrossTalkHisto; ///< cross talk level
  
};

#endif // OTMONITOR_MCOTDEPOSITMONITOR_H
