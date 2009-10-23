// $Id: PackedMCHit.h,v 1.3 2009-10-23 10:46:17 jonrob Exp $
#ifndef EVENT_PACKEDMCHIT_H
#define EVENT_PACKEDMCHIT_H 1

#include <string>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/MCHit.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedMCHit Event/PackedMCHit.h
   *
   *  Packed MCHit
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedMCHit
  {
    /// Default constructor
    PackedMCHit()
      : sensDetID(0),
        entx(0),enty(0),entz(0),
        vtxx(0),vtxy(0),vtxz(0),
        energy(0), tof(0),
        mp(0),
        mcParticle(-1)
    {}

    int   sensDetID;
    int   entx,enty,entz;
    int   vtxx,vtxy,vtxz;
    int   energy;
    int   tof;
    int   mp;
    int   mcParticle;

  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMCHits = 1525;

  /// Namespace for locations in TDS
  namespace PackedMCHitLocation
  {
    static const std::string& Velo   = "pSim/Velo/Hits";
    static const std::string& PuVeto = "pSim/PuVeto/Hits";
    static const std::string& TT     = "pSim/TT/Hits";
    static const std::string& IT     = "pSim/IT/Hits";
    static const std::string& OT     = "pSim/OT/Hits";
    static const std::string& Muon   = "pSim/Muon/Hits";
  }

  /** @class PackedMCHits Event/PackedMCHit.h
   *
   *  Packed MCHits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedMCHits : public DataObject
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedMCHit> Vector;

  public:

    /// Standard constructor
    PackedMCHits( ) { }

    /// Destructor
    virtual ~PackedMCHits( ) { }

    /// Class ID
    static const CLID& classID() { return CLID_PackedMCHits; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMCHits::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

  private:

    Vector m_vect; ///< The packed data objects

  };

  // -----------------------------------------------------------------------

  /** @class MCHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MCHits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCHitPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::MCHit                    Data;
    typedef LHCb::PackedMCHit        PackedData;
    typedef LHCb::MCHits             DataVector;
    typedef LHCb::PackedMCHits PackedDataVector;

  public:

    /// Default Constructor
    MCHitPacker() 
      : m_dispScale ( 1.0e2 ),
        m_enScale   ( 5.0e3 ) {}
    
  public:

    /// Pack MCHits
    void pack( DataVector       & hits,
               PackedDataVector & phits,
               const unsigned int version ) const;

    /// Unpack MCHits
    void unpack( PackedDataVector & phits,
                 DataVector       & hits ) const;

    /// Compare two MCHits to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB,
                      GaudiAlgorithm & parent ) const;

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  protected:

    /// Scale factor for displacement vector
    double m_dispScale;

    /// Scale factor for energy
    double m_enScale;

  };

  // -----------------------------------------------------------------------

  /** @class MCVeloHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC Velo Hits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCVeloHitPacker : public MCHitPacker
  {
  public:
    MCVeloHitPacker() : MCHitPacker()
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::Velo; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::Velo;       }
  };

  /** @class MCPuVetoHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC PuVeto Hits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCPuVetoHitPacker : public MCHitPacker
  {
  public:
    MCPuVetoHitPacker() : MCHitPacker()
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::PuVeto; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::PuVeto;       }
  };

  /** @class MCTTHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC TT Hits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCTTHitPacker : public MCHitPacker
  {
  public:
    MCTTHitPacker() : MCHitPacker()
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::TT; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::TT;       }
  };

  /** @class MCITHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC IT Hits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCITHitPacker : public MCHitPacker
  {
  public:
    MCITHitPacker() : MCHitPacker()
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }    
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::IT; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::IT;       }
  };

  /** @class MCOTHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC OT Hits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCOTHitPacker : public MCHitPacker
  {
  public:
    MCOTHitPacker() : MCHitPacker()
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::OT; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::OT;       }
  };

  /** @class MCMuonHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC Muon Hits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCMuonHitPacker : public MCHitPacker
  {
  public:
    MCMuonHitPacker() : MCHitPacker()
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::Muon; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::Muon;       }
  };

}

#endif // EVENT_PACKEDMCHIT_H
