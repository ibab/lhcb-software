// $Id: PackedCaloCluster.h,v 1.3 2009-11-10 10:24:09 jonrob Exp $
#ifndef EVENT_PackedCaloCluster_H
#define EVENT_PackedCaloCluster_H 1

#include <string>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/CaloCluster.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

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
    /// Default constructor
    PackedCaloClusterEntry()
      : digit(-1), status(0), fraction(0)
    { }

    long long digit;
    unsigned int status;
    short int fraction;
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
    /// Default constructor
    PackedCaloCluster()
      : key(0), type(0), seed(0),
        pos_x(0), pos_y(0), pos_z(0), pos_e(0),
        pos_c0(0), pos_c1(0),
        pos_cov00(0), pos_cov11(0), pos_cov22(0),
        pos_cov10(0), pos_cov20(0), pos_cov21(0),
        firstEntry(0), lastEntry(0)
    { }

    int key;
    int type;
    unsigned int seed;
    int pos_x, pos_y, pos_z, pos_e;
    int pos_c0, pos_c1;
    int       pos_cov00,pos_cov11,pos_cov22;
    short int pos_cov10,pos_cov20,pos_cov21;
    int pos_spread00,pos_spread11;
    short int pos_spread10;
    unsigned short int firstEntry, lastEntry;
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

    /// Standard constructor
    PackedCaloClusters( ) : m_packingVersion(defaultPackingVersion()) { }

    /// Destructor
    virtual ~PackedCaloClusters( ) { }

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
    char m_packingVersion;

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
    CaloClusterPacker() : m_parent(NULL) {}

  public:

    /// Default Constructor
    CaloClusterPacker( GaudiAlgorithm & parent ) : m_parent(&parent) {}

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
    GaudiAlgorithm& parent() const { return *m_parent; }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

    /// Pointer to parent algorithm
    GaudiAlgorithm * m_parent;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PackedCaloCluster_H
