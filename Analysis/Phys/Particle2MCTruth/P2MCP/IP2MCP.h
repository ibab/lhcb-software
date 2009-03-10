// $Id: IP2MCP.h,v 1.2 2009-03-10 18:29:38 jpalac Exp $
#ifndef P2MCP_IP2MCP_H 
#define P2MCP_IP2MCP_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// from DaVinciMCKernel
#include "P2MCP/P2MCPTypes.h"

namespace LHCb
{
  class Particle;
  class MCParticle;
}


static const InterfaceID IID_IP2MCP ( "IP2MCP", 1, 0 );

/** @class IP2MCP IP2MCP.h Kernel/IP2MCP.h
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
class IP2MCP : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IP2MCP; }

  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles from the TES. The location handling is defined
   * by the implementation.
   * @param particle LHCb::Particle* to be associated
   * @return P2MCPTypes::ToVector containing sorted weighted associations
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual P2MCPTypes::ToVector 
  associate(const LHCb::Particle* particle) const = 0;
  /**
   *
   * Calculate the weighted associations between an LHCb::Particle and
   * and some LHCb::MCParticles from a TES locaiton.
   * Return the result as P2MCPTypes::ToVector
   * @param particle LHCb::Particle* to be associated
   * @param mcParticleLocation TES location of LHCb::MCParticles to base the
   * association on.
   * @return P2MCPTypes::ToVector containing sorted weighted associations
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual P2MCPTypes::ToVector 
  associate(const LHCb::Particle* particle,
            const std::string& mcParticleLocation) const = 0;

  /**
   * Calculate and return the weighted associations between an 
   * LHCb::Particle and some LHCb::MCParticles from a TES locaiton.
   * @param particle LHCb::Particle* to be associated
   * @return P2MCPTypes::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual P2MCPTypes::LightTable 
  relatedMCPs(const LHCb::Particle*) const = 0;

  /**
   *
   * Calculate and return the weighted associations between an 
   * LHCb::Particle and some LHCb::MCParticles from a TES locaiton.
   * @param particle LHCb::Particle* to be associated
   * @param mcParticleLocation TES location of LHCb::MCParticles to base the
   * association on.
   * @return P2MCPTypes::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual P2MCPTypes::LightTable 
  relatedMCPs(const LHCb::Particle*,
              const std::string& mcParticleLocation) const = 0;


  /**
   *
   * Calculate and return the weighted associations between a container of
   * LHCb::Particles and a container of LHCb::MCParticles.
   * @param particles Container of LHCb::Particles to be associated
   * @param mcParticles Container of LHCb::MCParticles to base the
   * association on.
   * @return P2MCPTypes::LightTable Relations table containing 
   * sorted weighted relations between the LHCb::Particles and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual P2MCPTypes::LightTable 
  relatedMCPs(const LHCb::Particle* particles,
              const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  /**
   *
   * Calculate and return the weighted associations between a container of  
   * LHCb::Particle and a container of LHCb::MCParticles on the TES
   * The TES location of the LHCb::MCParticles is handled by the 
   * implementations
   * @param particles Container of LHCb::Particles to be associated
   * @return P2MCPTypes::LightTable Relations table containing 
   * relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::ConstVector& particles) const = 0;

  /**
   *
   * Calculate and return the associations between a container of
   * LHCb::Particles and a container of LHCb::MCParticles on the TES
   * @param particles Container of LHCb::Particles to be associated
   * @param mcParticleLocation TES location of LHCb::MCParticles to base the
   * association on.
   * @return P2MCPTypes::LightTable Relations table containing 
   * relations between the LHCb::Particles and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::ConstVector& particles,
               const std::string& mcParticleLocation) const = 0;

  /**
   *
   * Calculate and return the associations between a container of  
   * LHCb::Particle and a container of LHCb::MCParticles on the TES
   * The TES location of the LHCb::MCParticles is handled by the 
   * implementations
   * @param particles Container of LHCb::Particles to be associated
   * @return P2MCPTypes::LightTable Relations table containing 
   * relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::ConstVector& particles,
               const LHCb::MCParticle::ConstVector& mcParticles) const = 0;

  /**
   *
   * Calculate and return the associations between a container of  
   * LHCb::Particle and a container of LHCb::MCParticles on the TES
   * The TES location of the LHCb::MCParticles is handled by the 
   * implementations
   * @param particles Container of LHCb::Particles to be associated
   * @return P2MCPTypes::LightTable Relations table containing 
   * relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::Container& particles) const = 0;

  /**
   *
   * Calculate and return the associations between a container of
   * LHCb::Particles and a container of LHCb::MCParticles on the TES
   * @param particles Container of LHCb::Particles to be associated
   * @param mcParticleLocation TES location of LHCb::MCParticles to base the
   * association on.
   * @return P2MCPTypes::LightTable Relations table containing 
   * relations between the LHCb::Particles and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::Container& particles,
               const std::string& mcParticleLocation) const = 0;

  /**
   *
   * Calculate and return the associations between a container of  
   * LHCb::Particle and a container of LHCb::MCParticles on the TES
   * The TES location of the LHCb::MCParticles is handled by the 
   * implementations
   * @param particles Container of LHCb::Particles to be associated
   * @return P2MCPTypes::LightTable Relations table containing 
   * relations between the LHCb::Particle and the LCHb::MCParticles
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/ 
  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::Container& particles,
               const LHCb::MCParticle::ConstVector& mcParticles) const = 0;
  /**
   * Calculate the association weight between an LHCb::Particle and an
   * LHCb::MCParticle.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date   2009-02-02
   *
   **/
  virtual bool 
  isMatched(const LHCb::Particle* particle, 
            const LHCb::MCParticle* mcParticle) const = 0;

};
#endif // P2MCP_IP2MCP_H
