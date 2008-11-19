// $Id: PackedProtoParticle.h,v 1.2 2008-11-19 10:10:49 ocallot Exp $
#ifndef EVENT_PACKEDPROTOPARTICLE_H 
#define EVENT_PACKEDPROTOPARTICLE_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <string>
#include <vector>

namespace LHCb {

  /** @class PackedProtoParticle PackedProtoParticle.h Event/PackedProtoParticle.h
   *  This is the packed version of the ProtoParticle
   *
   *  @author Olivier Callot
   *  @date   2008-11-13
   */
  struct PackedProtoParticle {

    /// Standard constructor
    PackedProtoParticle( ) {}; 

    PackedProtoParticle( const PackedProtoParticle& c ) :
      key( c.key ), track( c.track ), richPID( c.richPID ), muonPID( c.muonPID ),
      firstHypo( c.firstHypo ), lastHypo( c.lastHypo ),
      firstExtra( c.firstExtra ), lastExtra( c.lastExtra )
    {};

    int  key;
    int  track;
    int  richPID;
    int  muonPID;
    unsigned short int firstHypo;
    unsigned short int lastHypo;
    unsigned short int firstExtra;
    unsigned short int lastExtra;
  };
  
  static const CLID CLID_PackedProtoParticles = 1552;

  // Namespace for locations in TDS
  namespace PackedProtoParticleLocation {
    static const std::string& Charged = "pRec/ProtoP/Charged";
    static const std::string& Neutral = "pRec/Protop/Neutral";
  }

  class PackedProtoParticles : public DataObject {
  public: 
    /// Standard constructor
    PackedProtoParticles( ) { 
      m_vect.reserve(500);
      m_extra.reserve(5000);
    }; 

    virtual ~PackedProtoParticles( ) {}; ///< Destructor
    virtual const CLID& clID()  const { return PackedProtoParticles::classID(); }
    static  const CLID& classID()     { return CLID_PackedProtoParticles;       }

    void setVersion( int vers )       { m_version = vers; }
    int version()               const { return m_version; }

    void addEntry( PackedProtoParticle& obj ) { m_vect.push_back( obj ); }
    std::vector<PackedProtoParticle>::const_iterator begin() const { return m_vect.begin(); }
    std::vector<PackedProtoParticle>::const_iterator end()   const { return m_vect.end(); }

    void addRef( int i ) { m_refs.push_back( i ); }
    std::vector<int>::const_iterator beginRefs() const { return m_refs.begin(); }
    unsigned int  sizeRef()                      const { return m_refs.size(); }

    void addExtra( int a, int b ) { std::pair<int,int> tmp( a, b ); m_extra.push_back( tmp ); }
    std::vector<std::pair<int,int> >::const_iterator beginExtra() const { return m_extra.begin(); }
    unsigned int sizeExtra()                         const { return m_extra.size(); }

  protected:

  private:
    int m_version;
    std::vector<PackedProtoParticle> m_vect;
    std::vector<int>                 m_refs;
    std::vector<std::pair<int,int> > m_extra;
  };
};
#endif // EVENT_PACKEDPROTOPARTICLE_H
