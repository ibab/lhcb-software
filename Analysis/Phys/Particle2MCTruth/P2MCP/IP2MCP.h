// $Id: IP2MCP.h,v 1.6 2009-06-26 12:13:44 jpalac Exp $
#ifndef P2MCP_IP2MCP_H 
#define P2MCP_IP2MCP_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// from LHCb
#include "Event/MCParticle.h"
// from DaVinciMCKernel
#include "Kernel/IParticle2MCAssociator.h"
// from Particle2MCTruth
#include "P2MCP/P2MCPTypes.h"

namespace LHCb
{
  class Particle;
}

/** @class IP2MCP IP2MCP.h P2MCP/IP2MCP.h
 *  
 *  Tool interface to generate weighted associations between one or many 
 *  LHCb::Particles and return them to the user in different formats. The
 *  user is in control of the resulting associations and the invoking interface
 *  methods should have no side-effects. TES access should be read-only.
 *  Interface implementations should not populate the TES with their results
 *  directly or indirectly, and should not have any runtime configuration
 *  that alters the result, except for the assumed TES location of the 
 *  LHCb::MCParticles for the relevant methods.
 * 
 *  @author Juan PALACIOS
 *  @date   2009-01-19
 */
class IP2MCP : virtual public IParticle2MCAssociator
{
public:

  /**
   * Calculate and return the weighted associations between an 
   * LHCb::Particle and some LHCb::MCParticles from a TES locaiton.
   * @param particle LHCb::Particle* to be associated
   * @return P2MCP::Types::FlatTrees  containing 
   * tree-sorted weighted relations between the 
   * LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual P2MCP::Types::FlatTrees
  relatedMCPs(const LHCb::Particle* particle) const = 0;

  /**
   *
   * Calculate and return the weighted associations between an 
   * LHCb::Particle and some LHCb::MCParticles from a TES locaiton.
   * @param particle LHCb::Particle* to be associated
   * @param mcParticleLocation TES location of LHCb::MCParticles to base the
   * association on.
   * @return P2MCP::Types::FlatTrees containing tree-sorted 
   * weighted relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual P2MCP::Types::FlatTrees
  relatedMCPs(const LHCb::Particle* particle,
              const std::string& mcParticleLocation) const = 0;


  /**
   *
   * Calculate and return the weighted associations between a container of
   * LHCb::Particles and a container of LHCb::MCParticles.
   * @param particles Container of LHCb::Particles to be associated
   * @param mcParticles Container of LHCb::MCParticles to base the
   * association on.
   * @return P2MCP::Types::FlatTrees containing tree-sorted
   * relations between the LHCb::Particles and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual P2MCP::Types::FlatTrees
  relatedMCPs(const LHCb::Particle* particle,
              const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  /**
   *
   * Calculate and return the weighted associations between a container of
   * LHCb::Particles and a container of LHCb::MCParticles.
   * @param particles Container of LHCb::Particles to be associated
   * @param mcParticles Container of LHCb::MCParticles to base the
   * association on.
   * @return P2MCP::Types::FlatTrees Relations table containing tree-sorted
   * relations between the LHCb::Particles and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual P2MCP::Types::FlatTrees
  relatedMCPs(const LHCb::Particle* particle,
              const LHCb::MCParticle::Container& mcParticles) const = 0;



  /**
   * Calculate the association between an LHCb::Particle and an
   * LHCb::MCParticle.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual bool 
  isMatched(const LHCb::Particle* particle, 
            const LHCb::MCParticle* mcParticle) const = 0;

protected:
  virtual ~IP2MCP(); ///< virtual and protected destructor

 public:
   // Return the interface ID
   static const InterfaceID& interfaceID();

};
#endif // P2MCP_IP2MCP_H
