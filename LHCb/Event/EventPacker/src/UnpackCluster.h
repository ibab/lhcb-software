// $Id: $
#ifndef UNPACKCLUSTER_H 
#define UNPACKCLUSTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/PackedCluster.h"

/** @class UnpackCluster UnpackCluster.h
 *  UNpack the clusters for Velo and ST
 *
 *  @author Olivier Callot
 *  @date   2012-03-06
 */
class UnpackCluster : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackCluster( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackCluster( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputName;
  std::string m_extension;
};
#endif // UNPACKCLUSTER_H
