#ifndef STCLUSTERTUPLE_H 
#define STCLUSTERTUPLE_H 1

// Include files
#include "Kernel/STTupleAlgBase.h"
#include "Linker/LinkerTool.h"

namespace LHCb
{
  class STCluster;
  class MCHit;
}

/** @class STClusterTuple STClusterTuple.h
 *   
 *  Algorithm that dumps info related to
 *  STClusters (and associated MCHits)
 *  into a tuple
 *
 *  @author Vincenzo Battista
 *  @date   2015-06-05
 */
class STClusterTuple : public ST::TupleAlgBase {
public: 
  /// Standard constructor
  STClusterTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STClusterTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  //bool Origin(const LHCb::MCHit* aHit, std::string check);
  bool Origin(const LHCb::MCHit* aHit, LHCb::MCVertex::MCVertexType check);

private:

  //Associator table
  typedef LinkerTool<LHCb::STCluster, LHCb::MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;
  
  //Useful arrays, strings and counters
  std::vector<std::string> m_spillVector;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_Flags;
  
  unsigned int m_nEvent;

  std::string m_asctLocation;
  std::string m_clusterLocation;
  std::string m_hitLocation;

  unsigned int m_countClus;
  unsigned int m_nMax;
  mutable std::map<std::string, unsigned int> m_countHits;
  mutable std::map<std::string, unsigned int> m_countFlags;
  double m_clusCharge;

  bool m_MCVertexInfo;
  
};

#endif // STCLUSTERTUPLE_H
