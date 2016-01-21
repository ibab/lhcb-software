#ifndef EVENT_PACKEDMCHIT_H
#define EVENT_PACKEDMCHIT_H 1

#include <string>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/MCHit.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

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
    int   sensDetID{0};
    int   entx{0},enty{0},entz{0};
    int   vtxx{0},vtxy{0},vtxz{0};
    int   energy{0};
    int   tof{0};
    int   mp{0};
    long long mcParticle{-1};
  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMCHits = 1525;

  /// Namespace for locations in TDS
  namespace PackedMCHitLocation
  {
    static const std::string& Velo    = "pSim/Velo/Hits";
    static const std::string& PuVeto  = "pSim/PuVeto/Hits";
    static const std::string& VP      = "pSim/VP/Hits";
    static const std::string& TT      = "pSim/TT/Hits";
    static const std::string& UT      = "pSim/UT/Hits";
    static const std::string& IT      = "pSim/IT/Hits";
    static const std::string& SL    = "pSim/SL/Hits";
    static const std::string& OT      = "pSim/OT/Hits";
    static const std::string& FT      = "pSim/FT/Hits";
    static const std::string& Muon    = "pSim/Muon/Hits";
    static const std::string& HC      = "pSim/HC/Hits";
    static const std::string& Bcm     = "pSim/Bcm/Hits";
    static const std::string& Bls     = "pSim/Bls/Hits";
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
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 1; }

  public:

    /// Class ID
    static const CLID& classID() { return CLID_PackedMCHits; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMCHits::classID(); }

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

  private:

    /// Default Constructor hidden
    MCHitPacker( ) {}
    
  public:
    
    /// Constructor
    MCHitPacker( const GaudiAlgorithm & p ) : m_pack(&p) {}
    
  public:

    /// Pack MCHits
    void pack( const DataVector & hits,
               PackedDataVector & phits ) const;

    /// Unpack MCHits
    void unpack( const PackedDataVector & phits,
                 DataVector             & hits ) const;

    /// Compare two MCHits to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

  private:

    /// Access the parent algorithm
    const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  protected:

    /// Scale factor for displacement vector
    double m_dispScale{ 1.0e2 };

    /// Scale factor for energy
    double m_enScale{ 5.0e3 };

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
    MCVeloHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
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
    MCPuVetoHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::PuVeto; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::PuVeto;       }
  };

  /** @class MCVPHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC VP Hits
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCVPHitPacker : public MCHitPacker
  {
  public:
    MCVPHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    { 
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::VP; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::VP;       }
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
    MCTTHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::TT; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::TT;       }
  };

  /** @class MCUTHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC UT Hits
   *
   *  @author Jianchun Wang
   *  @date   2012-07-13
   */
  class MCUTHitPacker : public MCHitPacker
  {
  public:
    MCUTHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::UT; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::UT;       }
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
    MCITHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }    
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::IT; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::IT;       }
  };

  /** @class MCSLHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC IT Hits
   *
   *  @author Paul Szczypka
   *  @date   2013-04-24
   */
  class MCSLHitPacker : public MCHitPacker
  {
  public:
    MCSLHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }    
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::SL; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::SL;       }
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
    MCOTHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::OT; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::OT;       }
  };


  class MCFTHitPacker : public MCHitPacker
  {
  public:
    MCFTHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::FT; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::FT;       }
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
    MCMuonHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::Muon; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::Muon;       }
  };

  /** @class MCHCHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC HC Hits
   *
   *  @author Heinrich Schindler
   *  @date   2014-05-08
   */
  class MCHCHitPacker : public MCHitPacker
  {
  public:
    MCHCHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::HC; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::HC;       }
  };

  /** @class MCBcmHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC Bcm Hits
   *
   *  @author Heinrich Schindler
   *  @date   2015-02-12
   */
  class MCBcmHitPacker : public MCHitPacker
  {
  public:
    MCBcmHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::Bcm; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::Bcm;       }
  };

  /** @class MCBlsHitPacker Event/PackedMCHit.h
   *
   *  Utility class to handle the packing and unpacking of the MC Bls Hits
   *
   *  @author Heinrich Schindler
   *  @date   2015-02-12
   */
  class MCBlsHitPacker : public MCHitPacker
  {
  public:
    MCBlsHitPacker( const GaudiAlgorithm & parent ) : MCHitPacker(parent)
    {
      m_dispScale = 1.0e2;
      m_enScale   = 5.0e3;
    }
    static const std::string packedLocation()   { return LHCb::PackedMCHitLocation::Bls; }
    static const std::string unpackedLocation() { return LHCb::MCHitLocation::Bls;       }
  };

}

#endif // EVENT_PACKEDMCHIT_H
