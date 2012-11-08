// $Id: $
#ifndef EVENT_PACKEDRELATIONS_H
#define EVENT_PACKEDRELATIONS_H 1

// Kernel
#include "Kernel/StandardPacker.h"

// Include files
namespace LHCb 
{

  /** @class PackedRelation PackedRelations.h Event/PackedRelations.h
   *
   *  Packed Relation
   *  
   *  @author Olivier Callot
   *  @date   2012-01-24
   */
  class PackedRelation 
  {
  public:

    PackedRelation() : container(0), start(0), end(0) {}
    
    ~PackedRelation() {}
    
    long long container;
    int start;
    int end;
  };

  static const CLID CLID_PackedRelations = 1560;

  /// Namespace for locations in TDS
  namespace PackedRelationsLocation 
  {
    static const std::string& InStream = "/pPhys/Relations";
    static const std::string& P2MCP    = "/pPhys/P2MCPRelations";
    static const std::string& P2Int    = "/pPhys/P2IntRelations";
  }

  /** @class PackedRelations PackedRelations.h Event/PackedRelations.h
   *
   *  Packed Relations
   *  
   *  @author Olivier Callot
   *  @date   2012-01-24
   */
  class PackedRelations : public DataObject 
  {
  
  public:
  
    /// Standard constructor
    PackedRelations( ) {};

    virtual ~PackedRelations( ) {}; ///< Destructor

    /// Class ID
    static const CLID& classID() { return CLID_PackedRelations; }

    /// Class ID
    virtual const CLID& clID() const { return PackedRelations::classID(); }
    
    std::vector<PackedRelation>& relations() { return m_relations; }
    std::vector<long long>& sources()        { return m_source; }
    std::vector<long long>& dests()          { return m_dest; }

  private:

    std::vector<PackedRelation> m_relations;
    std::vector<long long> m_source;
    std::vector<long long> m_dest;
  
  };
}
#endif // EVENT_PACKEDRELATIONS_H
