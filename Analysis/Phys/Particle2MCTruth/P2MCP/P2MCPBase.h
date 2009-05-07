// $Id: P2MCPBase.h,v 1.6 2009-05-07 10:29:31 jpalac Exp $
#ifndef P2MCP_P2MCPBASE_H 
#define P2MCP_P2MCPBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// local
#include "P2MCP/IP2MCP.h"            // Interface


/** @class P2MCPBase P2MCPBase.h
 *  
 *  Common implementation for descendants of IParticle2MCAssociator.
 *  Mainly inline helper methods for common implementation of host of 
 *  similar methods in the interface.
 *  Set of methods is self-consistent. Derived classes only need to implement
 *  methods
 *  @code 
 *  bool isMatched(const LHCb::Particle*, const LHCb::MCParticle)
 *  @endcode
 *  and
 *  @code
 *  LHCb::MCParticle::ConstVector sort(const LHCb::MCParticle::Container* mcParticles) const
 *  @code
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

  virtual P2MCP::Types::FlatTrees
  relatedMCPs(const LHCb::Particle* particle) const ;

  virtual P2MCP::Types::FlatTrees
  relatedMCPs(const LHCb::Particle* particle,
              const std::string& mcParticleLocation) const ;
  
  virtual P2MCP::Types::FlatTrees
  relatedMCPs(const LHCb::Particle* particle,
              const LHCb::MCParticle::ConstVector& mcParticles) const ;

  virtual P2MCP::Types::FlatTrees
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
    P2MCP::Types::FlatTrees trees = i_relatedMCPs(particle, begin, end);
    return (trees.empty() ) ? 0 : trees[0].back();

  }
  

  template <typename Iter> 
  P2MCP::Types::FlatTrees 
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
      return sort( mcps );
    } else {
      Warning("No particle!").ignore();
      P2MCP::Types::FlatTrees trees(0);
      return trees;
    }
  }

private :

  virtual 
  P2MCP::Types::FlatTrees 
  sort(const LHCb::MCParticle::ConstVector& mcParticles) const;

  virtual 
  P2MCP::Types::FlatTrees 
  sort(const LHCb::MCParticle::Container& mcParticles) const;

  template <typename Iter> 
  P2MCP::Types::FlatTrees i_sort(const Iter begin, const Iter end) const
  {
    return P2MCP::Types::FlatTrees(1,P2MCP::Types::FlatTree(begin, end));
  }

private:

  std::string m_defMCLoc;

};
#endif // P2MCP_P2MCPBASE_H
