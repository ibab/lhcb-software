// $Id: L0DUReportMonitor.h,v 1.5 2008-05-29 14:01:15 odescham Exp $
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
  SeqMap m_trigSeq;

  CounterMap m_chanCnt;
  CounterMap m_condCnt;
  CounterMap m_chanRate;
  CounterMap m_chanRelRate;
  CounterMap m_condRate;
  CounterMap m_trigCnt;
  CounterMap m_trigRate;
  CounterMap m_trigRelRate;
  CounterMapMap m_chanCntMap;
  CounterMapMap m_condCntMap;
  CounterMapMap m_chanRateMap;
  CounterMapMap m_chanRelRateMap;
  CounterMapMap m_condRateMap;
  CounterMapMap m_trigCntMap;
  CounterMapMap m_trigRateMap;
  CounterMapMap m_trigRelRateMap;  

  double m_evtCnt;
  double m_decCnt;
  std::map<unsigned int, double> m_evtCntMap;
  std::map<unsigned int, double> m_decCntMap;
  std::map<unsigned int, LHCb::L0DUConfig*> m_configs;
  int m_prevTCK;
  std::string m_reportLocation;
  bool m_data;
  bool m_cond;
  bool m_chan;
  bool m_trig;
  bool m_dec;
  bool m_full;
  int m_bin;
  bool m_split;
};
#endif // L0DUREPORTMONITOR_H
