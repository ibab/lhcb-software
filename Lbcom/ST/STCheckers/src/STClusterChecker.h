// $Id: STClusterChecker.h,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
#ifndef _STClusterChecker_H
#define _STClusterChecker_H

#include <vector>
#include <string>
#include <map>

#include "GaudiAlg/GaudiHistoAlg.h"

namespace LHCb{
  class STCluster;
};
class ISTSignalToNoiseTool;
class DeSTDetector;


/** @class STClusterChecker STClusterChecker.h
 *
 *  Class for checking STClusters
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

#endif // _MCSTDepositChecker_H














