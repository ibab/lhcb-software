// $Id: PackedMuonPID.h,v 1.3 2009-11-10 10:24:09 jonrob Exp $
#ifndef EVENT_PACKEDMUONPID_H
#define EVENT_PACKEDMUONPID_H 1

#include <string>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/MuonPID.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

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
    /// Default constructor
    PackedMuonPID()
      : MuonLLMu(0), MuonLLBg(0),
        nShared(0), status(0),
        idtrack(-1),
        mutrack(-1),
        key(0)
    {}

    int MuonLLMu;
    int MuonLLBg;
    int nShared;
    int status;
    int idtrack;
    int mutrack;
    int key;

  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMuonPIDs = 1571;

  /// Namespace for locations in TDS
  namespace PackedMuonPIDLocation
  {
    static const std::string& Default = "pRec/Muon/MuonPID";
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
    static char defaultPackingVersion() { return 1; }
    
  public:

    /// Standard constructor
    PackedMuonPIDs( ) : m_packingVersion(defaultPackingVersion()) { }

    /// Destructor
    virtual ~PackedMuonPIDs( ) { }

    /// Class ID
    static const CLID& classID() { return CLID_PackedMuonPIDs; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMuonPIDs::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }

    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }

  private:

    /// Data packing version (not used as yet, but for any future schema evolution)
    char   m_packingVersion;

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

  public:

    /// Default Constructor
    MuonPIDPacker() {}

  public:

    /// Pack MuonPIDs
    void pack( const DataVector & hits,
               PackedDataVector & phits ) const;

    /// Unpack MuonPIDs
    void unpack( const PackedDataVector & phits,
                 DataVector       & hits ) const;

    /// Compare two MuonPIDs to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB,
                      GaudiAlgorithm & parent ) const;

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PACKEDMUONPID_H
