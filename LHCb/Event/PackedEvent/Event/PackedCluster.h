// $Id: $
#ifndef EVENT_PACKEDCLUSTER_H 
#define EVENT_PACKEDCLUSTER_H 1
#include "GaudiKernel/DataObject.h"
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include <string>
#include <vector>

namespace LHCb {
  // Include files
  
  /** @class PackedCluster PackedCluster.h Event/PackedCluster.h
   *  Packed description of a cluster (Velo, ST) for mDST transmission
   *
   *  @author Olivier Callot
   *  @date   2012-03-05
   */
  struct PackedCluster {
    /// Standard constructor
    PackedCluster( ) : id(0), begin(0), end(0), sum(0), sourceID(0), tell1Channel(0), spill(0) {}; 
    
    /// Copy constructor
    PackedCluster( const PackedCluster& c ) :
      id(c.id), begin(c.begin), end(c.end), sum(c.sum), sourceID(c.sourceID),
      tell1Channel(c.tell1Channel), spill(c.spill) {};
    
    unsigned int id;
    unsigned int begin;
    unsigned int end;
    unsigned int sum;
    unsigned int sourceID;
    unsigned int tell1Channel;
    int spill;
  };
  
  static const CLID CLID_PackedClusters = 1540;
  
  namespace PackedClusterLocation {
    static const std::string& Default = "pRec/Clusters";
  }
  
  class PackedClusters : public DataObject {
  public:
    /// Standard constructor
    PackedClusters() {
      m_clusters.reserve( 100 );
      m_strips.reserve(   1000 );
      m_adcs.reserve(     1000 );
    }
    virtual ~PackedClusters() {}; /// Destructor

    virtual const CLID& clID()  const { return PackedClusters::classID(); }
    static  const CLID& classID()     { return CLID_PackedClusters;       }
    
    void addVeloCluster( LHCb::VeloCluster* vCl ) {
      PackedCluster t;
      t.id = 0x20000000 + vCl->liteCluster().channelID().channelID();
      int size = (8*vCl->interStripFraction()) + 0.5;
      t.id += size * 0x1000000;
      if ( vCl->pseudoSize() > 2 ) t.id += 0x8000000;
      if ( vCl->highThreshold() )  t.id += 0x10000000;
      t.begin = m_strips.size();
      for ( unsigned int kk = 0; vCl->size() > kk; ++kk ) {
        m_strips.push_back( vCl->strip(kk)    );
        m_adcs.push_back(   vCl->adcValue(kk) );
      }
      t.end = m_strips.size();
      m_clusters.push_back( t );
    }
    
    void addTTCluster( LHCb::STCluster* sCl ) {
      addSTCluster( sCl, 0x40000000 );
    }

    void addITCluster( LHCb::STCluster* sCl ) {
      addSTCluster( sCl, 0x60000000 );
    }

    void addSTCluster(  LHCb::STCluster* sCl, unsigned int key ) {
      PackedCluster t;
      t.id = key + sCl->liteCluster().channelID().channelID();
      int size = (4*sCl->interStripFraction()) + 0.5;
      t.id += size * 0x1000000;
      if ( sCl->pseudoSize() > 2 ) t.id += 0x4000000;
      if ( sCl->highThreshold() )  t.id += 0x8000000;
      t.begin = m_strips.size();
      for ( unsigned int kk = 0; sCl->size() > kk; ++kk ) {
        m_strips.push_back( sCl->strip(kk)    );
        m_adcs.push_back(   sCl->adcValue(kk) );
      }
      t.end = m_strips.size();
      t.sum          = sCl->neighbourSum();
      t.sourceID     = sCl->sourceID();
      t.tell1Channel = sCl->tell1Channel();
      t.spill        = int( sCl->spill() );
      m_clusters.push_back( t );
    }

    const std::vector<PackedCluster>& clusters() const { return m_clusters; }
    const std::vector<int>& strips()             const { return m_strips; }
    const std::vector<unsigned int>& adcs()      const { return m_adcs; }

  private:
    std::vector<PackedCluster> m_clusters;
    std::vector<int>           m_strips;
    std::vector<unsigned int>  m_adcs;
  };
};
#endif // EVENT_PACKEDCLUSTER_H
