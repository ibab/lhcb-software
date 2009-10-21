// $Id: PackedMCParticle.h,v 1.5 2009-10-21 16:40:16 jonrob Exp $
#ifndef EVENT_PACKEDMCPARTICLE_H 
#define EVENT_PACKEDMCPARTICLE_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <string>
#include <vector>

namespace LHCb 
{
  struct PackedMCParticle {
    PackedMCParticle()   {};

    PackedMCParticle( const PackedMCParticle& c ) : // copy constructor
      key( c.key), px( c.px), py(c.py), pz(c.pz), mass(c.mass), PID( c.PID),
      originVertex( c.originVertex ), endVertices( c.endVertices) {};
  
    int key;
    int px;
    int py;
    int pz;
    float mass;
    int PID;
    int originVertex;
    std::vector<int>endVertices;
  };

  static const CLID CLID_PackedMCParticles = 1510;

  // Namespace for locations in TDS
  namespace PackedMCParticleLocation {
    static const std::string& Default = "pSim/MCParticles";
  }

  /** @class PackedMCParticles PackedMCParticle.h Event/PackedMCParticle.h
   *  DataObject containing a vector of packed MCParticles
   *
   *  @author Olivier Callot
   *  @date   2005-03-18
   */
  class PackedMCParticles : public DataObject {
  public: 
    /// Standard constructor
    PackedMCParticles( ) {}; 

    virtual ~PackedMCParticles( ) {}; ///< Destructor
    virtual const CLID& clID() const { return PackedMCParticles::classID(); }
    static  const CLID& classID()    { return CLID_PackedMCParticles;       }

    void addEntry(  PackedMCParticle& obj ) { m_vect.push_back( obj ); }
    std::vector<PackedMCParticle>::const_iterator begin() const { return m_vect.begin(); }
    std::vector<PackedMCParticle>::const_iterator end()   const { return m_vect.end(); }

    void reserve( const unsigned int size ) { m_vect.reserve(size); }
    unsigned int size() const { return m_vect.size(); }

  private:

    std::vector<PackedMCParticle> m_vect;

  };
} // namespace LHCb

#endif // EVENT_PACKEDMCPARTICLES_H
