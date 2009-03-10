// $Id: P2MCPBase.cpp,v 1.2 2009-03-10 18:31:28 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "P2MCP/P2MCPBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : P2MCPBase
//
// 2009-01-30 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
P2MCPBase::P2MCPBase( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : 
  GaudiTool ( type, name , parent ),
  m_defMCLoc(LHCb::MCParticleLocation::Default)
{
  declareProperty ( "MCParticleDefaultLocation" , m_defMCLoc  ) ;
}
//=============================================================================
P2MCPTypes::ToVector 
P2MCPBase::associate(const LHCb::Particle* particle) const 
{
  return associate(particle, m_defMCLoc);
}
//=============================================================================
P2MCPTypes::ToVector 
P2MCPBase::associate(const LHCb::Particle* particle,
                     const std::string& mcParticleLocation) const 
{
  P2MCPTypes::ToVector assocVector;
  P2MCPTypes::LightTable table = this->relatedMCPs(particle,
                                                   mcParticleLocation);
  

  P2MCPTypes::LightTable::Range r = table.i_relations(particle);
  for (P2MCPTypes::LightTable::Range::const_iterator i = r.begin();
       i!=r.end(); ++i) {
    MCAssociation myMCAssoc(i->to(), 1.);
    assocVector.push_back(myMCAssoc);
  }
  return assocVector;
}
//=============================================================================
P2MCPTypes::LightTable 
P2MCPBase::relatedMCPs(const LHCb::Particle* particle) const
{
  return relatedMCPs(particle, m_defMCLoc);
}
//=============================================================================
P2MCPTypes::LightTable 
P2MCPBase::relatedMCPs(const LHCb::Particle* particle,
                       const std::string& mcParticleLocation) const
{
  LHCb::MCParticle::Container* mcps = i_MCParticles(mcParticleLocation);
  if (0!=mcps) {
    return i_relatedMCPs(particle,  mcps->begin(), mcps->end());
  } else {
    return P2MCPTypes::LightTable();
  }   
}
//=============================================================================
P2MCPTypes::LightTable 
P2MCPBase::relatedMCPs(const LHCb::Particle* particle,
                       const LHCb::MCParticle::ConstVector& mcParticles) const
{
  return i_relatedMCPs(particle,  mcParticles.begin(), mcParticles.end());
}
//=============================================================================
P2MCPTypes::LightTable 
P2MCPBase::associations(const LHCb::Particle::ConstVector& particles) const
{
  return i_associations(particles.begin(), particles.end(), m_defMCLoc);
}
//=============================================================================
P2MCPTypes::LightTable 
P2MCPBase::associations(const LHCb::Particle::ConstVector& particles,
                        const std::string& mcParticleLocation) const
{
  return i_associations(particles.begin(), particles.end(), mcParticleLocation);
}
//=============================================================================
P2MCPTypes::LightTable 
P2MCPBase::associations(const LHCb::Particle::ConstVector& particles,
                        const LHCb::MCParticle::ConstVector& mcParticles) const
{
  return i_associations(particles.begin(),
                        particles.end(),
                        mcParticles.begin(),
                        mcParticles.end());
  
}
//=============================================================================
P2MCPTypes::LightTable 
P2MCPBase::associations(const LHCb::Particle::Container& particles) const
{
  return i_associations(particles.begin(), particles.end(), m_defMCLoc);
}
//=============================================================================
P2MCPTypes::LightTable 
P2MCPBase::associations(const LHCb::Particle::Container& particles,
                        const std::string& mcParticleLocation) const
{
  return i_associations(particles.begin(), particles.end(), mcParticleLocation);  
}
//=============================================================================
P2MCPTypes::LightTable 
P2MCPBase::associations(const LHCb::Particle::Container& particles,
                        const LHCb::MCParticle::ConstVector& mcParticles) const
{
  return i_associations(particles.begin(), particles.end(), mcParticles.begin(), mcParticles.end());          
}
//=============================================================================
bool
P2MCPBase::isMatched(const LHCb::Particle* /*particle */, 
                     const LHCb::MCParticle* /* mcParticle */) const
{
  return false;
}
//=============================================================================
LHCb::MCParticle::ConstVector 
P2MCPBase::sort(const LHCb::MCParticle::ConstVector& mcParticles) const 
{
  return i_sort(mcParticles.begin(), mcParticles.end());
}
//=============================================================================
LHCb::MCParticle::ConstVector 
P2MCPBase::sort(const LHCb::MCParticle::Container* mcParticles) const
{
  return i_sort(mcParticles->begin(), mcParticles->end());
}
//=============================================================================
// initialize
//=============================================================================
StatusCode P2MCPBase::initialize() {
  return GaudiTool::initialize();
}
//=============================================================================
// finalize
//=============================================================================
StatusCode P2MCPBase::finalize() {
  return GaudiTool::finalize() ; 
}
//=============================================================================
// Destructor
//=============================================================================
P2MCPBase::~P2MCPBase() {}
//=============================================================================
