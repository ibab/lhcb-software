// $Id: PackedMCCaloHit.h,v 1.1 2009-10-21 16:40:16 jonrob Exp $
#ifndef EVENT_PACKEDMCCALOHIT_H
#define EVENT_PACKEDMCCALOHIT_H 1

#include <string>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/MCCaloHit.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedMCCaloHit Event/PackedMCCaloHit.h
   *
   *  Packed MCCaloHit
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedMCCaloHit
  {
    /// Default constructor
    PackedMCCaloHit()
      : activeE(0),
        sensDetID(0),
        time(0),
        mcParticle(-1)
    {}

    int   activeE;
    int   sensDetID;
    char  time;
    int   mcParticle;

  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMCCaloHits = 1526;

  /// Namespace for locations in TDS
  namespace PackedMCCaloHitLocation
  {
    static const std::string& Spd  = "pSim/Spd/Hits";
    static const std::string& Prs  = "pSim/Prs/Hits";
    static const std::string& Ecal = "pSim/Ecal/Hits";
    static const std::string& Hcal = "pSim/Hcal/Hits";
  }

  /** @class PackedMCCaloHits Event/PackedMCCaloHit.h
   *
   *  Packed MCCaloHits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedMCCaloHits : public DataObject
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedMCCaloHit> Vector;

  public:

    /// Standard constructor
    PackedMCCaloHits( ) { }

    /// Destructor
    virtual ~PackedMCCaloHits( ) { }

    /// Class ID
    static const CLID& classID() { return CLID_PackedMCCaloHits; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMCCaloHits::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

  private:

    Vector m_vect; ///< The packed data objects

  };

  // -----------------------------------------------------------------------

  /** @class MCCaloHitPacker Event/PackedMCCaloHit.h
   *
   *  Utility class to handle the packing and unpacking of the MCCaloHits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCCaloHitPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::MCCaloHit                    Data;
    typedef LHCb::PackedMCCaloHit        PackedData;
    typedef LHCb::MCCaloHits             DataVector;
    typedef LHCb::PackedMCCaloHits PackedDataVector;
    // Since this class is used for different sub-systems, given invalid defaults
    // Users must explicitly set the options in the packers and unpacker.
    static const std::string packedLocation()   { return ""; }
    static const std::string unpackedLocation() { return ""; }

  public:

    /// Default Constructor
    MCCaloHitPacker() : m_energyScale( 1.0e2 ) { }

  public:

    /// Pack MCCaloHits
    void pack( DataVector       & hits,
               PackedDataVector & phits,
               const unsigned int version  );

    /// Unpack MCCaloHits
    void unpack( PackedDataVector & phits,
                 DataVector       & hits );

    /// Compare two MCCaloHits to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB,
                      GaudiAlgorithm & parent );

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  protected:

    /// Scale factor for energy
    double m_energyScale;

  };

  // -----------------------------------------------------------------------

  /** @class MCSpdHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC CALO Spd Hits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCSpdHitPacker : public MCCaloHitPacker
  {
  public:
    MCSpdHitPacker() : MCCaloHitPacker()
    {
      m_energyScale = 1.0e2;
    }
  };

  // -----------------------------------------------------------------------

  /** @class MCPrsHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC CALO Prs Hits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCPrsHitPacker : public MCCaloHitPacker
  {
  public:
    MCPrsHitPacker() : MCCaloHitPacker()
    {
      m_energyScale = 1.0e2;
    }
  };

  // -----------------------------------------------------------------------

  /** @class MCEcalHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC CALO Ecal Hits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCEcalHitPacker : public MCCaloHitPacker
  {
  public:
    MCEcalHitPacker() : MCCaloHitPacker()
    {
      m_energyScale = 1.0e2;
    }
  };

  // -----------------------------------------------------------------------

  /** @class MCHcalHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC CALO Hcal Hits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCHcalHitPacker : public MCCaloHitPacker
  {
  public:
    MCHcalHitPacker() : MCCaloHitPacker()
    {
      m_energyScale = 1.0e2;
    }
  };

}

#endif // EVENT_PACKEDMCCALOHIT_H
