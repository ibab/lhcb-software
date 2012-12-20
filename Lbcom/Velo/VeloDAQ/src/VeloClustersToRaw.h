#ifndef VELOCLUSTERSTORAW_H 
#define VELOCLUSTERSTORAW_H 1

#include <vector>
#include <algorithm>
#include <string>

#include "Event/VeloCluster.h"

#include "GaudiAlg/GaudiAlgorithm.h"

class DeVelo;
namespace LHCb {
  class RawEvent;
}

/** @class VeloClustersToRaw VeloClustersToRaw.h
 *  Create a raw VELO cluster bank from VeloCluster container.
 *
 *  @author Kurt Rinnert
 *  @date   2011-09-20
 */
class VeloClustersToRaw : public GaudiAlgorithm {

public:
  /// Standard constructor
  VeloClustersToRaw( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloClustersToRaw( ); ///< Destructor

  virtual StatusCode initialize();  ///< Algorithm initialization
  virtual StatusCode execute   ();  ///< Algorithm execution

private:
  

  unsigned int makeBank (const unsigned int sensor, std::vector<const LHCb::VeloCluster*>::const_iterator& begin);

  StatusCode storeBank(const unsigned int sensor, std::vector<const LHCb::VeloCluster*>::const_iterator& begin); 

  bool selfTest(); ///< run self test on default locations

private:

  // configurable locations in the TES
  std::string m_clusterLoc;      ///< cluster container input location
  std::string m_rawEventLoc;     ///< raw bank output location
  
  unsigned int m_bankVersion; ///< raw bank version tag

  bool m_runSelfTest; ///< run self test on default locations

  // long lived containers for performance reasons. Also used to communicate
  // with makeBank() method
  std::vector<const LHCb::VeloCluster*> m_sortedClusters; 
  std::vector<SiDAQ::buffer_word> m_rawData;
  std::vector<SiDAQ::buffer_word> m_clusterADCBuffer;
  std::vector<SiDAQ::buffer_word> m_clusterPosBuffer;

  LHCb::RawEvent* m_rawEventOut;

  // size of raw bank in bytes, inclding the 4 byte header but
  // *without* the padding bytes at the end
  unsigned int m_bankSizeInBytes;
  
  /// pointer to Velo Detector Element
  DeVelo* m_velo;
  
};
#endif // VELOCLUSTERSTORAW_H
