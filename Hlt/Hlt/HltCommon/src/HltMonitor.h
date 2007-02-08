// $Id: HltMonitor.h,v 1.5 2007-02-08 17:38:06 hernando Exp $
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

protected:

  std::vector< std::string > m_keys;

};
#endif // HLTMONITOR_H
