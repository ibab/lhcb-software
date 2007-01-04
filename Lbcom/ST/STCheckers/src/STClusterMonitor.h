// $Id: STClusterMonitor.h,v 1.1 2007-01-04 11:08:51 jvantilb Exp $
#ifndef STClusterMonitor_H
#define STClusterMonitor_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
  class STCluster;
};
class ISTSignalToNoiseTool;
class DeSTDetector;


/** @class STClusterMonitor STClusterMonitor.h
 *
 *  Class for monitoring STClusters
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class STClusterMonitor : public GaudiHistoAlg {

public:
 
  /// constructer
  STClusterMonitor( const std::string& name, 
                    ISvcLocator *svcloc );

  /// destructer
  virtual ~STClusterMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  virtual StatusCode fillHistograms(const LHCb::STCluster* aCluster);

  // histograms
  DeSTDetector* m_tracker;
  
  std::string m_sigNoiseToolName;
  ISTSignalToNoiseTool* m_sigNoiseTool;

  std::string m_clusterLocation;
  std::string m_detType;
  
};

#endif // MCSTClusterMonitor_H
