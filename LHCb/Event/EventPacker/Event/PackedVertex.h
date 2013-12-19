// $Id: PackedVertex.h,v 1.2 2010-05-19 09:04:08 jonrob Exp $
#ifndef EVENT_PACKEDVERTEX_H
#define EVENT_PACKEDVERTEX_H 1

#include <string>

// Kernel
#include "Event/StandardPacker.h"

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
        lastOutgoingPart(0),
        firstInfo(0), lastInfo(0)
    {}
    
    /// Key and possibly container index.
    long long key; 

    int technique;    ///< packed technique
    int chi2;         ///< packed chi^2
    int nDoF;         ///< packed nDOF

    // Position
    int x, y, z;

    // Covariance matrix
    int cov00, cov11, cov22;
    short int cov10, cov20, cov21;

    /// first outgoing particle
    unsigned int firstOutgoingPart;
    /// last outgoing particle
    unsigned int lastOutgoingPart;

    /// first info
    unsigned int firstInfo;
    /// last info
    unsigned int lastInfo;

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

    /// Packed Extra Info
    typedef std::pair<int,int> ExtraInfo;

    /// Packed Extra Info Vector
    typedef std::vector<ExtraInfo> ExtraInfoVector;

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
    void addExtra( const int a, const int b ) { m_extra.push_back( ExtraInfo(a,b) ); }

    /// Write access the extra info
    ExtraInfoVector& extras()             { return m_extra; }

    /// Read access the extra info
    const ExtraInfoVector& extras() const { return m_extra; }

  private:

    /// Data packing version
    char m_packingVersion;

    /// The packed data objects
    Vector m_vect;

    /// Outgoing Particles
    OutgoingParticles m_parts;

    /// Extra info
    ExtraInfoVector m_extra;

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

  private:

    /// Default Constructor hidden
    VertexPacker() : m_parent(NULL) {}

  public:

    /// Default Constructor
    VertexPacker( GaudiAlgorithm & parent ) : m_parent(&parent) {}

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

    /// Compare two Vertex vectors to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

    /// Compare two Vertices to check the packing -> unpacking performance
    StatusCode check( const Data & dataA,
                      const Data & dataB ) const;

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

#endif // EVENT_PACKEDVERTEX_H
