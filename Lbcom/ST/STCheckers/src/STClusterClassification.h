// $Id $
#ifndef _STClusterClassification_H
#define _STClusterClassification_H

#include <map>
#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"

#include "Linker/LinkerTool.h"

namespace LHCb{
 class STCluster;
 class MCHit;
};



/** @class STClusterClassification STClusterClassification.h
 *
 *  Class for checking STNoiseClusters
 *
 *  @author M.Needham
 *  @date   21/4/2001
 */

// from Associators
#include "Linker/LinkerTool.h"

class STClusterClassification : public GaudiAlgorithm {

public:
 
  /// constructer
  STClusterClassification(const std::string& name, 
                 ISvcLocator *svcloc );

  /// destructer
  virtual ~STClusterClassification();

  /// initialize
  StatusCode initialize();

  /// execute
  StatusCode execute();

  /// finalize
  StatusCode finalize();

private:

  typedef LinkerTool<LHCb::STCluster, LHCb::MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  StatusCode fillInfo(const LHCb::MCHit* aHit ) const;
  std::string  findSpill(const LHCb::MCHit* aHit) const;
  
  unsigned int tCount() const;

  std::vector<std::string> m_SpillVector;  // short names of spills
  std::vector<std::string>  m_spillNames; // full name of spills
     
  // index of event spill
  int m_eventIndex;
  int m_nStation;


  std::string m_asctLocation;
  std::string m_clusterLocation;
  std::string m_hitLocation; 
  std::string m_detType;

  mutable std::map<std::string, unsigned int> m_infoMap;

};

#endif // _STClusterClassification_H













