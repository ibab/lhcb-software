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

  unsigned int getVertexType(const LHCb::MCHit *aHit);  ///< MCVertex information

  //Associator table
  typedef LinkerTool<LHCb::STCluster, LHCb::MCHit> AsctTool; ///< Linker tool
  typedef AsctTool::DirectType Table; ///< Direct STCluster/MCHit
  typedef Table::Range Range; ///< Range table
  typedef Table::iterator iterator; ///< Table iterator
  
  unsigned int m_nEvent; ///< Number of events
  unsigned int m_nMax; ///< Maximum number of clusters per array

  std::string m_asctLocation; ///< Associator table location
  std::string m_clusterLocation; ///< STClusters location
  std::string m_hitLocation;  ///< MCHits location

  bool m_MonteCarlo; ///< Dump Monte Carlo information in the tuple
  
};

#endif // STCLUSTERTUPLE_H
