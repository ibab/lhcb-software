// $Id: PackedRecVertex.h,v 1.4 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PACKEDRECVERTEX_H
#define EVENT_PACKEDRECVERTEX_H 1

#include <string>
#include <vector>

#include "GaudiKernel/DataObject.h"

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/RecVertex.h"

class GaudiAlgorithm;

namespace LHCb
{

  // -----------------------------------------------------------------------

  /** @class PackedRecVertex Event/PackedRecVertex.h
   *
   *  Structure to describe a reconstructed vertex
   *
   *  @author Olivier Callot
   *  @date   2008-11-14
   */
  struct PackedRecVertex
  {

    /// Standard constructor
    PackedRecVertex( ) :
      key(0), technique(0), chi2(0), nDoF(0),
      x(0), y(0), z(0),
      cov00(0), cov11(0), cov22(0),
      cov10(0), cov20(0), cov21(0),
      firstTrack(0), lastTrack(0),
      firstInfo(0), lastInfo(0),
      container(0)
    {}

    /// Copy constructor
    PackedRecVertex( const PackedRecVertex& c ) :
      key( c.key ), technique( c.technique ), 
      chi2( c.chi2 ), nDoF( c.nDoF ),
      x( c.x ), y( c.y ), z( c.z ),
      cov00( c.cov00 ), cov11( c.cov11 ), cov22( c.cov22 ),
      cov10( c.cov10 ), cov20( c.cov20 ), cov21( c.cov21 ),
      firstTrack( c.firstTrack ), lastTrack( c.lastTrack ),
      firstInfo( c.firstInfo ), lastInfo( c.lastInfo ),
      container( c.container )
    {}

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
    unsigned short int firstTrack,  lastTrack;
    unsigned short int firstInfo,   lastInfo;
    int container;

  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedRecVertices = 1553;

  // Namespace for locations in TDS
  namespace PackedRecVertexLocation
  {
    static const std::string& Primary = "pRec/Vertex/Primary";
    static const std::string& InStream = "/pPhys/RecVertices";
  }

  class PackedRecVertices : public DataObject 
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedRecVertex> Vector;
  
  public:
  
    /// Standard constructor
    PackedRecVertices( ) 
    {
      m_vect.reserve(10);
      m_refs.reserve(200);
      m_extra.reserve(500);
    }

    virtual ~PackedRecVertices( ) {}; ///< Destructor
    virtual const CLID& clID()  const { return PackedRecVertices::classID(); }
    static  const CLID& classID()     { return CLID_PackedRecVertices;       }

    void addEntry( PackedRecVertex& obj ) { m_vect.push_back( obj ); }
    std::vector<PackedRecVertex>& vertices()                   { return m_vect; }
    const std::vector<PackedRecVertex>& vertices() const       { return m_vect; }

    void addRef( int i ) { m_refs.push_back( i ); }
    /// Avoid hidden method
    virtual unsigned long addRef() { return DataObject::addRef(); }
    std::vector<int>& refs()                           { return m_refs; }
    const std::vector<int>& refs() const               { return m_refs; }

    void addExtra( int a, int b ) { std::pair<int,int> tmp( a, b ); m_extra.push_back( tmp ); }
    std::vector<std::pair<int,int> >& extras()             { return m_extra; }
    const std::vector<std::pair<int,int> >& extras() const { return m_extra; }

    void addWeight( const short int weight ) { m_weights.push_back( weight ); }
    std::vector<short int>& weights()             { return m_weights; }
    const std::vector<short int>& weights() const { return m_weights; }

  private:

    std::vector<PackedRecVertex>     m_vect;
    std::vector<int>                 m_refs;
    std::vector<std::pair<int,int> > m_extra;
    std::vector<short int>           m_weights;

  };

  // -----------------------------------------------------------------------

  /** @class RecVertexPacker Event/PackedRecVertex.h
   *
   *  Utility class to handle the packing and unpacking of the RecVertices
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class RecVertexPacker
  {

  public:

    // These are required by the templated algorithms
    typedef LHCb::RecVertex                     Data;
    typedef LHCb::PackedRecVertex         PackedData;
    typedef LHCb::RecVertices             DataVector;
    typedef LHCb::PackedRecVertices PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedRecVertexLocation::Primary; }
    static const std::string& unpackedLocation() { return LHCb::RecVertexLocation::Primary; }

  private:

    /// Default Constructor hidden
    RecVertexPacker() {}

  public:

    /// Constructor
    RecVertexPacker( GaudiAlgorithm & parent ) : m_parent(&parent) {}

  public:

    /// Pack a Vertex
    void pack( const Data & vert,
               PackedData & pvert,
               const DataVector & verts, 
               PackedDataVector & pverts ) const;

    /// Pack Vertices
    void pack( const DataVector & verts,
               PackedDataVector & pverts ) const;

    /// Unpack a Vertex
    void unpack( const PackedData       & pvert,
                 Data                   & vert,
                 const PackedDataVector & pverts,
                 DataVector             & verts ) const;

    /// Unpack Vertices
    void unpack( const PackedDataVector & pverts,
                 DataVector             & verts ) const;

  private:

    /// Access the parent algorithm
    GaudiAlgorithm& parent() const { return *m_parent; }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

    /// Pointer to parent algorithm
    GaudiAlgorithm * m_parent;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PACKEDRECVERTEX_H
