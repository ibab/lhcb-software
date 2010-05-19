// $Id: PackedParticle.h,v 1.2 2010-05-19 09:04:08 jonrob Exp $
#ifndef EVENT_PACKEDPARTICLE_H
#define EVENT_PACKEDPARTICLE_H 1

#include <string>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/Particle.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

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
    /// Default constructor
    PackedParticle()
      : particleID(0)
    {}

    // packed data members
    int particleID; ///< PID Code

  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedParticles = 1581;

  /// Namespace for locations in TDS
  namespace PackedParticleLocation
  {
    static const std::string& User = "pPhys/User/Particles";
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

  public:

    /// Standard constructor
    PackedParticles( ) : m_packingVersion(0) { }

    /// Destructor
    virtual ~PackedParticles( ) { }

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

  private:

    /// Data packing version (not used as yet, but for any future schema evolution)
    char   m_packingVersion;

    /// The packed data objects
    Vector m_vect;

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

  public:

    /// Default Constructor
    ParticlePacker() {}

  public:

    /// Pack Particles
    void pack( const DataVector & parts,
               PackedDataVector & pparts ) const;

    /// Unpack Particles
    void unpack( const PackedDataVector & pparts,
                 DataVector             & parts ) const;

    /// Compare two Particles to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB,
                      GaudiAlgorithm & parent ) const;

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PACKEDPARTICLE_H
