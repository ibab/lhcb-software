// $Id: PackedMuonPID.h,v 1.3 2009-11-10 10:24:09 jonrob Exp $
#ifndef EVENT_PACKEDMUONPID_H
#define EVENT_PACKEDMUONPID_H 1

#include <string>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/MuonPID.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedMuonPID Event/PackedMuonPID.h
   *
   *  Packed MuonPID
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedMuonPID
  {
    int MuonLLMu{0};
    int MuonLLBg{0};
    int nShared{0};
    int status{0};
    long long idtrack{-1};
    long long mutrack{-1};
    long long key{-1};
  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMuonPIDs = 1571;

  /// Namespace for locations in TDS
  namespace PackedMuonPIDLocation
  {
    static const std::string& Default  =  "pRec/Muon/MuonPID";
    static const std::string& InStream = "/pRec/Muon/CustomPIDs";
  }

  /** @class PackedMuonPIDs Event/PackedMuonPID.h
   *
   *  Packed MuonPIDs
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedMuonPIDs : public DataObject
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedMuonPID> Vector;

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 2; }
    
  public:

    /// Class ID
    static const CLID& classID() { return CLID_PackedMuonPIDs; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMuonPIDs::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

  public:

    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }

    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }

  private:

    /// Data packing version 
    char   m_packingVersion{ defaultPackingVersion() };

    /// The packed data objects
    Vector m_vect; 

  };

  // -----------------------------------------------------------------------

  /** @class MuonPIDPacker Event/PackedMuonPID.h
   *
   *  Utility class to handle the packing and unpacking of the MuonPIDs
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MuonPIDPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::MuonPID                    Data;
    typedef LHCb::PackedMuonPID        PackedData;
    typedef LHCb::MuonPIDs             DataVector;
    typedef LHCb::PackedMuonPIDs PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedMuonPIDLocation::Default; }
    static const std::string& unpackedLocation() { return LHCb::MuonPIDLocation::Default; }

  private:

    /// Default Constructor hidden
    MuonPIDPacker() {}

  public:

    /// Default Constructor
    MuonPIDPacker( const GaudiAlgorithm * p ) : m_pack(p) {}

  public:

    /// Pack a MuonPID
    void pack( const Data & pid,
               PackedData & ppid,
               PackedDataVector & ppids ) const;

    /// Pack MuonPIDs
    void pack( const DataVector & pids,
               PackedDataVector & ppids ) const;

    /// Unpack a single MuonPID
    void unpack( const PackedData       & ppid,
                 Data                   & pid,
                 const PackedDataVector & ppids,
                 DataVector             & pids ) const;

    /// Unpack MuonPIDs
    void unpack( const PackedDataVector & ppids,
                 DataVector             & pids ) const;

    /// Compare two MuonPID containers to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

    /// Compare two MuonPIDs to check the packing -> unpacking performance
    StatusCode check( const Data & dataA,
                      const Data & dataB ) const;

  private:

    /// Access the parent algorithm
    const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

    /// Check if the given packing version is supported
    bool isSupportedVer( const char& ver ) const
    {
      const bool OK = ( 2 == ver || 1 == ver || 0 == ver );
      if ( !OK )
      {
        std::ostringstream mess;
        mess << "Unknown packed data version " << (int)ver;
        throw GaudiException( mess.str(), "MuonPIDPacker", StatusCode::FAILURE );
      }
      return OK;
    }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PACKEDMUONPID_H
