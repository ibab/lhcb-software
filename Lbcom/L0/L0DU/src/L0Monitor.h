// $Id: L0Monitor.h,v 1.6 2006-04-12 12:45:40 ocallot Exp $
#ifndef L0MONITOR_H 
#define L0MONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class L0Monitor L0Monitor.h
 *  Re-implementation of a simple monitor for L0 decision
 *
 *  @author Olivier Callot
 *  @date   2006-02-16
 */
class L0Monitor : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0Monitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0Monitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  double m_totEvt;
  double m_goodL0;

  double m_trigElec;
  double m_trigPhot;
  double m_trigHadr;
  double m_trigPi0L;
  double m_trigPi0G;
  double m_trigPi0 ;
  double m_trigMuon;
  double m_trigSumMu;

  double m_exclElec;
  double m_exclPhot;
  double m_exclHadr;
  double m_exclPi0L;
  double m_exclPi0G;
  double m_exclPi0 ;
  double m_exclMuon;
  double m_exclSumMu;

  double m_minPeak2Veto;
};
#endif // L0MONITOR_H
