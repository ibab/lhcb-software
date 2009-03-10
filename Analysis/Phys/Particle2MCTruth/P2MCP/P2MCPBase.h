// $Id: P2MCPBase.h,v 1.2 2009-03-10 18:29:38 jpalac Exp $
#ifndef P2MCP_P2MCPBASE_H 
#define P2MCP_P2MCPBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// LHCb
#include "Event/MCParticle.h"
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
                  virtual public IP2MCP {
public: 
  /// Standard constructor
  P2MCPBase( const std::string& type, 
             const std::string& name,
             const IInterface* parent);

  virtual StatusCode initialize() ;

  virtual StatusCode finalize() ;
  
  virtual ~P2MCPBase( );

  virtual P2MCPTypes::ToVector 
  associate(const LHCb::Particle* particle) const ;

  virtual P2MCPTypes::ToVector 
  associate(const LHCb::Particle* particle,
            const std::string& mcParticleLocation) const ;

  virtual P2MCPTypes::LightTable 
  relatedMCPs(const LHCb::Particle*) const ;

  virtual P2MCPTypes::LightTable 
  relatedMCPs(const LHCb::Particle*,
              const std::string& mcParticleLocation) const ;
  
  virtual P2MCPTypes::LightTable 
  relatedMCPs(const LHCb::Particle*,
              const LHCb::MCParticle::ConstVector& mcParticles) const ;

  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::ConstVector& particles) const ;
  
  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::ConstVector& particles,
               const std::string& mcParticleLocation) const ;

  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::ConstVector& particles,
               const LHCb::MCParticle::ConstVector& mcParticles) const ;

  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::Container& particles) const ;

  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::Container& particles,
               const std::string& mcParticleLocation) const ;

  virtual P2MCPTypes::LightTable 
  associations(const LHCb::Particle::Container& particles,
               const LHCb::MCParticle::ConstVector& mcParticles) const ;

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
  P2MCPTypes::LightTable 
  i_relatedMCPs(const LHCb::Particle* particle,
                Iter begin,
                Iter end     ) const
  {
    P2MCPTypes::LightTable table;
    if (0!=particle) {
      LHCb::MCParticle::ConstVector mcps;
      for ( Iter iMCP = begin ; iMCP != end ; ++iMCP){
        const bool match = isMatched(particle, *iMCP);
        if ( match ) mcps.push_back(*iMCP);
      }
      mcps = sort(mcps);
      return i_buildTable(particle, mcps.begin(), mcps.end() );
    } else {
      Warning("No particle!").ignore();
      return P2MCPTypes::LightTable();
    }
  }

  template <typename pIter, typename mcPIter>
  P2MCPTypes::LightTable 
  i_associations(const pIter pBegin, 
                 const pIter pEnd, 
                 const mcPIter mcBegin, 
                 const mcPIter mcEnd) const
  {

    P2MCPTypes::LightTable table;
    for (pIter part = pBegin; part != pEnd; ++part) {
      table.merge( i_relatedMCPs(*part, mcBegin, mcEnd).relations() );
    }
    //    table.i_sort();
    return table;
  }
  
  template <typename Iter> 
  inline P2MCPTypes::LightTable 
  i_associations(const Iter pBegin,
                 const Iter pEnd,
                 const std::string& mcParticleLocation) const
  {
    LHCb::MCParticle::Container* mcps = i_MCParticles(mcParticleLocation);
    if (0!=mcps) {
      return i_associations(pBegin, pEnd, mcps->begin(), mcps->end());
    } else {
      return P2MCPTypes::LightTable();
    }
  }

  template <typename Iter>
  P2MCPTypes::LightTable i_buildTable(const LHCb::Particle* particle,
                                      const Iter mcBegin,
                                      const Iter mcEnd) const
  {
    P2MCPTypes::LightTable table;
    for ( Iter iMCP = mcBegin ; iMCP != mcEnd ; ++iMCP) {
      table.i_push( particle, *iMCP );
    }
    //    table.i_sort();
    return table;
  }

private :

  virtual LHCb::MCParticle::ConstVector sort(const LHCb::MCParticle::ConstVector& mcParticles) const;

  virtual LHCb::MCParticle::ConstVector sort(const LHCb::MCParticle::Container* mcParticles) const;

  template <typename Iter> 
  LHCb::MCParticle::ConstVector i_sort(const Iter begin, const Iter end) const
  {
    return LHCb::MCParticle::ConstVector(begin, end);
  }

private:

  std::string m_defMCLoc;

};
#endif // P2MCP_P2MCPBASE_H
