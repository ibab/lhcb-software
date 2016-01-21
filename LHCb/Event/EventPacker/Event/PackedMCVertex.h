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
    int key{0};
    int x{0};
    int y{0};
    int z{0};
    float tof{0};
    int type{0};
    long long mother{-1};
    std::vector<long long> products;
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
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 1; }

  public:

    virtual const CLID& clID() const { return PackedMCVertices::classID(); }
    static  const CLID& classID()    { return CLID_PackedMCVertices;       }

  public:

    std::vector<PackedMCVertex>&       mcVerts()       { return m_vect; }
    const std::vector<PackedMCVertex>& mcVerts() const { return m_vect; }

  public:
    
    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }
    
    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }

  private:

    std::vector<PackedMCVertex> m_vect;

    /// Data packing version
    char   m_packingVersion{0};

  };

} // namespace LHCb

#endif // EVENT_PACKEDMCVERTEX_H
