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
   
    /// Key and possibly container index.
    long long key{0}; 

    int technique{0};    ///< packed technique
    int chi2{0};         ///< packed chi^2
    int nDoF{0};         ///< packed nDOF

    // Position
    int x{0}, y{0}, z{0};

    // Covariance matrix
    int cov00{0}, cov11{0}, cov22{0};
    short int cov10{0}, cov20{0}, cov21{0};

    /// first outgoing particle
    unsigned int firstOutgoingPart{0};
    /// last outgoing particle
    unsigned int lastOutgoingPart{0};

    /// first info
    unsigned int firstInfo{0};
    /// last info
    unsigned int lastInfo{0};

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
    void addExtra( const int a, const int b ) { m_extra.emplace_back( ExtraInfo(a,b) ); }

    /// Write access the extra info
    ExtraInfoVector& extras()             { return m_extra; }

    /// Read access the extra info
    const ExtraInfoVector& extras() const { return m_extra; }

  private:

    /// Data packing version
    char m_packingVersion{ defaultPackingVersion() };

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
    VertexPacker() {}

  public:

    /// Default Constructor
    VertexPacker( const GaudiAlgorithm * parent ) : m_pack(parent) {}
    
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
    const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

    /// Safe sqrt ...
    inline double safe_sqrt( const double x ) const
    { return ( x > 0 ? std::sqrt(x) : 0.0 ); }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PACKEDVERTEX_H
