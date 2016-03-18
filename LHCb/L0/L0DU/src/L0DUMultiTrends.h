// $Id: L0DUMultiTrends.h,v 1.3 2010-02-02 11:27:02 odescham Exp $
#ifndef L0DUMULTITRENDS_H 
#define L0DUMULTITRENDS_H 1

// Include files
// from STL
#include <string>
// from Root
#include <TH1D.h>
#include <TAxis.h>
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
// Interface
#include "L0Interfaces/IL0DUConfigProvider.h"
#include "L0Interfaces/IL0DUEmulatorTool.h"
#include "L0Interfaces/IL0DUFromRawTool.h"

/** @class L0DUMultiTrends L0DUMultiTrends.h
 *  L0 Processors (L0Calo/L0Muon/PUVETO) -> L0DU 
 *  bit assignment from EDMS 528259
 *
 *  @author Olivier Deschamps
 *  @date   2005-01-28
 */
class L0DUMultiTrends : public GaudiTupleAlg{
public:
  /// Standard constructor
  L0DUMultiTrends( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0DUMultiTrends( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  int tck2int(std::string otck);
  std::string tck2string(int itck);
  // manipulate histo
  void doRates();
  void slideHistos(int nStep);
  void booking(std::string pref="");
  void setLabels();  

  // booking/filling TCK comparaison plots
  std::map<std::string,AIDA::IHistogram1D*> m_cMap;
  void cBook(std::string unit , std::string title);
  void cFill(std::string unit , int bin);
  void cFillSubTriggers();
  void cFillChannels();
  void cBookSubTriggers(LHCb::L0DUConfig* config,std::string pref="");
  void cBookChannels(LHCb::L0DUConfig* config,std::string pref="");
  void tBookSubTriggers(std::string pref="");
  void tBookChannels(std::string pref="");

  // booking/filling trending plots
  std::map<std::string,AIDA::IHistogram1D*> m_tMap;
  void tBook(std::string unit , std::string title);
  void tFill(std::string unit , int bin);

 
  // Tools
  IL0DUConfigProvider* m_config;
  IL0DUEmulatorTool*   m_emulator;
  IL0DUFromRawTool*    m_fromRaw;
  LHCb::L0ProcessorDatas* m_datas;
  IEventTimeDecoder* m_odin;
  //
  std::string m_emulatorTool;
  std::string m_fromRawTool;
  std::string m_configTool;
  std::vector<std::string> m_list;
  bool m_book;
  bool m_hasOrigin;
  int m_bin;
  int m_nPredTriggers;
  bool m_tTrend ;
  // trending
  long m_trendStep;
  int m_trendPeriod;
  int m_dMon;
  bool m_sMon;
  bool m_cMon;
  bool m_rate;
  //
  unsigned long m_origin;
  unsigned long m_oBin;
  unsigned long m_count;
};
#endif // L0DUMULTITRENDS_H
