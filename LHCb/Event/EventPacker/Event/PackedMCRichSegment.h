// $Id: PackedMCRichSegment.h,v 1.4 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PackedMCRichSegment_H
#define EVENT_PackedMCRichSegment_H 1

#include <string>
#include <vector>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/MCRichSegment.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedMCRichSegment Event/PackedMCRichSegment.h
   *
   *  Packed MCRichSegment
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedMCRichSegment
  {
    int key{0};
    int history{0};
    std::vector<int> trajPx, trajPy, trajPz;
    std::vector<int> trajMx, trajMy, trajMz;
    long long mcParticle{-1};
    long long mcRichTrack{-1};
    std::vector<long long> mcPhotons, mcHits;
  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMCRichSegments = 1523;

  /// Namespace for locations in TDS
  namespace PackedMCRichSegmentLocation
  {
    static const std::string& Default = "pSim/Rich/Segments";
  }

  /** @class PackedMCRichSegments Event/PackedMCRichSegment.h
   *
   *  Packed MCRichSegments
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedMCRichSegments : public DataObject
  {
  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedMCRichSegment> Vector;

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 1; }

  public:

    /// Class ID
    static const CLID& classID() { return CLID_PackedMCRichSegments; }

    /// Class ID
    virtual const CLID& clID() const { return classID(); }

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
    char   m_packingVersion{ defaultPackingVersion() };

    /// The packed data objects
    Vector m_vect; 

  };

  // -----------------------------------------------------------------------

  /** @class MCRichSegmentPacker Event/PackedMCRichSegment.h
   *
   *  Utility class to handle the packing and unpacking of the MCRichSegments
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCRichSegmentPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::MCRichSegment                    Data;
    typedef LHCb::PackedMCRichSegment        PackedData;
    typedef LHCb::MCRichSegments             DataVector;
    typedef LHCb::PackedMCRichSegments PackedDataVector;
    static const std::string& packedLocation()
    { return LHCb::PackedMCRichSegmentLocation::Default; }
    static const std::string& unpackedLocation()
    { return LHCb::MCRichSegmentLocation::Default; }

  private:

    /// Default Constructor hidden
    MCRichSegmentPacker() {}

  public:

    /// Constructor
    MCRichSegmentPacker( const GaudiAlgorithm * p ) : m_pack(p) {}

  public:

    /// Pack an MCRichSegment
    void pack( const DataVector & segs,
               PackedDataVector & psegs ) const;

    /// Unpack an MCRichSegment
    void unpack( const PackedDataVector & psegs,
                 DataVector             & segs ) const;

    /// Compare two MCRichHits to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

  private:

    /// Access the parent algorithm
    const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PackedMCRichSegment_H
