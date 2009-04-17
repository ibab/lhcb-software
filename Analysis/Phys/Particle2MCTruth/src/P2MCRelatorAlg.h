// $Id: P2MCRelatorAlg.h,v 1.2 2009-04-17 20:54:12 jpalac Exp $
#ifndef P2MCRELATORALG_H 
#define P2MCRELATORALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from LHCb
#include "Kernel/Particle2MCParticle.h"
// from P2MCP
#include "P2MCP/IP2MCP.h"

/** @class P2MCRelatorAlg P2MCRelatorAlg.h
 *  
 *  GaudiAlgorithm that takes an MCParticle and Particle location, and uses
 *  an IP2MCP to create a Particle2MCParticle::Table of associations between
 *  Particles and MCParticles that pass IP2MCP::isMatched.
 *  The relations table is written to the parent of ParticleLocation + "P2MCPRelations".
 *  <b> Properties</b>
 *
 *  ParticleLocation: TES location of input LHCb::Particles
 *
 *  MCParticleLocation: TES location of input LHCb::Particles. Default: LHCb::MCParticleLocation::Default
 *
 *  IP2MCP: Implementation name of IP2MCP interface. Default: MCMatchObjP2MCRelator
 *
 *  <b>Example</b>: Create relations table for LHCb::MCParticles associated to 
 *  LHCb::Particles in "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles". Table is
 *  written to "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2MCPRelations"
 *
 *  @code
 *  from Configurables import P2MCRelatorAlg
 *  p2mc = P2MCRelatorAlg()
 *  p2mc.ParticleLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles"
 *  # Add it to a selection sequence
 *  seq = GaudiSequencer('SeqDC06selBd2Jpsi2MuMu_Kst2KPi')
 *  seq.Members += [p2mc]
 *  @endcode
 *
 *
 *  @author Juan PALACIOS
 *  @date   2009-04-16
 */
class P2MCRelatorAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  P2MCRelatorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~P2MCRelatorAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  template <class Iter>
  void i_particleLoop(Iter begin, Iter end) 
  {
    for (Iter it = begin; it!= end; ++it) {
      LHCb::MCParticle::Container::const_iterator iMCP = 
        m_mcParticles->begin();
      for ( ; iMCP != m_mcParticles->end(); ++iMCP) {
        if (m_p2mcp->isMatched(*it, *iMCP ) ) m_table.i_push(*it, *iMCP);
      }
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "stored " 
                                              << m_table.relations(*it).size() 
                                              << " relations for Particle " 
                                              << (*it)->particleID().pid() 
                                              << endmsg;
      const SmartRefVector<LHCb::Particle>& daughters = (*it)->daughters();
      i_particleLoop(daughters.begin(), daughters.end() );
    }
    
  }
  
  std::string trunkLocation(const std::string& location) const;

private:

  std::string m_particleLocation;
  std::string m_mcpLocation;
  IP2MCP* m_p2mcp;
  std::string m_p2mcpType;
  Particle2MCParticle::LightTable m_table;
  LHCb::MCParticle::Container* m_mcParticles;
  
};
#endif // P2MCRELATORALG_H
