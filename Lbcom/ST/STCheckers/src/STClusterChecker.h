// $Id: STClusterChecker.h,v 1.3 2006-12-21 17:54:48 jvantilb Exp $
#ifndef STClusterChecker_H
#define STClusterChecker_H 1

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
  class STCluster;
};
class ISTSignalToNoiseTool;
class DeSTDetector;


/** @class STClusterChecker STClusterChecker.h
 *
 *  Class for monitoring STClusters
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

class STClusterChecker : public GaudiHistoAlg {

public:
 
  /// constructer
  STClusterChecker(const std::string& name, 
                   ISvcLocator *svcloc );

  /// destructer
  virtual ~STClusterChecker();

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

#endif // MCSTClusterChecker_H
