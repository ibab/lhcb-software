// $Id: $
#ifndef PACKCLUSTER_H
#define PACKCLUSTER_H 1

#include <sstream>

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/PackedCluster.h"

/** @class PackCluster PackCluster.h
 *
 *  Pack the cluster on the tracks of a specified container.
 *
 *  @author Olivier Callot
 *  @date   2012-03-05
 */
class PackCluster : public GaudiAlgorithm 
{

public:

  /// Standard constructor
  PackCluster( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackCluster( ); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution

private:

  std::string m_inputName;
  std::string m_outputName; ///< Output location for packed clusters
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing

};

#endif // PACKCLUSTER_H
