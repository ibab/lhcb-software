// $Id: PackedParticle.h,v 1.2 2010-05-19 09:04:08 jonrob Exp $
#ifndef EVENT_PACKEDPARTICLE_H
#define EVENT_PACKEDPARTICLE_H 1

#include <string>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/Particle.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedParticle Event/PackedParticle.h
   *
   *  Packed Particle
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedParticle
  {

    // packed data members
    long long key{0};         ///< reference to the original container + key of the particle
    int particleID{0};        ///< PID Code
    int measMass{0};          ///< Measured mass
    int measMassErr{0};       ///< Error on the measured mass
    int lv_px{0},lv_py{0},lv_pz{0}; ///< 3D Momemtum part of Lorentz vector
    float lv_mass{0};         ///< Mass part of Lorentz vector 
    int refx{0},refy{0},refz{0};    ///< reference point

    // Momentum Cov matrix
    int momCov00{0},momCov11{0},momCov22{0},momCov33{0}; 
    short int momCov10{0};
    short int momCov20{0},momCov21{0};
    short int momCov30{0},momCov31{0},momCov32{0};

    // Position Cov matrix
    int posCov00{0},posCov11{0},posCov22{0}; 
    short int posCov10{0};
    short int posCov20{0},posCov21{0};

    // PosMom Cov matrix
    int pmCov00{0},pmCov01{0},pmCov02{0};
    int pmCov10{0},pmCov11{0},pmCov12{0};
    int pmCov20{0},pmCov21{0},pmCov22{0};
    int pmCov30{0},pmCov31{0},pmCov32{0};

    // Extra info
    unsigned int firstExtra{0}, lastExtra{0};

    // End Vertex
    long long vertex{-1};  

    // ProtoParticle
    long long proto{-1};

    // daughters
    unsigned int firstDaughter{0}, lastDaughter{0};  

  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedParticles = 1581;

  /// Namespace for locations in TDS
  namespace PackedParticleLocation
  {
    static const std::string& User     =  "pPhys/User/Particles";
    static const std::string& InStream = "/pPhys/Particles";
  }

  /** @class PackedParticles Event/PackedParticle.h
   *
   *  Packed Particles
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedParticles : public DataObject
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedParticle> Vector;

    /// Extra info pair
    typedef std::pair<int,int> PackedExtraInfo;

    /// Extra info vector
    typedef std::vector<PackedExtraInfo> PackedExtraInfoVector;

    /// Daughters
    typedef std::vector<long long> Daughters;

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 1; }

  public:

    /// Class ID
    static const CLID& classID() { return CLID_PackedParticles; }

    /// Class ID
    virtual const CLID& clID() const { return PackedParticles::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }

    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }

    /// Write access to the extra info
    PackedExtraInfoVector & extra()             { return m_extra; }

    /// Read access to the extra info
    const PackedExtraInfoVector & extra() const { return m_extra; }

    /// Write access to the daughters
    Daughters & daughters()             { return m_daughters; }

    /// Read access to the extra info
    const Daughters & daughters() const { return m_daughters; }

  private:

    /// Data packing version
    char   m_packingVersion{ defaultPackingVersion() };

    /// The packed data objects
    Vector m_vect;

    /// The extra info 
    PackedExtraInfoVector m_extra;

    /// Vector of packed daughter smartrefs
    Daughters m_daughters;

  };

  // -----------------------------------------------------------------------

  /** @class ParticlePacker Event/PackedParticle.h
   *
   *  Utility class to handle the packing and unpacking of the Particles
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class ParticlePacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::Particle                    Data;
    typedef LHCb::PackedParticle        PackedData;
    typedef LHCb::Particles             DataVector;
    typedef LHCb::PackedParticles PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedParticleLocation::User; }
    static const std::string& unpackedLocation() { return LHCb::ParticleLocation::User; }

  private:

    /// Default Constructor hidden
    ParticlePacker() {}

  public:

    /// Default Constructor
    ParticlePacker( const GaudiAlgorithm * p ) : m_pack(p) {}

  public:

    /// Pack a single Particle
    void pack( const Data & part,
               PackedData & ppart,
               PackedDataVector & pparts ) const;

    /// Pack Particles
    void pack( const DataVector & parts,
               PackedDataVector & pparts ) const;

    /// Unpack a single Particle
    void unpack( const PackedData       & ppart,
                 Data                   & part,
                 const PackedDataVector & pparts,
                 DataVector             & parts ) const;

    /// Unpack Particles
    void unpack( const PackedDataVector & pparts,
                 DataVector             & parts ) const;

    /// Compare two Particle vectors to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

    /// Compare two Particles to check the packing -> unpacking performance
    StatusCode check( const Data & dataA,
                      const Data & dataB ) const;

  private:

    /// Access the parent algorithm
    const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

    /// Safe sqrt ...
    inline double safe_sqrt( const double x ) const
    { return ( x > 0 ? std::sqrt(x) : 0.0 ); }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PACKEDPARTICLE_H
