// $Id: MCAssociation.h,v 1.4 2009-03-26 17:50:48 jpalac Exp $
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
class MCAssociation {
public: 
  /// Standard constructor
  MCAssociation( )
    :
    m_associatedMCP(0),
    m_weight(0.)
  {}

  MCAssociation(const LHCb::MCParticle* mcp, 
                const double weight)
    :
    m_associatedMCP(mcp),
    m_weight(weight)
  {
  }

  virtual ~MCAssociation() {}
  
  const LHCb::MCParticle* to() const { return m_associatedMCP; }

  const double& weight() const { return m_weight; }

  double& weight() { return m_weight; }

private:
  const LHCb::MCParticle* m_associatedMCP;
  double m_weight;

};
#endif // KERNEL_MCASSOCIATION_H
