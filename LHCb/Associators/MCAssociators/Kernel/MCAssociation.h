#ifndef KERNEL_MCASSOCIATION_H 
#define KERNEL_MCASSOCIATION_H 1

// Include files
namespace LHCb 
{
  class MCParticle;
}

/** @class MCAssociation MCAssociation.h Kernel/MCAssociation.h
 *  
 *  The class MCAssociation holds a single MCParticle and an association 
 *  weight relating it to something.
 *
 *  @author V. Gligorov
 *  @date   2007-11-07
 */
class MCAssociation final {
public: 
  MCAssociation( ) = default;

  MCAssociation(const LHCb::MCParticle* mcp, 
                const double weight)
    : m_associatedMCP(mcp), m_weight(weight)
  {
  }
  
  const LHCb::MCParticle* to() const { return m_associatedMCP; }

  const double& weight() const { return m_weight; }

  double& weight() { return m_weight; }

private:
  const LHCb::MCParticle* m_associatedMCP = nullptr;
  double m_weight = 0;

};
#endif // KERNEL_MCASSOCIATION_H
