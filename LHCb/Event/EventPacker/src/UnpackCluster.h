// $Id: $
#ifndef UNPACKCLUSTER_H
#define UNPACKCLUSTER_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/PackedCluster.h"

/** @class UnpackCluster UnpackCluster.h
 *  UNpack the clusters for Velo and ST
 *
 *  @author Olivier Callot
 *  @date   2012-03-06
 */
class UnpackCluster : public GaudiAlgorithm 
{

public:

  /// Standard constructor
  UnpackCluster( const std::string& name, 
                 ISvcLocator* pSvcLocator );

  virtual ~UnpackCluster( ); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution

private:

  std::string m_inputName;    ///< Input name of packed clusters
  std::string m_extension;    ///< Name extension (for testing)
  bool m_alwaysOutput;        ///< Flag to turn on the creation of output, even when input is missing

  /// Check to prevent recursive calls to this, due to the multiple possible outputs
  bool m_running;

};

#endif // UNPACKCLUSTER_H
