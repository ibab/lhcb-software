#ifndef STClusterClassification_H
#define STClusterClassification_H 1

#include "Kernel/STHistoAlgBase.h"

#include "Linker/LinkerTool.h"

namespace LHCb{
 class STCluster;
 class MCHit;
}

/** @class STClusterClassification STClusterClassification.h
 *
 *  This algorithm counts the total number of STClusters from
 *  - a primary interaction
 *  - a secondary interaction
 *  - noise
 *  - spillover
 *  - unknown sources.
 *  It prints out the statistics in the finalize method.
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   04/12/2006
 */


class STClusterClassification : public ST::HistoAlgBase {

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

  std::vector<std::string> m_spillVector;  // short names of spills
  std::vector<std::string>  m_spillNames; // full name of spills
     
  int m_nEvent;

  std::string m_asctLocation;
  std::string m_clusterLocation;
  std::string m_hitLocation; 

  mutable std::map<std::string, unsigned int> m_infoMap;

};

#endif // STClusterClassification_H













