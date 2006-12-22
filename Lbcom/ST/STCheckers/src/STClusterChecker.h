// $Id: STClusterChecker.h,v 1.4 2006-12-22 12:23:01 jvantilb Exp $
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
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */

class STClusterChecker : public GaudiHistoAlg {

public:
 
  /// constructer
  STClusterChecker( const std::string& name, 
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
