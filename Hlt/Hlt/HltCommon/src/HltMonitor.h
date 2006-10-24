// $Id: HltMonitor.h,v 1.3 2006-10-24 09:44:03 hernando Exp $
#ifndef HLTMONITOR_H 
#define HLTMONITOR_H 1

// Include files
// from Gaudi
#include "HltBase/HltMonitorAlgorithm.h"
#include "Event/HltNames.h"

/** @class HltMonitor HltMonitor.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-09-01
 */
class HltMonitor : public HltMonitorAlgorithm {
public: 
  /// Standard constructor
  HltMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  template <class CONTAINER>
  void monitor(CONTAINER& con, std::vector<std::string> keys) {
    if (con.size() <=0) return;
    for (std::vector<std::string>::iterator it = keys.begin();
         it != keys.end(); ++it) {
      const std::string& title = (*it);
      int key = HltNames::particleInfoID(title);
      if (key <0) continue;
      for (typename CONTAINER::iterator it2 = con.begin(); 
           it2 != con.end(); it2++) {
        if ((*it2)->hasInfo(key)) 
          fill( histo1D(title), (*it2)->info(key,0), 1.);
      }
    } 
  }
  
protected:

  std::vector< std::string > m_keys;

};
#endif // HLTMONITOR_H
