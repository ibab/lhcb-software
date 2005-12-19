// $Id: STNoiseClusterMonitor.h,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
#ifndef _STNoiseClusterMonitor_H
#define _STNoiseClusterMonitor_H

#include <vector>
#include <string>

class AIDA::IHistogram1D;
#include "GaudiAlg/GaudiHistoAlg.h"

#include "Linker/LinkerTool.h"

namespace LHCb{
 class STCluster;
 class MCHit;
};

class DeSTDetector;

/** @class STNoiseClusterMonitor STNoiseClusterMonitor.h
 *
 *  Class for checking STNoiseClusters
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

// from Associators
#include "Linker/LinkerTool.h"

class STNoiseClusterMonitor : public GaudiHistoAlg {

public:
 
  /// constructer
  STNoiseClusterMonitor(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~STNoiseClusterMonitor();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

private:

  typedef LinkerTool<LHCb::STCluster, LHCb::MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  virtual StatusCode fillHistograms(const LHCb::MCHit* aHit,
                                    const LHCb::STCluster* aCluster) const;
  int  findSpillIndex(const LHCb::MCHit* aHit) const;
  
  std::vector<std::string> m_SpillVector;  // short names of spills
  std::vector<std::string>  m_spillNames; // full name of spills
     
  // index of event spill
  int m_eventIndex;
  int m_nStation;

  DeSTDetector* m_tracker;
  std::string m_detType;

  std::string m_clusterLocation;
  std::string m_hitLocation; 

};

#endif // _STNoiseClusterMonitor_H













