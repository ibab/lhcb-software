// $Id: $
#ifndef EVENT_PACKEDPARTICLE2INTS_H
#define EVENT_PACKEDPARTICLE2INTS_H 1

// Include files
namespace LHCb
{

  /** @class PackedParticle2Int PackedParticle2Ints.h Event/PackedParticle2Ints.h
   *
   *  Packed Particle to int
   *
   *  @author Olivier Callot
   *  @date   2012-01-30
   */
  class PackedParticle2Int
  {
  public:
    long long key{0};
    long long container{0};
    int start{0};
    int end{0};
  };

  static const CLID CLID_PackedParticle2Ints = 1591;

  /// Namespace for locations in TES
  namespace PackedParticle2IntsLocation
  {
    static const std::string& InStream = "/pPhys/Particle2Int";
  }

  /** @class PackedParticle2Ints PackedParticle2Ints.h Event/PackedParticle2Ints.h
   *
   *  Packed Particle to int(s)
   *
   *  @author Olivier Callot
   *  @date   2012-01-30
   */
  class PackedParticle2Ints : public DataObject
  {

  public:

    /// Class ID
    static const CLID& classID() { return CLID_PackedParticle2Ints; }

    /// Class ID
    virtual const CLID& clID() const { return PackedParticle2Ints::classID(); }

  public:

    std::vector<PackedParticle2Int>& relations() { return m_relations; }

    std::vector<int>& ints() { return m_ints; }

  private:

    std::vector<PackedParticle2Int> m_relations;
    std::vector<int> m_ints;

  };
}

#endif // EVENT_PACKEDPARTICLE2INTS_H
