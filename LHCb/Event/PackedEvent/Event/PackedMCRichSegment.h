// $Id: PackedMCRichSegment.h,v 1.1 2009-10-21 16:40:17 jonrob Exp $
#ifndef EVENT_PackedMCRichSegment_H
#define EVENT_PackedMCRichSegment_H 1

#include <string>
#include <vector>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/MCRichSegment.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

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
    /// Default constructor
    PackedMCRichSegment()
      : key(0), history(0),
        mcParticle(-1), mcRichTrack(-1)
    {}

    int key;
    int history;
    std::vector<int> trajPx, trajPy, trajPz;
    std::vector<int> trajMx, trajMy, trajMz;
    int mcParticle;
    int mcRichTrack;
    std::vector<int> mcPhotons, mcHits;

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

    /// Standard constructor
    PackedMCRichSegments( ) { }

    /// Destructor
    virtual ~PackedMCRichSegments( ) { }

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

  private:

    Vector m_vect; ///< The packed data objects

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

  public:

    /// Default Constructor
    MCRichSegmentPacker() {}

  public:

    /// Pack an MCRichSegment
    void pack( DataVector       & segs,
               PackedDataVector & psegs,
               const unsigned int version );

    /// Unpack an MCRichSegment
    void unpack( PackedDataVector & psegs,
                 DataVector       & segs );

    /// Compare two MCRichHits to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB,
                      GaudiAlgorithm & parent );

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PackedMCRichSegment_H
