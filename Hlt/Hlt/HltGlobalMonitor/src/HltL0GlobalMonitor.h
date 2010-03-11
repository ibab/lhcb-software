#ifndef HLTL0GLOBALMONITOR_H 
#define HLTL0GLOBALMONITOR_H 1

#include "Event/L0DUReport.h"

#include "boost/function.hpp" 

#include "GaudiKernel/StringKey.h"
#include "HltBase/HltBaseAlg.h"

/** @class HltL0GlobalMonitor HltL0GlobalMonitor.h
 *
 *  functionality:
 *        monitor the global Hlt results
 *        make histograms with Hlt diagnostics info
 *
 *  @author Arthur Maciel
 *  @date   2003-03-06
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
       T* t =  this->exist<T>( location ) ?  this->get<T>( location ) 
                                          :  (T*)0;
       if (t == 0 && this->msgLevel(MSG::WARNING) ) Warning( " could not retrieve "  + location,StatusCode::SUCCESS,10) ;
       return t;
  };
  std::string m_L0DUReportLocation;

  AIDA::IHistogram1D* m_L0Input;

  int m_events;

  unsigned int m_lastL0TCK;

};
#endif // HLTL0GLOBALMONITOR_H
