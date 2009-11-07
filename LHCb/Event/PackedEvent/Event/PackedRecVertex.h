// $Id: PackedRecVertex.h,v 1.4 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PACKEDRECVERTEX_H 
#define EVENT_PACKEDRECVERTEX_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <string>
#include <vector>

namespace LHCb {
  
  /** @class PackedRecVertex PackedRecVertex.h Event/PackedRecVertex.h
   *  Structure to describe a reconstructed vertex
   *
   *  @author Olivier Callot
   *  @date   2008-11-14
   */
  struct PackedRecVertex {
    /// Standard constructor
    PackedRecVertex( ) {}; 

    PackedRecVertex( const PackedRecVertex& c ) : // copy constructor
      key( c.key ), technique( c.technique ), chi2( c.chi2 ), nDoF( c.nDoF ),
      x( c.x ), y( c.y ), z( c.z ),
      cov00( c.cov00 ), cov11( c.cov11 ), cov22( c.cov22 ), 
      cov10( c.cov10 ), cov20( c.cov20 ), cov21( c.cov21 ),
      firstTrack( c.firstTrack ), lastTrack( c.lastTrack ),
      firstInfo( c.firstInfo ), lastInfo( c.lastInfo )
    {};
      
    int key;
    int technique;
    int chi2;
    int nDoF;
    int x;
    int y;
    int z;
    int cov00;
    int cov11;
    int cov22;
    short int cov10;
    short int cov20;
    short int cov21;

    unsigned short int firstTrack;
    unsigned short int lastTrack;
    unsigned short int firstInfo;
    unsigned short int lastInfo;
  };
  
  static const CLID CLID_PackedRecVertices = 1553;

  // Namespace for locations in TDS
  namespace PackedRecVertexLocation {
    static const std::string& Primary = "pRec/Vertex/Primary";
  }

  class PackedRecVertices : public DataObject {
  public: 
    /// Standard constructor
    PackedRecVertices( ) { 
      m_vect.reserve(10);
      m_refs.reserve(200);
      m_extra.reserve(500);
    }; 

    virtual ~PackedRecVertices( ) {}; ///< Destructor
    virtual const CLID& clID()  const { return PackedRecVertices::classID(); }
    static  const CLID& classID()     { return CLID_PackedRecVertices;       }

    void addEntry( PackedRecVertex& obj ) { m_vect.push_back( obj ); }
    std::vector<PackedRecVertex>::const_iterator begin() const { return m_vect.begin(); }
    std::vector<PackedRecVertex>::const_iterator end()   const { return m_vect.end(); }
    std::vector<PackedRecVertex>& vertices()                   { return m_vect; }
    const std::vector<PackedRecVertex>& vertices() const       { return m_vect; }

    void addRef( int i ) { m_refs.push_back( i ); }
    std::vector<int>::const_iterator beginRefs() const { return m_refs.begin(); }
    unsigned int  sizeRefs()                     const { return m_refs.size(); }
    std::vector<int>& refs()                           { return m_refs; }
    const std::vector<int>& refs() const               { return m_refs; }

    void addExtra( int a, int b ) { std::pair<int,int> tmp( a, b ); m_extra.push_back( tmp ); }
    std::vector<std::pair<int,int> >::const_iterator beginExtra() const { return m_extra.begin(); }
    unsigned int sizeExtra()                         const { return m_extra.size(); }
    std::vector<std::pair<int,int> >& extras()             { return m_extra; }
    const std::vector<std::pair<int,int> >& extras() const { return m_extra; }

  private:
    std::vector<PackedRecVertex>     m_vect;
    std::vector<int>                 m_refs;
    std::vector<std::pair<int,int> > m_extra;
  };
};
#endif // EVENT_PACKEDRECVERTEX_H
