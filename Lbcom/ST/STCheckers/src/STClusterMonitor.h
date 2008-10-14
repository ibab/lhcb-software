// $Id: STClusterMonitor.h,v 1.4 2008-10-14 08:49:57 mneedham Exp $
#ifndef STClusterMonitor_H
#define STClusterMonitor_H 1

#include "Kernel/STHistoAlgBase.h"

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

class STClusterMonitor : public ST::HistoAlgBase {

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

  virtual void fillHistograms(const LHCb::STCluster* aCluster);

  // histograms
  DeSTDetector* m_tracker;
  
  std::string m_sigNoiseToolName;
  ISTSignalToNoiseTool* m_sigNoiseTool;

  std::string m_summaryLocation;
  std::string m_clusterLocation;
  
};

#endif // MCSTClusterMonitor_H
