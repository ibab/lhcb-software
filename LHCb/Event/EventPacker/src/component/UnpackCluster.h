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

  /// Create and access Velo clusters on demand
  LHCb::VeloClusters * vClus()
  {
    if ( !m_vClus )
    {
      put( m_vClus = new LHCb::VeloClusters() , 
           LHCb::VeloClusterLocation::Default  + m_extension );
    }
    return m_vClus;
  }

  /// Create and access TT Clusters on demand
  LHCb::STClusters* ttClus()
  {
    if ( !m_ttClus )
    {
      put( m_ttClus = new LHCb::STClusters() ,
           LHCb::STClusterLocation::TTClusters + m_extension );
    }
    return m_ttClus;
  }

  /// Create and access UT Clusters on demand
  LHCb::STClusters* utClus()
  {
    if ( !m_utClus )
    {
      put( m_utClus = new LHCb::STClusters() ,
           LHCb::STClusterLocation::UTClusters + m_extension );
    }
    return m_utClus;
  }

  /// Create and access IT Clusters on demand
  LHCb::STClusters* itClus()
  {
    if ( !m_itClus )
    {
      put( m_itClus = new LHCb::STClusters() ,
           LHCb::STClusterLocation::ITClusters + m_extension );
    }
    return m_itClus;
  }

private:

  std::string m_inputName;    ///< Input name of packed clusters
  std::string m_extension;    ///< Name extension (for testing)
  bool m_alwaysOutput;        ///< Flag to turn on the creation of output, even when input is missing

  /// Check to prevent recursive calls to this, due to the multiple possible outputs
  bool m_running = false;

  // Cache pointers to containers, for on-demand creation
  LHCb::VeloClusters * m_vClus = nullptr; ///< Velo clusters
  LHCb::STClusters*   m_ttClus = nullptr; ///< TT Clusters
  LHCb::STClusters*   m_utClus = nullptr; ///< UT Clusters
  LHCb::STClusters*   m_itClus = nullptr; ///< IT Clusters

};

#endif // UNPACKCLUSTER_H
