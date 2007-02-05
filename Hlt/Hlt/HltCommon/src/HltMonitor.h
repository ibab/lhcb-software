// $Id: HltMonitor.h,v 1.4 2007-02-05 09:09:08 hernando Exp $
#ifndef HLTMONITOR_H 
#define HLTMONITOR_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "Event/HltNames.h"

//typedef HltAlgorithm HltAlgorithm

/** @class HltMonitor HltMonitor.h
 *  
 *  functionality (TODO):
 *         generic monitoring/histograms of info stores in tracks and vertices
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-09-01
 */
class HltMonitor : public HltAlgorithm {
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
