#ifndef EVENT_PackedCaloCluster_H
#define EVENT_PackedCaloCluster_H 1

#include <string>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/CaloCluster.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct CaloClusterEntry Event/PackedCaloCluster.h
   *
   *  Packed LHCb::CaloClusterEntry
   *
   *  @author Christopher Rob Jones
   *  @date   2012-03-30
   */
  struct PackedCaloClusterEntry
  {
    long long digit{-1};
    unsigned int status{0};
    short int fraction{0};
  };

  /** @struct PackedCaloCluster Event/PackedCaloCluster.h
   *
   *  Packed LHCb::CaloCluster
   *
   *  @author Christopher Rob Jones
   *  @date   2012-03-30
   */
  struct PackedCaloCluster
  {
    int key{0};
    int type{0};
    unsigned int seed{0};
    int pos_x{0}, pos_y{0}, pos_z{0}, pos_e{0};
    int pos_c0{0}, pos_c1{0};
    int       pos_cov00{0},pos_cov11{0},pos_cov22{0};
    short int pos_cov10{0},pos_cov20{0},pos_cov21{0};
    int pos_spread00{0},pos_spread11{0};
    short int pos_spread10{0};
    unsigned short int firstEntry{0}, lastEntry{0};
  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedCaloClusters = 1541;

  /// Namespace for locations in TDS
  namespace PackedCaloClusterLocation
  {
    static const std::string& Default = "pRec/Calo/Clusters";
  }

  /** @class PackedCaloClusters Event/PackedCaloCluster.h
   *
   *  Packed Calo Clusters
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedCaloClusters : public DataObject
  {

  public:

    /// Vector of PackedCaloCluster objects
    typedef std::vector<LHCb::PackedCaloCluster> ClusterVector;

    /// Vector of PackedCaloClusterEntry objects
    typedef std::vector<LHCb::PackedCaloClusterEntry> ClusterEntryVector;

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 0; }
    
  public:

    /// Class ID
    static const CLID& classID() { return CLID_PackedCaloClusters; }

    /// Class ID
    virtual const CLID& clID() const { return PackedCaloClusters::classID(); }

  public:

    /// Write access to the data vector
    ClusterVector & data()                     { return m_clusters; }

    /// Read access to the data vector
    const ClusterVector & data() const         { return m_clusters; }

    /// Write access to the data vector
    ClusterEntryVector & entries()             { return m_entries;  }

    /// Read access to the data vector
    const ClusterEntryVector & entries() const { return m_entries;  }

    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }

    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }

  private:

    /// Data packing version
    char m_packingVersion{ defaultPackingVersion() };

    /// The packed data objects
    ClusterVector m_clusters; 

    /// Packed Calo Entries
    ClusterEntryVector m_entries;

  };

  // -----------------------------------------------------------------------

  /** @class CaloClusterPacker Event/PackedCaloCluster.h
   *
   *  Utility class to handle the packing and unpacking of the Calo Clusters
   *
   *  @author Christopher Rob Jones
   *  @date   2012-03-30
   */
  class CaloClusterPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::CaloCluster                     Data;
    typedef LHCb::PackedCaloCluster         PackedData;
    typedef LHCb::CaloClusters              DataVector;
    typedef LHCb::PackedCaloClusters  PackedDataVector;
    static const std::string& packedLocation()   
    {
      return LHCb::PackedCaloClusterLocation::Default; 
    }
    static const std::string& unpackedLocation() 
    {
      return LHCb::CaloClusterLocation::Default; 
    }

  private:

    /// Default Constructor hidden
    CaloClusterPacker() {}

  public:

    /// Default Constructor
    CaloClusterPacker( const GaudiAlgorithm * p ) : m_pack(p) {}

  public:

    /// Pack Calo Clusters
    void pack( const DataVector & clus,
               PackedDataVector & pclus ) const;

    /// Unpack Calo Clusters
    void unpack( const PackedDataVector & pclus,
                 DataVector             & cluss ) const;

    /// Compare two Calo Clusters containers to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

    /// Compare two Calo Clusters to check the packing -> unpacking performance
    StatusCode check( const Data & dataA,
                      const Data & dataB ) const;

  public:

    /// Access the parent algorithm
    const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

    /// Safe sqrt ...
    inline double safe_sqrt( const double x ) const
    { return ( x > 0 ? std::sqrt(x) : 0.0 ); }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PackedCaloCluster_H
