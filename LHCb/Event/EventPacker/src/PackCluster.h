// $Id: $
#ifndef PACKCLUSTER_H 
#define PACKCLUSTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/PackedCluster.h"

/** @class PackCluster PackCluster.h
 *  Pack the cluster on the tracks of a specified container.
 *
 *  @author Olivier Callot
 *  @date   2012-03-05
 */
class PackCluster : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackCluster( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackCluster( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
};
#endif // PACKCLUSTER_H
