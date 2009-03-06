#ifndef HLTGLOBALMONITOR_H 
#define HLTGLOBALMONITOR_H 1

#include "Event/ODIN.h"
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"

#include "HltBase/HltBaseAlg.h"



/** @class HltGlobalMonitor HltGlobalMonitor.h
 *
 *  functionality:
 *        monitor the global Hlt results
 *        make histograms with Hlt diagnostics info
 *
 *  @author Arthur Maciel
 *  @date   2003-03-06
 */

class HltGlobalMonitor : public HltBaseAlg {
public: 
  /// Standard constructor
  HltGlobalMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltGlobalMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm execution

private:
  void monitorODIN(const LHCb::ODIN*,const LHCb::L0DUReport*,const LHCb::HltDecReports*);
  void monitorL0DU(const LHCb::ODIN*,const LHCb::L0DUReport*,const LHCb::HltDecReports*);
  void monitorHLT (const LHCb::ODIN*,const LHCb::L0DUReport*,const LHCb::HltDecReports*);
  void monitorMemory();
  

  std::string m_ODINLocation;
  std::string m_L0DUReportLocation;
  std::string m_HltDecReportsLocation;
  std::vector<std::string>  m_Hlt1Lines;
  
  int m_gpstimesec;
  longlong m_time;
  int m_time_ref;
  int m_scanevents;
  int m_totaltime;
  int m_totalmem;
  long m_virtmem; 

  template <typename T> T* fetch(const std::string& location) {
       T* t =  this->exist<T>( location ) ?  this->get<T>( location ) 
                                           :  (T*)0;
       if (t == 0 && this->msgLevel(MSG::WARNING) ) this->warning() << " could not retrieve " 
                                                                    << location << endmsg;
       return t;
    };


  AIDA::IHistogram1D* m_odin;
  AIDA::IHistogram1D* m_L0Input;
  AIDA::IHistogram1D* m_L0Hlt1Accept;
  AIDA::IHistogram1D* m_L0Hlt2Accept;
  std::vector<double> m_allAcc,m_allCall;
  AIDA::IHistogram1D* m_hltAcc;
  AIDA::IHistogram1D* m_hlt1alley;
  AIDA::IHistogram1D* m_hltNAcc;
  AIDA::IHistogram1D* m_hltInclusive;
  AIDA::IHistogram1D* m_hltExclusive;
  AIDA::IHistogram1D* m_hltVirtMem;
  AIDA::IHistogram1D* m_hltVirtinTime;
  AIDA::IHistogram1D* m_hltEventsTime;

  AIDA::IHistogram2D* m_hltCorrelations;
};
#endif // HLTGLOBALMONITOR_H
