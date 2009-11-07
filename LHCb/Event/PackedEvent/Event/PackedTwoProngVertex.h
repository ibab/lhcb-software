// $Id: PackedTwoProngVertex.h,v 1.2 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PACKEDTWOPRONGVERTEX_H
#define EVENT_PACKEDTWOPRONGVERTEX_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <string>
#include <vector>

namespace LHCb {

  // Namespace for locations in TDS
  namespace PackedTwoProngVertexLocation {
    static const std::string& Default = "pRec/Vertex/V0";
  }
  /** @class PackedTwoProngVertex PackedTwoProngVertex.h Event/PackedTwoProngVertex.h
   *
   *
   *  @author Olivier Callot
   *  @date   2009-01-21
   */
  struct PackedTwoProngVertex {
    /// Standard constructor
    PackedTwoProngVertex( ) {}; 

    int key;
    int technique;
    int chi2;
    int nDoF;
    int x;
    int y;
    int z;

    int txA;
    int tyA;
    int pA;
    
    int txB;
    int tyB;
    int pB;
    
    int cov00;
    int cov11;
    int cov22;
    int cov33;
    int cov44;
    int cov55;
    int cov66;
    int cov77;
    int cov88;
    int cov99;


    short int cov10;
    short int cov20, cov21;
    short int cov30, cov31, cov32;
    short int cov40, cov41, cov42, cov43;
    short int cov50, cov51, cov52, cov53, cov54;
    short int cov60, cov61, cov62, cov63, cov64, cov65;
    short int cov70, cov71, cov72, cov73, cov74, cov75, cov76;
    short int cov80, cov81, cov82, cov83, cov84, cov85, cov86, cov87;

    unsigned short int firstTrack;
    unsigned short int lastTrack;
    unsigned short int firstInfo;
    unsigned short int lastInfo;
    unsigned short int firstPid;
    unsigned short int lastPid;
  };
  
  static const CLID CLID_PackedTwoProngVertices = 1554;


  class PackedTwoProngVertices : public DataObject {
  public:
    /// Standard constructor
    PackedTwoProngVertices( ){};

    virtual ~PackedTwoProngVertices( ) {}; ///< Destructor
    virtual const CLID& clID()  const { return PackedTwoProngVertices::classID(); }
    static  const CLID& classID()     { return CLID_PackedTwoProngVertices;       }

    void addEntry( PackedTwoProngVertex& obj ) { m_vect.push_back( obj ); }
    std::vector<PackedTwoProngVertex>::const_iterator begin() const { return m_vect.begin(); }
    std::vector<PackedTwoProngVertex>::const_iterator end()   const { return m_vect.end(); }
    std::vector<PackedTwoProngVertex>& vertices()                   { return m_vect; }
    const std::vector<PackedTwoProngVertex>& vertices() const       { return m_vect; }

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
    std::vector<PackedTwoProngVertex>     m_vect;
    std::vector<int>                 m_refs;
    std::vector<std::pair<int,int> > m_extra;
  };
} // End of LHCb namespace
#endif // EVENT_PACKEDTWOPRONGVERTEX_H
