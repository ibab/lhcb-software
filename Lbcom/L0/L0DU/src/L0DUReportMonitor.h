// $Id: L0DUReportMonitor.h,v 1.3 2008-03-27 16:32:53 odescham Exp $
#ifndef L0DUREPORTMONITOR_H 
#define L0DUREPORTMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
// from Event
#include "Event/L0DUReport.h"


/** @class L0DUReportMonitor L0DUReportMonitor.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-10-25
 */
class L0DUReportMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  L0DUReportMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0DUReportMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  typedef std::map<std::string , int>                              SeqMap;
  typedef std::map<std::string , double>                           CounterMap;
  typedef std::map<unsigned int, std::map<std::string , double> >  CounterMapMap;
  
  SeqMap m_condSeq;
  SeqMap m_chanSeq;

  CounterMap m_chanCnt;
  CounterMap m_condCnt;
  CounterMap m_chanRate;
  CounterMap m_chanRelRate;
  CounterMap m_condRate;
  CounterMapMap m_chanCntMap;
  CounterMapMap m_condCntMap;
  CounterMapMap m_chanRateMap;
  CounterMapMap m_chanRelRateMap;
  CounterMapMap m_condRateMap;
  double m_evtCnt;
  double m_decCnt;
  std::map<unsigned int, double> m_evtCntMap;
  std::map<unsigned int, double> m_decCntMap;
  std::map<unsigned int, LHCb::L0DUConfig*> m_configs;
  int m_prevTCK;
  std::string m_reportLocation;
  bool m_histo;
  int m_bin;
  bool m_split;
};
#endif // L0DUREPORTMONITOR_H
