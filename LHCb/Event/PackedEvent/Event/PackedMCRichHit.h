// $Id: PackedMCRichHit.h,v 1.1 2009-10-21 16:40:16 jonrob Exp $
#ifndef EVENT_PACKEDMCRICHHIT_H
#define EVENT_PACKEDMCRICHHIT_H 1

#include <string>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/MCRichHit.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedMCRichHit Event/PackedMCRichHit.h
   *
   *  Packed MCRichHit
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedMCRichHit
  {
    /// Default constructor
    PackedMCRichHit()
      : x(0), y(0), z(0),
        energy(0), tof(0),
        sensDetID(0), history(0),
        mcParticle(-1)
    {}

    int   x,y,z;
    int   energy;
    int   tof;
    int   sensDetID;
    int   history;
    int   mcParticle;

  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMCRichHits = 1521;

  /// Namespace for locations in TDS
  namespace PackedMCRichHitLocation
  {
    static const std::string& Default = "pSim/Rich/Hits";
  }

  /** @class PackedMCRichHits Event/PackedMCRichHit.h
   *
   *  Packed MCRichHits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedMCRichHits : public DataObject
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedMCRichHit> Vector;

  public:

    /// Standard constructor
    PackedMCRichHits( ) { }

    /// Destructor
    virtual ~PackedMCRichHits( ) { }

    /// Class ID
    static const CLID& classID() { return CLID_PackedMCRichHits; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMCRichHits::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

  private:

    Vector m_vect; ///< The packed data objects

  };

  // -----------------------------------------------------------------------

  /** @class MCRichHitPacker Event/PackedMCRichHit.h
   *
   *  Utility class to handle the packing and unpacking of the MCRichHits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCRichHitPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::MCRichHit                    Data;
    typedef LHCb::PackedMCRichHit        PackedData;
    typedef LHCb::MCRichHits             DataVector;
    typedef LHCb::PackedMCRichHits PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedMCRichHitLocation::Default; }
    static const std::string& unpackedLocation() { return LHCb::MCRichHitLocation::Default; }

  public:

    /// Default Constructor
    MCRichHitPacker() {}

  public:

    /// Pack MCRichHits
    void pack( DataVector       & hits,
               PackedDataVector & phits,
               const unsigned int version );

    /// Unpack MCRichHits
    void unpack( PackedDataVector & phits,
                 DataVector       & hits );

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

#endif // EVENT_PACKEDMCRICHHIT_H
