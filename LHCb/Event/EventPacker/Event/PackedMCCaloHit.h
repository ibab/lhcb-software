// $Id: PackedMCCaloHit.h,v 1.5 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PACKEDMCCALOHIT_H
#define EVENT_PACKEDMCCALOHIT_H 1

#include <string>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/MCCaloHit.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

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
    int       activeE{0};
    int       sensDetID{0};
    char      time{0};
    long long mcParticle{-1};
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
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 1; }

  public:

    /// Class ID
    static const CLID& classID() { return CLID_PackedMCCaloHits; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMCCaloHits::classID(); }

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

  private:

    /// Default Constructor hidden
    MCCaloHitPacker() { }

  public:

    /// Constructor
    MCCaloHitPacker( const GaudiAlgorithm & p ) : m_pack(&p) { }

  public:

    /// Pack MCCaloHits
    void pack( const DataVector & hits,
               PackedDataVector & phits ) const;

    /// Unpack MCCaloHits
    void unpack( const PackedDataVector & phits,
                 DataVector             & hits ) const;

    /// Compare two MCCaloHits to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

  private:

    /// Access the parent algorithm
    const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  protected:

    /// Scale factor for energy
    double m_energyScale{ 1.0e2 };

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
    MCSpdHitPacker(GaudiAlgorithm & parent) : MCCaloHitPacker(parent)
    {
      m_energyScale = 1.0e2;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCCaloHitLocation::Spd; }
    static const std::string unpackedLocation() { return LHCb::MCCaloHitLocation::Spd;       }
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
    MCPrsHitPacker(GaudiAlgorithm & parent) : MCCaloHitPacker(parent)
    {
      m_energyScale = 1.0e2;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCCaloHitLocation::Prs; }
    static const std::string unpackedLocation() { return LHCb::MCCaloHitLocation::Prs;       }
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
    MCEcalHitPacker(GaudiAlgorithm & parent) : MCCaloHitPacker(parent)
    {
      m_energyScale = 1.0e2;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCCaloHitLocation::Ecal; }
    static const std::string unpackedLocation() { return LHCb::MCCaloHitLocation::Ecal;       }
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
    MCHcalHitPacker(GaudiAlgorithm & parent) : MCCaloHitPacker(parent)
    {
      m_energyScale = 1.0e2;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCCaloHitLocation::Hcal; }
    static const std::string unpackedLocation() { return LHCb::MCCaloHitLocation::Hcal;       }
  };

}

#endif // EVENT_PACKEDMCCALOHIT_H
