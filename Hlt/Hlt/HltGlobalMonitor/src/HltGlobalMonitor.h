#ifndef HLTGLOBALMONITOR_H 
#define HLTGLOBALMONITOR_H 1

#include "Event/ODIN.h"
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"

#include "boost/function.hpp" 

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/StringKey.h"
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

class HltGlobalMonitor : public HltBaseAlg 
               , virtual public IIncidentListener {
public: 
  /// Standard constructor
  HltGlobalMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltGlobalMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  void handle(const Incident&);

private:
  void monitorODIN(const LHCb::ODIN*,const LHCb::L0DUReport*,const LHCb::HltDecReports*);
  void monitorL0DU(const LHCb::ODIN*,const LHCb::L0DUReport*,const LHCb::HltDecReports*);
  void monitorHLT1 (const LHCb::ODIN*,const LHCb::L0DUReport*,const LHCb::HltDecReports*);
  void monitorHLT2 (const LHCb::ODIN*,const LHCb::L0DUReport*,const LHCb::HltDecReports*);
  void monitorMemory();
  void storeTrend(AIDA::IProfile1D*, double);
  template <typename T> T* fetch(const std::string& location) {
       T* t =  this->exist<T>( location ) ?  this->get<T>( location ) 
                                          :  (T*)0;
       if (t == 0 && this->msgLevel(MSG::WARNING) ) Warning( " could not retrieve "  + location,StatusCode::SUCCESS,10) ;
       return t;
  };
  
  typedef std::map< std::string, std::vector<std::string> >  DecToGroupType;
  DecToGroupType m_DecToGroup1;
  DecToGroupType m_DecToGroup2;

  std::map<Gaudi::StringKey,std::pair<unsigned,unsigned> > m_hlt1Line2AlleyBin; // index in m_hlt1Alleys, m_hlt1AlleyRates , followed by binnumber inside alley
  std::map<Gaudi::StringKey,std::pair<unsigned,unsigned> > m_hlt2Line2AlleyBin;
  std::vector<AIDA::IHistogram1D*> m_hlt1Alleys;
  std::vector<AIDA::IHistogram1D*> m_hlt2Alleys;
  std::vector<StatEntity*>  m_hlt1AlleyRates;
  std::vector<StatEntity*>  m_hlt2AlleyRates;
  std::vector<std::string>  m_Hlt1Lines;
  std::vector<std::string>  m_Hlt2Lines;

  std::string m_ODINLocation;
  std::string m_L0DUReportLocation;
  std::string m_HltDecReportsLocation;

  AIDA::IHistogram1D* m_odin;
  AIDA::IHistogram1D* m_L0Input;
  AIDA::IHistogram1D* m_L0Hlt1Accept;
  AIDA::IHistogram1D* m_L0Hlt2Accept;
  AIDA::IHistogram1D* m_hlt1Alley;
  AIDA::IHistogram1D* m_hlt2Alley;

  AIDA::IProfile1D* m_hltVirtTime;
  AIDA::IHistogram1D* m_hltTime;
  AIDA::IProfile1D* m_hltEventsTime;

  AIDA::IHistogram2D* m_hlt1AlleysCorrelations;
  AIDA::IHistogram2D* m_hlt2AlleysCorrelations;

  double   m_currentTime; // seconds since start of clock
  double m_timeSize;
  double m_timeInterval;
  longlong m_startClock; // in microseconds
  longlong m_startEvent; // in microseconds
  long m_virtmem; 
  int m_gpstimesec;
  int m_time_ref;
  int m_scanevents;
  int m_totaltime;
  int m_totalmem;
  int m_events;

  unsigned int m_lastL0TCK;

};
#endif // HLTGLOBALMONITOR_H
