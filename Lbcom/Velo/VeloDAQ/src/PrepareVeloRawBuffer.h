// $Id: PrepareVeloRawBuffer.h,v 1.2 2004-11-29 14:16:06 cattanem Exp $
#ifndef PREPAREVELORAWBUFFER_H 
#define PREPAREVELORAWBUFFER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

// from Event
#include "Event/VeloCluster.h"

/** @class PrepareVeloRawBuffer PrepareVeloRawBuffer.h
 *  Prepare the Velo Cluster container for fast access.
 *
 *  @author Olivier Callot
 *  @date   2003-04-11
 *  2004-04-13 : modified and moved to Velo area Chris Parkes
 *  2004-11-04 : modified to use new GaudiAlgorithm by David Hutchcroft
 */
class PrepareVeloRawBuffer : public GaudiAlgorithm {
public:
  /// Standard constructor
  PrepareVeloRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrepareVeloRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

  class sortClustersByID {
  public:
    bool operator () ( const VeloCluster* first, 
                       const VeloCluster* second ) const {
      if ( 0==first  ) return true;
      if ( 0==second ) return false;

      if ( first->sensor() != second->sensor() ) {
        return first->sensor() < second->sensor();
      }
      int type = first->sensor() % 8;

      //== Reversed Phi sensors: Increasing phi is decreasing strip number
      if ( 4 == type || 6 == type ) {
        return first->strip(0) > second->strip(0) ;
      }
      return first->strip(0) < second->strip(0) ;
    }
  };

protected:

private:
  DeVelo*          m_velo;            ///< Detector element
  VeloClusters* m_clusters; ///< vector to store clusters
};
#endif // PREPAREVELORAWBUFFER_H
