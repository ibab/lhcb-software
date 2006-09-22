// $Id: HltMonitor.h,v 1.1 2006-09-22 11:36:09 hernando Exp $
#ifndef HLTMONITOR_H 
#define HLTMONITOR_H 1

// Include files
// from Gaudi
#include "HltCommon/HltAlgorithm.h"
#include "HltCommon/ParserDescriptor.h"
#include "Event/HltNames.h"

/** @class HltMonitor HltMonitor.h
 *  
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

  template <class CON>
  void monitor(CON*& con) {
    if (!con) return;
    for (std::vector<std::string>::iterator it = m_titles.begin();
         it != m_titles.end(); ++it) {
      const std::string& title = (*it);
      int key = LHCb::HltNames::particleInfoID(title);
      if (key <0) continue;
      for (typename CON::iterator it2 = con->begin(); 
           it2 != con->end(); it2++) {
        if ((*it2)->hasInfo(key)) 
          fill( histo1D(title), (*it2)->info(key,0), 1.);
      }
    } 
  }
  
protected:
  
  StringArrayProperty m_histoDescriptor;
  std::vector< std::string > m_titles;

};
#endif // HLTMONITOR_H
