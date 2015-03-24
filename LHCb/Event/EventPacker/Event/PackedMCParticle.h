// $Id: PackedMCParticle.h,v 1.5 2009-10-21 16:40:16 jonrob Exp $
#ifndef EVENT_PACKEDMCPARTICLE_H
#define EVENT_PACKEDMCPARTICLE_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <string>
#include <vector>

namespace LHCb
{

  /** @class PackedMCParticle Event/PackedMCParticle.h
   *
   *  Packed LHCb::MCParticle
   *
   *  @author Olivier Callot
   *  @date   2005-03-18
   */
  struct PackedMCParticle
  {

    /// Default Constructor
    PackedMCParticle() :
      key(0), 
      px(0), py(0), pz(0), 
      mass(0), 
      PID(0),
      originVertex(-1),
      flags(0)
    {}

    /// copy constructor
    PackedMCParticle( const PackedMCParticle& c ) :
      key(c.key), 
      px(c.px), py(c.py), pz(c.pz), 
      mass(c.mass), 
      PID(c.PID),
      originVertex(c.originVertex), 
      endVertices(c.endVertices),
      flags(c.flags)
    {}

    int key;
    int px;
    int py;
    int pz;
    float mass;
    int PID;
    long long originVertex;
    std::vector<long long> endVertices;
    unsigned int flags;

  };

  static const CLID CLID_PackedMCParticles = 1510;

  // Namespace for locations in TDS
  namespace PackedMCParticleLocation
  {
    static const std::string& Default = "pSim/MCParticles";
  }

  /** @class PackedMCParticles Event/PackedMCParticle.h
   *
   *  DataObject containing a vector of packed MCParticles
   *
   *  @author Olivier Callot
   *  @date   2005-03-18
   */
  class PackedMCParticles : public DataObject
  {

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 1; }

  public:

    /// Standard constructor
    PackedMCParticles( ) : m_packingVersion(0) {}

    virtual ~PackedMCParticles( ) {}; ///< Destructor

  public:

    virtual const CLID& clID() const { return PackedMCParticles::classID(); }
    static  const CLID& classID()    { return CLID_PackedMCParticles;       }

  public:

    std::vector<PackedMCParticle>&       mcParts()       { return m_vect; }
    const std::vector<PackedMCParticle>& mcParts() const { return m_vect; }

  public:
    
    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }
    
    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }
    
  private:

    /// Packed MCParticles
    std::vector<PackedMCParticle> m_vect;

    /// Data packing version
    char   m_packingVersion;

  };

} // namespace LHCb

#endif // EVENT_PACKEDMCPARTICLES_H
