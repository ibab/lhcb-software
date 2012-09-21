// $Id: $
#ifndef EVENT_PACKEDCLUSTER_H
#define EVENT_PACKEDCLUSTER_H 1
#include "GaudiKernel/DataObject.h"
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include <string>
#include <vector>

namespace LHCb 
{
 
  /** @class PackedCluster PackedCluster.h Event/PackedCluster.h
   *  Packed description of a cluster (Velo, ST) for mDST transmission
   *
   *  @author Olivier Callot
   *  @date   2012-03-05
   */
  struct PackedCluster 
  {
  
    /// Standard constructor
    PackedCluster( ) 
      : id(0), 
        begin(0), end(0), 
        sum(0), sourceID(0), 
        tell1Channel(0), spill(0) 
    { }

    /// Copy constructor
    PackedCluster( const PackedCluster& c )
      : id(c.id), 
        begin(c.begin), end(c.end), 
        sum(c.sum), sourceID(c.sourceID),
        tell1Channel(c.tell1Channel), spill(c.spill) 
    { }

    unsigned int id;
    unsigned int begin;
    unsigned int end;
    unsigned int sum;
    unsigned int sourceID;
    unsigned int tell1Channel;
    int spill;

  };

  static const CLID CLID_PackedClusters = 1540;

  namespace PackedClusterLocation 
  {
    static const std::string& Default = "pRec/Track/Clusters";
  }

  class PackedClusters : public DataObject 
  {
  
  public:
  
    /// Standard constructor
    PackedClusters() 
    {
      m_clusters.reserve ( 100  );
      m_strips.reserve   ( 1000 );
      m_adcs.reserve     ( 1000 );
    }
    
    virtual ~PackedClusters() {}; /// Destructor

    virtual const CLID& clID()  const { return PackedClusters::classID(); }
    static  const CLID& classID()     { return CLID_PackedClusters;       }

  public:

    void addVeloCluster( const LHCb::VeloCluster* vCl );

    void addTTCluster( const LHCb::STCluster* sCl ) 
    {
      addSTCluster( sCl, 0x40000000 );
    }

    void addUTCluster( const LHCb::STCluster* sCl ) 
    {
      addSTCluster( sCl, 0x50000000 ); // need to find the proper key (JC)
    }

    void addITCluster( const LHCb::STCluster* sCl )
    {
      addSTCluster( sCl, 0x60000000 );
    }

    void addSTCluster( const LHCb::STCluster* sCl, 
                       const unsigned int key );

  public:

    const std::vector<PackedCluster>& clusters() const { return m_clusters; }
    const std::vector<int>& strips()             const { return m_strips; }
    const std::vector<unsigned int>& adcs()      const { return m_adcs; }

  private:

    std::vector<PackedCluster> m_clusters;
    std::vector<int>           m_strips;
    std::vector<unsigned int>  m_adcs;

  };

}

#endif // EVENT_PACKEDCLUSTER_H
