// $Id: P2MCPBase.h,v 1.10 2009-10-15 09:08:50 jpalac Exp $
#ifndef P2MCP_P2MCPBASE_H 
#define P2MCP_P2MCPBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// local
#include "P2MCP/IP2MCP.h"            // Interface


/** @class P2MCPBase P2MCPBase.h P2MCP/P2MCPBase.h
 *  
 *  Common implementation for descendants of IParticle2MCAssociator.
 *  Mainly inline helper methods for common implementation of host of 
 *  similar methods in the interface.
 *  Set of methods is self-consistent. Derived classes only need to implement
 *  method
 *  @code 
 *  bool isMatched(const LHCb::Particle*, const LHCb::MCParticle)
 *  @endcode
 *  and
 *
 *  @author Juan PALACIOS
 *  @date   2009-01-30
 */
class P2MCPBase : public GaudiTool,
                  virtual public IP2MCP
{
public: 
  /// Standard constructor
  P2MCPBase( const std::string& type, 
             const std::string& name,
             const IInterface* parent);

  virtual StatusCode initialize() ;

  virtual StatusCode finalize() ;
  
  virtual ~P2MCPBase( );

  virtual 
  const LHCb::MCParticle* 
  relatedMCP(const LHCb::Particle* particle) const ;

  virtual 
  const LHCb::MCParticle* 
  operator() (const LHCb::Particle* particle) const ;


  virtual const LHCb::MCParticle*
  relatedMCP(const LHCb::Particle*,
             const std::string& mcParticleLocation) const ;

  virtual 
  const LHCb::MCParticle* 
  relatedMCP(const LHCb::Particle* particle,
             const LHCb::MCParticle::ConstVector& mcParticles) const ;

  virtual 
  const LHCb::MCParticle* 
  relatedMCP(const LHCb::Particle* particle,
             const LHCb::MCParticle::Container& mcParticles) const ;

  virtual P2MCP::DecayLines
  relatedMCPs(const LHCb::Particle* particle) const ;

  virtual P2MCP::DecayLines
  relatedMCPs(const LHCb::Particle* particle,
              const std::string& mcParticleLocation) const ;
  
  virtual P2MCP::DecayLines
  relatedMCPs(const LHCb::Particle* particle,
              const LHCb::MCParticle::ConstVector& mcParticles) const ;

  virtual P2MCP::DecayLines
  relatedMCPs(const LHCb::Particle* particle,
              const LHCb::MCParticle::Container& mcParticles) const ;

  virtual bool 
  isMatched(const LHCb::Particle* particle, 
            const LHCb::MCParticle* mcParticle) const ;
  
private:

  inline LHCb::MCParticle::Container* 
  i_MCParticles(const std::string& location) const
  {
    return (exist<LHCb::MCParticle::Container>( location )) ? 
      get<LHCb::MCParticle::Container>( location ) : 0 ;
  }
  
  template <typename Iter>
  const LHCb::MCParticle*
  i_bestMCP(const LHCb::Particle* particle,
            Iter begin,
            Iter end ) const 
  {
    P2MCP::DecayLines trees = i_relatedMCPs(particle, begin, end);
    return (trees.empty() ) ? 0 : trees[0].back();

  }
  

  template <typename Iter> 
  P2MCP::DecayLines
  i_relatedMCPs(const LHCb::Particle* particle,
                Iter begin,
                Iter end     ) const
  {
    if (0!=particle) {
      LHCb::MCParticle::ConstVector mcps;
      for ( Iter iMCP = begin ; iMCP != end ; ++iMCP){
        const bool match = isMatched(particle, *iMCP);
        if ( match ) mcps.push_back(*iMCP);
      }
      return P2MCP::DecayLines( mcps );
    } else {
      Warning("No particle!").ignore();
      return P2MCP::DecayLines();
    }
  }

private:

  std::string m_defMCLoc;

};
#endif // P2MCP_P2MCPBASE_H
