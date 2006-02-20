// $Id: PrepareVeloRawBuffer.h,v 1.6 2006-02-20 15:29:33 cattanem Exp $
#ifndef PREPAREVELORAWBUFFER_H 
#define PREPAREVELORAWBUFFER_H 1

#include <string>

#include "VeloEvent/InternalVeloCluster.h"

#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PrepareVeloRawBuffer PrepareVeloRawBuffer.h
 *  Prepare the cluster container from 1MHz raw buffer.
 *  Completely rewritten for the new 1MHz raw buffer.
 *
 *  @author David Jones
 *  @author Kurt Rinnert
 *  @author Olivier Callot
 *  @date   2003-04-11
 *  2004-04-13 : modified and moved to Velo area Chris Parkes
 *  2004-11-04 : modified to use new GaudiAlgorithm by David Hutchcroft
 *  2006-02-15 : completely rewritten for 1MHz raw buffer, Kurt Rinnert, David Jones
 */
class PrepareVeloRawBuffer : public GaudiAlgorithm {

public:
  /// Standard constructor
  PrepareVeloRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrepareVeloRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize();    ///< Algorithm finalisation

public:

  class sortClustersBySensorAndStrip {
  public:

    bool operator () ( const LHCb::InternalVeloCluster* first, 
                       const LHCb::InternalVeloCluster* second ) const {
      if ( 0==first  ) return true;
      if ( 0==second ) return false;

      if ( first->sensor() != second->sensor() ) {
        return first->sensor() < second->sensor();
      }

      return first->strip(0) < second->strip(0) ;
    }
  };

private:
  
  unsigned int makeBank (std::vector<const LHCb::InternalVeloCluster*>::const_iterator begin, 
                         std::vector<const LHCb::InternalVeloCluster*>::const_iterator end);


private:

  // configurable locations in the TES
  std::string m_clusterLoc;
  std::string m_rawEventLoc;
  
  // version tag, this strongly tied to the implementatio, hence const
  const int m_bankVersion;

  // long lived containers for performance reasons. Also used to communicate
  // with makeBank() method
  std::vector<const LHCb::InternalVeloCluster*> m_sortedClusters; 
  std::vector<buffer_word> m_rawData;
  std::vector<buffer_word> m_clusterADCBuffer;
  std::vector<buffer_word> m_clusterPosBuffer;
};
#endif // PREPAREVELORAWBUFFER_H
