// $Id: $
#ifndef EVENT_PACKEDRELATIONS_H
#define EVENT_PACKEDRELATIONS_H 1

// Kernel
#include "Event/StandardPacker.h"

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
    long long container{0};
    int start{0};
    int end{0};
  };
  
  // =================== Unweighted Relations =========================

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

    /// Class ID
    static const CLID& classID() { return CLID_PackedRelations; }

    /// Class ID
    virtual const CLID& clID() const { return PackedRelations::classID(); }

  public:
    
    std::vector<PackedRelation>&       relations()       { return m_relations; }
    const std::vector<PackedRelation>& relations() const { return m_relations; }

    std::vector<long long>&            sources()         { return m_source; }
    const std::vector<long long>&      sources()   const { return m_source; }

    std::vector<long long>&            dests()           { return m_dest; }
    const std::vector<long long>&      dests()     const { return m_dest; }

  private:

    std::vector<PackedRelation> m_relations;
    std::vector<long long>      m_source;
    std::vector<long long>      m_dest;
  
  };

  // =================== Weighted Relations =========================

  static const CLID CLID_PackedWeightedRelations = 1562;

  /// Namespace for locations in TDS
  namespace PackedWeightedRelationsLocation 
  {
    static const std::string& PP2MCP = "/pPhys/PP2MCPRelations";
  }

  /** @class PackedWeightedRelations PackedRelations.h Event/PackedRelations.h
   *
   *  Packed Weighted Relations
   *  
   *  @author Chris Jones
   *  @date   2015-04-13
   */
  class PackedWeightedRelations : public DataObject 
  {

  public:

    /// Class ID
    static const CLID& classID() { return CLID_PackedWeightedRelations; }

    /// Class ID
    virtual const CLID& clID() const { return PackedWeightedRelations::classID(); }

  public:
    
    std::vector<PackedRelation>&       relations()       { return m_relations; }
    const std::vector<PackedRelation>& relations() const { return m_relations; }

    std::vector<long long>&            sources()         { return m_source; }
    const std::vector<long long>&      sources()   const { return m_source; }

    std::vector<long long>&            dests()           { return m_dest; }
    const std::vector<long long>&      dests()     const { return m_dest; }

    std::vector<float>&                weights()         { return m_weights; }
    const std::vector<float>&          weights()   const { return m_weights; }

  private:

    std::vector<PackedRelation> m_relations;
    std::vector<long long>      m_source;
    std::vector<long long>      m_dest;
    std::vector<float>          m_weights;
  
  };

}

#endif // EVENT_PACKEDRELATIONS_H
