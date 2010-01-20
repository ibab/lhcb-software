// $Id: L0DUReportMonitor.h,v 1.7 2010-01-20 16:30:58 odescham Exp $
#ifndef L0DUREPORTMONITOR_H 
#define L0DUREPORTMONITOR_H 1

// Include files
// from Gaudi
#include "L0Base/L0AlgBase.h"
// from Event
#include "Event/L0DUReport.h"
#include <TH1D.h>
#include <TH2D.h>
#include <TAxis.h>

/** @class L0DUReportMonitor L0DUReportMonitor.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-10-25
 */
class L0DUReportMonitor : public L0AlgBase {
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
  CounterMap m_condRate;
  CounterMap m_trigCnt;
  CounterMap m_trigRate;

  CounterMapMap m_chanCntMap;
  CounterMapMap m_condCntMap;
  CounterMapMap m_chanRateMap;
  CounterMapMap m_condRateMap;
  CounterMapMap m_trigCntMap;
  CounterMapMap m_trigRateMap;


  std::map<int , CounterMap > m_chanRelRate;
  std::map<int , CounterMap > m_trigRelRate;
  std::map<int , CounterMapMap > m_trigRelRateMap;
  std::map<int , CounterMapMap > m_chanRelRateMap;
  std::map<int , double > m_decCnt;
  std::map<int ,std::map<unsigned int, double> > m_decCntMap;
  std::map<unsigned int, double>  m_evtCntMap;
  double m_evtCnt;

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
  std::string m_lab;
  void label( AIDA::IHistogram1D* h1d , std::map<int,std::string>  labels,bool count=true,std::string opt="");
  void label( AIDA::IHistogram2D* h2d , std::map<int,std::string>  labels, int mask=0x3,bool count=true,std::string opt="");
  bool m_generic;
  bool m_swap;
};
#endif // L0DUREPORTMONITOR_H
