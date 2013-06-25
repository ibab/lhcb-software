#ifndef FTCLUSTERMONITOR_H 
#define FTCLUSTERMONITOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"


/** @class FTClusterMonitor FTClusterMonitor.h
 *  
 *
 *  @author Eric Cogneras
 *  @date   2012-07-05
 */
class FTClusterMonitor : public GaudiHistoAlg {
public: 
  /// Standard constructor
  FTClusterMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FTClusterMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  std::string m_clusterLocation;      ///< FTCluster Container
};
#endif // FTCLUSTERMONITOR_H
