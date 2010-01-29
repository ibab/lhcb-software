#ifndef HLTGLOBALMONITOR_H 
#define HLTGLOBALMONITOR_H 1

#include "Event/ODIN.h"
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"

#include "GaudiKernel/IIncidentListener.h"
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
  

  std::string m_ODINLocation;
  std::string m_L0DUReportLocation;
  std::string m_HltDecReportsLocation;
  std::vector<std::string>  m_Hlt1Lines;
  std::vector<std::string>  m_Hlt2Lines;

  std::vector<std::string>  m_Hlt1L0;
  std::vector<std::string>  m_Hlt1Lumi;
  std::vector<std::string>  m_Hlt1Velo;
  std::vector<std::string>  m_Hlt1XPress;
  std::vector<std::string>  m_Hlt1Hadron;
  std::vector<std::string>  m_Hlt1SingleMuon;
  std::vector<std::string>  m_Hlt1DiMuon;
  std::vector<std::string>  m_Hlt1Electron;
  std::vector<std::string>  m_Hlt1Photon;
  std::vector<std::string>  m_Hlt1PA;
  std::vector<std::string>  m_Hlt1BeamGas;
  std::vector<std::string>  m_Hlt1MinBias;
  std::vector<std::string>  m_Hlt1Other;

  std::vector<std::string>  m_Hlt2Topological;             
  std::vector<std::string>  m_Hlt2InclusivePhi;
  std::vector<std::string>  m_Hlt2SingleMuon;
  std::vector<std::string>  m_Hlt2DiMuon;
  std::vector<std::string>  m_Hlt2B2DX;
  std::vector<std::string>  m_Hlt2B2XGamma;
  std::vector<std::string>  m_Hlt2B2JpsiX;
  std::vector<std::string>  m_Hlt2B2XPhi;
  std::vector<std::string>  m_Hlt2B2HH;
  std::vector<std::string>  m_Hlt2Express;
  std::vector<std::string>  m_Hlt2Commissioning;
  std::vector<std::string>  m_Hlt2DisplVertices;
  std::vector<std::string>  m_Hlt2Other;

  typedef std::map< std::string, int >  m_DecToGroupType;
  m_DecToGroupType m_DecToGroup;
  m_DecToGroupType m_DecToGroup2;
  typedef std::vector< std::string >  m_GroupLabelsType;
  m_GroupLabelsType m_GroupLabels;
  m_GroupLabelsType m_GroupLabels2;
  
  longlong m_startClock; // in microseconds
  longlong m_startEvent; // in microseconds
  double   m_currentTime; // seconds since start of clock
  int m_gpstimesec;
  int m_time_ref;
  int m_scanevents;
  int m_totaltime;
  int m_totalmem;
  double m_timeSize;
  double m_timeInterval;
  long m_virtmem; 
  int m_events;

  template <typename T> T* fetch(const std::string& location) {
       T* t =  this->exist<T>( location ) ?  this->get<T>( location ) 
                                          :  (T*)0;
       if (t == 0 && this->msgLevel(MSG::WARNING) ) Warning( " could not retrieve "  + location,StatusCode::SUCCESS,10) ;
       return t;
  };


  unsigned int m_lastL0TCK;
  AIDA::IHistogram1D* m_odin;
  AIDA::IHistogram1D* m_L0Input;
  AIDA::IHistogram1D* m_L0Hlt1Accept;
  AIDA::IHistogram1D* m_L0Hlt2Accept;
  std::vector<StatEntity*> m_alley;
  AIDA::IHistogram1D* m_hlt1alley;
  AIDA::IHistogram1D* m_hlt2alley;
  AIDA::IHistogram1D* m_hlt1NAcc;
  AIDA::IHistogram1D* m_hlt2NAcc;
  AIDA::IHistogram1D* m_hlt1Inclusive;
  AIDA::IHistogram1D* m_hlt2Inclusive;
  AIDA::IHistogram1D* m_hlt1Exclusive;
  AIDA::IHistogram1D* m_hlt2Exclusive;
  AIDA::IProfile1D* m_hltVirtTime;
  AIDA::IHistogram1D* m_hltTime;
  AIDA::IProfile1D* m_hltEventsTime;

  AIDA::IHistogram2D* m_hlt1Correlations;
  AIDA::IHistogram2D* m_hlt2Correlations;
  AIDA::IHistogram2D* m_hlt1AlleysCorrelations;
  AIDA::IHistogram2D* m_hlt2AlleysCorrelations;

  AIDA::IHistogram1D* m_hlt1L0Alley;
  AIDA::IHistogram1D* m_hlt1LumiAlley;
  AIDA::IHistogram1D* m_hlt1VeloAlley;
  AIDA::IHistogram1D* m_hlt1XPressAlley;
  AIDA::IHistogram1D* m_hlt1HadronAlley;
  AIDA::IHistogram1D* m_hlt1SingleMuonAlley;
  AIDA::IHistogram1D* m_hlt1DiMuonAlley;
  AIDA::IHistogram1D* m_hlt1ElectronAlley;
  AIDA::IHistogram1D* m_hlt1PhotonAlley;
  AIDA::IHistogram1D* m_hlt1PAAlley;
  AIDA::IHistogram1D* m_hlt1BeamGasAlley;
  AIDA::IHistogram1D* m_hlt1MinBiasAlley;
  AIDA::IHistogram1D* m_hlt1OtherAlley;

  AIDA::IHistogram1D* m_hlt2TopologicalAlley;
  AIDA::IHistogram1D* m_hlt2InclusivePhiAlley;
  AIDA::IHistogram1D* m_hlt2SingleMuonAlley;
  AIDA::IHistogram1D* m_hlt2DiMuonAlley;
  AIDA::IHistogram1D* m_hlt2B2DXAlley;
  AIDA::IHistogram1D* m_hlt2B2XGammaAlley;
  AIDA::IHistogram1D* m_hlt2B2JpsiXAlley;
  AIDA::IHistogram1D* m_hlt2B2XPhiAlley;
  AIDA::IHistogram1D* m_hlt2B2HHAlley;
  AIDA::IHistogram1D* m_hlt2ExpressAlley;
  AIDA::IHistogram1D* m_hlt2CommissioningAlley;
  AIDA::IHistogram1D* m_hlt2DisplVerticesAlley;
  AIDA::IHistogram1D* m_hlt2OtherAlley;

};
#endif // HLTGLOBALMONITOR_H
