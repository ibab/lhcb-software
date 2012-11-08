// $Id: P2MCRelatorAlg.h,v 1.4 2009-08-25 13:21:28 jpalac Exp $
#ifndef P2MCRELATORALG_H
#define P2MCRELATORALG_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
// from LHCb
#include "Kernel/Particle2MCParticle.h"
// from P2MCP
#include "P2MCP/IP2MCP.h"
// boost
#include "boost/algorithm/string.hpp"

/** @class P2MCRelatorAlg P2MCRelatorAlg.h
 *
 *  GaudiAlgorithm that takes an MCParticle and Particle location, and uses
 *  an IP2MCP to create a Particle2MCParticle::Table of associations between
 *  Particles and MCParticles that pass IP2MCP::isMatched.
 *  The relations table is written to the parent of ParticleLocation + "P2MCPRelations".
 *
 *  <b> Properties</b>
 *
 *  ParticleLocations: TES location of input LHCb::Particles
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
 *  p2mc.ParticleLocations = ["Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles"]
 *  # Add it to a selection sequence
 *  seq = GaudiSequencer('SeqDC06selBd2Jpsi2MuMu_Kst2KPi')
 *  seq.Members += [p2mc]
 *  @endcode
 *
 *
 *  @author Juan PALACIOS
 *  @date   2009-04-16
 */
class P2MCRelatorAlg : public GaudiAlgorithm
{

public:

  /// Standard constructor
  P2MCRelatorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~P2MCRelatorAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  /** Returns the full location of the given object in the Data Store
   *
   *  @param pObj Data object
   *
   *  @return Location of given data object
   */
  inline std::string objectLocation( const DataObject * pObj ) const
  {
    return ( !pObj ? "" : (pObj->registry() ? pObj->registry()->identifier() : "") );
  }

  template <class Iter>
  void i_particleLoop( const Iter begin, const Iter end, 
                       const std::string level = "" )
  {
    for ( Iter it = begin; it != end; ++it )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << level
                << "-> " << objectLocation( (*it)->parent() )
                << " : key=" << (*it)->key()
                << " PID=" << (*it)->particleID().pid()
                << endmsg;
      for ( LHCb::MCParticle::Container::const_iterator iMCP = m_mcParticles->begin();
            iMCP != m_mcParticles->end(); ++iMCP )
      {
        if ( m_p2mcp->isMatched(*it,*iMCP) )
        {
          // Get the TES location for the relations for this Particle
          const std::string pLoc = relationsLocation( (*it)->parent() );
          if ( msgLevel(MSG::DEBUG) )
            debug() << level 
                    << " -> Found MCMatch : key=" << (*iMCP)->key()
                    << " MCPID=" << (*iMCP)->particleID().pid() 
                    << " : Filling " << pLoc 
                    << endmsg;
          if ( !pLoc.empty() )
          {
            // Fill table for this location
            m_tables[pLoc].i_push(*it,*iMCP);
          }
          else
          {
            Warning( "Particle parent not in the TES" ).ignore();
          }
        }
      }
      // Recursively save the relations for the daughters
      const SmartRefVector<LHCb::Particle> & daughters = (*it)->daughters();
      if ( !daughters.empty() )
      {
        i_particleLoop( daughters.begin(), daughters.end(), level+" " );
      }
    }
  }

  std::string relationsLocation( const DataObject * pObj ) const;

private:

  std::vector<std::string> m_particleLocations;
  std::string m_mcpLocation;
  IP2MCP * m_p2mcp;
  std::string m_p2mcpType;
  typedef std::map< std::string, Particle2MCParticle::LightTable > Tables;
  Tables m_tables;
  LHCb::MCParticle::Container * m_mcParticles;

};

#endif // P2MCRELATORALG_H
