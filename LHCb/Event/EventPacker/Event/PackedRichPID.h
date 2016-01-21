// $Id: PackedRichPID.h,v 1.5 2010-03-09 18:35:20 jonrob Exp $
#ifndef EVENT_PACKEDRICHPID_H
#define EVENT_PACKEDRICHPID_H 1

#include <string>
#include <sstream>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/RichPID.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GaudiException.h"

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedRichPID Event/PackedRichPID.h
   *
   *  Packed RichPID
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedRichPID
  {
    int pidResultCode{0};                      
    int dllEl{0},dllMu{0},dllPi{0},dllKa{0},dllPr{0};
    long long track{-1};          
    int dllBt{0};
    long long key{0};
  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedRichPIDs = 1561;

  /// Namespace for locations in TDS
  namespace PackedRichPIDLocation
  {
    static const std::string& Default  = "pRec/Rich/PIDs";
    static const std::string& InStream = "/pRec/Rich/CustomPIDs";
  }

  /** @class PackedRichPIDs Event/PackedRichPID.h
   *
   *  Packed RichPIDs
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedRichPIDs : public DataObject
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedRichPID> Vector;

  public:

    /// Default Packing Version
    static char defaultPackingVersion() { return 3; }

  public:

    /// Standard constructor
    PackedRichPIDs( ) : m_packingVersion(defaultPackingVersion()) { }

    /// Destructor
    virtual ~PackedRichPIDs( ) { }

    /// Class ID
    static const CLID& classID() { return CLID_PackedRichPIDs; }

    /// Class ID
    virtual const CLID& clID() const { return PackedRichPIDs::classID(); }

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

    /// Data packing version
    char   m_packingVersion{0};

    /// The packed data objects
    Vector m_vect; 

  };

  // -----------------------------------------------------------------------

  /** @class RichPIDPacker Event/PackedRichPID.h
   *
   *  Utility class to handle the packing and unpacking of the RichPIDs
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class RichPIDPacker
  {

  public:

    // These are required by the templated algorithms
    typedef LHCb::RichPID                    Data;
    typedef LHCb::PackedRichPID        PackedData;
    typedef LHCb::RichPIDs             DataVector;
    typedef LHCb::PackedRichPIDs PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedRichPIDLocation::Default; }
    static const std::string& unpackedLocation() { return LHCb::RichPIDLocation::Default; }

  private:

    /// Default Constructor hidden
    RichPIDPacker() {}

  public:

    /// Constructor
    RichPIDPacker( const GaudiAlgorithm & parent ) : m_pack(&parent) { }

  public:

    /// Pack a RichPID
    void pack( const Data & pid,
               PackedData & ppid,
               PackedDataVector & ppids ) const;

    /// Pack RichPIDs
    void pack( const DataVector & pids,
               PackedDataVector & ppids ) const;

    /// Unpack a single RichPID
    void unpack( const PackedData       & ppid,
                 Data                   & pid,
                 const PackedDataVector & ppids,
                 DataVector             & pids ) const;

    /// Unpack RichPIDs
    void unpack( const PackedDataVector & ppids,
                 DataVector             & pids ) const;

    /// Compare two RichPID containers to check the packing -> unpacking performance
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
      const bool OK = ( 0 <= ver && ver <= 3 );
      if ( !OK )
      {
        std::ostringstream mess;
        mess << "Unknown packed data version " << (int)ver;
        throw GaudiException( mess.str(), "RichPIDPacker", StatusCode::FAILURE );
      }
      return OK;
    }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PACKEDRICHPID_H
