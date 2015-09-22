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

private:

  //Associator table
  typedef LinkerTool<LHCb::STCluster, LHCb::MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;
  
  unsigned int m_nEvent;
  unsigned int m_nMax;

  std::string m_asctLocation;
  std::string m_clusterLocation;
  std::string m_hitLocation;

  bool m_MonteCarlo;

  //int getSpill(const LHCb::STCluster *aCluster);
  
};

#endif // STCLUSTERTUPLE_H
