// $Id: PackedRichPID.h,v 1.5 2010-03-09 18:35:20 jonrob Exp $
#ifndef EVENT_PACKEDRICHPID_H
#define EVENT_PACKEDRICHPID_H 1

#include <string>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/RichPID.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

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
    /// Default constructor
    PackedRichPID()
      : pidResultCode(0),
        dllEl(0),dllMu(0),dllPi(0),dllKa(0),dllPr(0),
        track(-1),
        dllBt(0),
        key(0)
    {}

    int pidResultCode;                      
    int dllEl,dllMu,dllPi,dllKa,dllPr;
    int track;          
    int dllBt;
    int key;
  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedRichPIDs = 1561;

  /// Namespace for locations in TDS
  namespace PackedRichPIDLocation
  {
    static const std::string& Default = "pRec/Rich/PIDs";
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
    static char defaultPackingVersion() { return 2; }

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

    /// Data packing version (not used as yet, but for any future schema evolution)
    char   m_packingVersion;

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
    RichPIDPacker() : m_parent(NULL) {}

  public:

    /// Constructor
    RichPIDPacker( GaudiAlgorithm & parent ) : m_parent(&parent) {}

  public:

    /// Pack RichPIDs
    void pack( const DataVector & pids,
               PackedDataVector & ppids ) const;

    /// Unpack RichPIDs
    void unpack( const PackedDataVector & ppids,
                 DataVector             & pids ) const;

    /// Compare two RichPIDs to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

  private:

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

#endif // EVENT_PACKEDRICHPID_H
