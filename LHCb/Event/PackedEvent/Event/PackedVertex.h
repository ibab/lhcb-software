// $Id: PackedVertex.h,v 1.2 2010-05-19 09:04:08 jonrob Exp $
#ifndef EVENT_PACKEDVERTEX_H
#define EVENT_PACKEDVERTEX_H 1

#include <string>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/Vertex.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedVertex Event/PackedVertex.h
   *
   *  Packed Vertex
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedVertex
  {

    /// Default constructor
    PackedVertex()
      : key(0),
        technique(0),
        chi2(0), nDoF(0),
        x(0), y(0), z(0),
        cov00(0), cov11(0), cov22(0),
        cov10(0), cov20(0), cov21(0),
        firstOutgoingPart(0), 
        lastOutgoingPart(0)
    {}
    
    /// Key and possibly container index.
    long long key; 

    /// packed technique
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

    /// first outgoing particle
    unsigned int firstOutgoingPart;

    /// last outgoing particle
    unsigned int lastOutgoingPart;
    unsigned int firstInfo,   lastInfo;

  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedVertices = 1582;

  /// Namespace for locations in TDS
  namespace PackedVertexLocation
  {
    static const std::string& User = "pPhys/User/Vertices";
    static const std::string& InStream = "/pPhys/Vertices";
  }

  /** @class PackedVertices Event/PackedVertex.h
   *
   *  Packed Vertices
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedVertices : public DataObject
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedVertex> Vector;

    /// Outgoing Particles
    typedef std::vector<long long> OutgoingParticles;

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 1; }

  public:

    /// Standard constructor
    PackedVertices( ) : m_packingVersion(defaultPackingVersion()) { }

    /// Destructor
    virtual ~PackedVertices( ) { }

    /// Class ID
    static const CLID& classID() { return CLID_PackedVertices; }

    /// Class ID
    virtual const CLID& clID() const { return PackedVertices::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

    /// Write access to the data vector
    OutgoingParticles & outgoingParticles()             { return m_parts; }

    /// Read access to the data vector
    const OutgoingParticles & outgoingParticles() const { return m_parts; }

    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }

    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }

    /// add an extra info
    void addExtra( int a, int b ) { std::pair<int,int> tmp( a, b ); m_extra.push_back( tmp ); }
    std::vector<std::pair<int,int> >& extras()             { return m_extra; }
    const std::vector<std::pair<int,int> >& extras() const { return m_extra; }

  private:

    /// Data packing version (not used as yet, but for any future schema evolution)
    char   m_packingVersion;

    /// The packed data objects
    Vector m_vect;

    /// Outgoing Particles
    OutgoingParticles m_parts;

    /// Extra info
    std::vector<std::pair<int,int> > m_extra;

  };

  // -----------------------------------------------------------------------

  /** @class VertexPacker Event/PackedVertex.h
   *
   *  Utility class to handle the packing and unpacking of the Vertices
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class VertexPacker
  {

  public:

    // These are required by the templated algorithms
    typedef LHCb::Vertex                     Data;
    typedef LHCb::PackedVertex         PackedData;
    typedef LHCb::Vertices             DataVector;
    typedef LHCb::PackedVertices PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedVertexLocation::User; }
    static const std::string& unpackedLocation() { return LHCb::VertexLocation::User; }

  public:

    /// Default Constructor
    VertexPacker() {}

  public:

    /// Pack a Vertex
    void pack( const Data & vert,
               PackedData & pvert,
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

    /// Compare two Vertices to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB,
                      GaudiAlgorithm & parent ) const;

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PACKEDVERTEX_H
