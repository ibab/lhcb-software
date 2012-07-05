#ifndef MCFTDEPOSITMONITOR_H 
#define MCFTDEPOSITMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// LHCbKernel
#include "Kernel/FTChannelID.h"

/** @class MCFTDepositMonitor MCFTDepositMonitor.h
 *  
 *
 *  @author Eric Cogneras
 *  @date   2012-07-04
 */
class MCFTDepositMonitor : public GaudiHistoAlg {

  typedef std::pair<LHCb::FTChannelID, double> FTDoublePair;
  typedef std::vector< FTDoublePair > FTDoublePairs;

public: 
  /// Standard constructor
  MCFTDepositMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCFTDepositMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_hitLocation;  ///< FT MCHits Location
  std::string m_depositLocation; ///< FT energy deposit Location

  DeFTDetector* m_deFT; ///< pointer to FT detector description
};
#endif // MCFTDEPOSITMONITOR_H
