#ifndef DECODEVELORAWBUFFER_H 
#define DECODEVELORAWBUFFER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// from Velo
#include "VeloDet/DeVelo.h"
#include "Kernel/VeloChannelID.h"
// from Event
#include "Event/VeloCluster.h"

/** @class DecodeVeloRawBuffer DecodeVeloRawBuffer.h
 *  Decode the Velo data from the Raw buffer
 
 *  @author Chris Parkes
 *  @date   2004-02-02
 */
class DecodeVeloRawBuffer : public Algorithm {
public:
  /// Standard constructor
  DecodeVeloRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DecodeVeloRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  DeVelo* m_velo;                  ///< Detector element
  VeloClusters* m_clusters; ///< vector to store clusters
  std::string m_outputContainer;      ///< Name of output container
};
#endif // DECODEVELORAWBUFFER_H
