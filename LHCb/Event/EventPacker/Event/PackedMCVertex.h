// $Id: PackedMCVertex.h,v 1.6 2009-10-30 17:20:13 jonrob Exp $
#ifndef EVENT_PACKEDMCVERTEX_H
#define EVENT_PACKEDMCVERTEX_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <string>
#include <vector>

namespace LHCb
{

  /** @struct PackedMCVertex Event/PackedMCVertex.h
   *
   *  Packed LHCb::MCVertex
   *
   *  @author Olivier Callot
   *  @date   2005-03-18
   */

  struct PackedMCVertex
  {

    /// Default Constructor
    PackedMCVertex() :
      key(0), x(0), y(0), z(0), tof(0), type(0),
      mother(-1)
    {}

    /// Copy Constructor
    PackedMCVertex( const PackedMCVertex& c ) :
      key( c.key), x( c.x), y(c.y), z(c.z), tof(c.tof), type( c.type),
      mother( c.mother), products( c.products)
    {}

    int key;
    int x;
    int y;
    int z;
    float tof;
    int type;
    int mother;
    std::vector<int> products;
  };

  static const CLID CLID_PackedMCVertices = 1511;

  // Namespace for locations in TDS
  namespace PackedMCVertexLocation
  {
    static const std::string& Default = "pSim/MCVertices";
  }

  /** @class PackedMCVertices Event/PackedMCVertex.h
   *
   *  DataObject containing a vector of packed MCVertices
   *
   *  @author Olivier Callot
   *  @date   2005-03-18
   */

  class PackedMCVertices : public DataObject
  {

  public:

    /// Standard constructor
    PackedMCVertices( ) {}

    virtual ~PackedMCVertices( ) {}; ///< Destructor
    virtual const CLID& clID() const { return PackedMCVertices::classID(); }
    static  const CLID& classID()    { return CLID_PackedMCVertices;       }

    void addEntry(  PackedMCVertex& obj ) { m_vect.push_back( obj ); }
    std::vector<PackedMCVertex>::const_iterator begin() const { return m_vect.begin(); }
    std::vector<PackedMCVertex>::const_iterator end()   const { return m_vect.end(); }

    void reserve( const unsigned int size ) { m_vect.reserve(size); }
    unsigned int size() const { return m_vect.size(); }

  private:

    std::vector<PackedMCVertex> m_vect;

  };

} // namespace LHCb

#endif // EVENT_PACKEDMCVERTEX_H
