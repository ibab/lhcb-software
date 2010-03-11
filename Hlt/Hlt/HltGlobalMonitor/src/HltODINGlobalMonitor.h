#ifndef HLTODINGLOBALMONITOR_H 
#define HLTODINGLOBALMONITOR_H 1

#include "Event/ODIN.h"

#include "boost/function.hpp" 

#include "GaudiKernel/StringKey.h"
#include "HltBase/HltBaseAlg.h"

/** @class HltODINGlobalMonitor HltODINGlobalMonitor.h
 *
 *  functionality:
 *        monitor the global Hlt results
 *        make histograms with Hlt diagnostics info
 *
 *  @author Arthur Maciel
 *  @date   2003-03-06
 */

class HltODINGlobalMonitor : public HltBaseAlg {
public: 
  /// Standard constructor
  HltODINGlobalMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltODINGlobalMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution


private:

  void monitorODIN(const LHCb::ODIN*);
  template <typename T> T* fetch(const std::string& location) {
       T* t =  this->exist<T>( location ) ?  this->get<T>( location ) 
                                          :  (T*)0;
       if (t == 0 && this->msgLevel(MSG::WARNING) ) Warning( " could not retrieve "  + location,StatusCode::SUCCESS,10) ;
       return t;
  };
 std::string m_ODINLocation;
 AIDA::IHistogram1D* m_odin;


  int m_events;

};
#endif // HLTODINGLOBALMONITOR_H
