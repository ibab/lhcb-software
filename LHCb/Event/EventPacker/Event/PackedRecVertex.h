// $Id: PackedRecVertex.h,v 1.4 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PACKEDRECVERTEX_H
#define EVENT_PACKEDRECVERTEX_H 1

#include <string>
#include <vector>

#include "GaudiKernel/DataObject.h"

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/RecVertex.h"

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

  /** @class PackedRecVertices Event/PackedRecVertex.h
   *
   *  Container of packed RecVertex objects
   *
   *  @author Olivier Callot
   *  @date   2008-11-14
   */

  class PackedRecVertices : public DataObject 
  {

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 1; }

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedRecVertex> Vector;
  
  public:
  
    /// Standard constructor
    PackedRecVertices( ) : m_packingVersion(0)
    {
      m_vect.reserve    ( 5   );
      m_refs.reserve    ( 100 );
      m_weights.reserve ( 100 );
      m_extra.reserve   ( 250 );
    }

    virtual ~PackedRecVertices( ) {}; ///< Destructor
    virtual const CLID& clID()  const { return PackedRecVertices::classID(); }
    static  const CLID& classID()     { return CLID_PackedRecVertices;       }

  public:

    std::vector<PackedRecVertex>& vertices()                   { return m_vect; }
    const std::vector<PackedRecVertex>& vertices() const       { return m_vect; }

    std::vector<long long>& refs()                         { return m_refs; }
    const std::vector<long long>& refs() const             { return m_refs; }

    void addExtra( const int a, const int b ) { m_extra.push_back( std::make_pair(a,b) ); }
    std::vector<std::pair<int,int> >& extras()             { return m_extra; }
    const std::vector<std::pair<int,int> >& extras() const { return m_extra; }

    std::vector<short int>& weights()             { return m_weights; }
    const std::vector<short int>& weights() const { return m_weights; }

  public:

    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }

    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }

  private:

    std::vector<PackedRecVertex>     m_vect;
    std::vector<long long>           m_refs;
    std::vector<std::pair<int,int> > m_extra;
    std::vector<short int>           m_weights;

    /// Data packing version
    char m_packingVersion;

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
    RecVertexPacker( const GaudiAlgorithm & parent ) : m_pack(&parent) {}

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
    inline const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

    /// Safe sqrt ...
    inline double safe_sqrt( const double x ) const
    { return ( x > 0 ? std::sqrt(x) : 0.0 ); }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PACKEDRECVERTEX_H
