#ifndef HLTL0GLOBALMONITOR_H 
#define HLTL0GLOBALMONITOR_H 1

#include "Event/L0DUReport.h"
#include "Event/ODIN.h"

#include "boost/function.hpp" 

#include "GaudiKernel/StringKey.h"
#include "HltBase/HltBaseAlg.h"

/** @class HltL0GlobalMonitor HltL0GlobalMonitor.h
 *
 *  functionality:
 *        monitor the L0 decisions seen by Hlt 
 *
 *
 *  @author Johannes Albrecht
 *  @date   2010-03-24 
 *
 *  original version from Arthur Maciel (2003)
 *
 */

class HltL0GlobalMonitor : public HltBaseAlg {
public: 
  /// Standard constructor
  HltL0GlobalMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltL0GlobalMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution


private:
  void monitorL0DU(const LHCb::L0DUReport*);
  template <typename T> T* fetch(const std::string& location) {
       T* t =  this->getIfExists<T>( location ) ;
       if (t == 0 && this->msgLevel(MSG::WARNING) ) Warning( " could not retrieve "  + location,StatusCode::SUCCESS,10) ;
       return t;
  }
  std::string m_ODINLocation;
  std::string m_L0DUReportLocation;
  std::string m_HltDecReportsLocation;
  std::string m_hlt1Decision;
  std::string m_hlt2Decision;
  

  AIDA::IHistogram1D* m_L0Input;
  AIDA::IHistogram1D* m_histL0Enabled;
  AIDA::IHistogram1D* m_histL0Disabled;
  AIDA::IHistogram1D* m_histL0EnabledHLT1;
  AIDA::IHistogram1D* m_histL0EnabledHLT2;

  int m_events;
  int m_nboflabels;

  unsigned int m_lastL0TCK;

};
#endif // HLTL0GLOBALMONITOR_H
