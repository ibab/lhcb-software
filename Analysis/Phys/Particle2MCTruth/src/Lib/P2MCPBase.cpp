// $Id: P2MCPBase.cpp,v 1.3 2009-03-13 18:08:07 jpalac Exp $
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
const LHCb::MCParticle* 
P2MCPBase::bestRelatedMCP(const LHCb::Particle* particle) const 
{
  const LHCb::MCParticle::Container* mcParticles = i_MCParticles(m_defMCLoc);
  return (0==mcParticles) ? 0 : i_bestMCP(particle, 
                                          mcParticles->begin(), 
                                          mcParticles->end() );
}
//=============================================================================
const LHCb::MCParticle* 
P2MCPBase::bestRelatedMCP(const LHCb::Particle* particle,
                          const LHCb::MCParticle::ConstVector& mcParticles) const
{
  return i_bestMCP(particle, mcParticles.begin(), mcParticles.end());
}
//=============================================================================
const LHCb::MCParticle* 
P2MCPBase::bestRelatedMCP(const LHCb::Particle* particle,
                          const LHCb::MCParticle::Container& mcParticles) const 
{
  return i_bestMCP(particle, mcParticles.begin(), mcParticles.end());
}
//=============================================================================
P2MCP::Types::FlatTrees 
P2MCPBase::relatedMCPs(const LHCb::Particle* particle) const
{
  return relatedMCPs(particle, m_defMCLoc);
}
//=============================================================================
P2MCP::Types::FlatTrees 
P2MCPBase::relatedMCPs(const LHCb::Particle* particle,
                       const std::string& mcParticleLocation) const
{
  LHCb::MCParticle::Container* mcps = i_MCParticles(mcParticleLocation);
  if (0!=mcps) {
    return i_relatedMCPs(particle,  mcps->begin(), mcps->end());
  } else {
    return P2MCP::Types::FlatTrees();
  }   
}
//=============================================================================
P2MCP::Types::FlatTrees 
P2MCPBase::relatedMCPs(const LHCb::Particle* particle,
                       const LHCb::MCParticle::ConstVector& mcParticles) const
{
  return i_relatedMCPs(particle,  mcParticles.begin(), mcParticles.end());
}
//=============================================================================
P2MCP::Types::FlatTrees 
P2MCPBase::relatedMCPs(const LHCb::Particle* particle,
                       const LHCb::MCParticle::Container& mcParticles) const
{
  return i_relatedMCPs(particle,  mcParticles.begin(), mcParticles.end());
}
//=============================================================================
bool
P2MCPBase::isMatched(const LHCb::Particle* /*particle */, 
                     const LHCb::MCParticle* /* mcParticle */) const
{
  return false;
}
//=============================================================================
P2MCP::Types::FlatTrees
P2MCPBase::sort(const LHCb::MCParticle::ConstVector& mcParticles) const 
{
  return i_sort(mcParticles.begin(), mcParticles.end());
}
//=============================================================================
P2MCP::Types::FlatTrees
P2MCPBase::sort(const LHCb::MCParticle::Container& mcParticles) const
{
  return i_sort(mcParticles.begin(), mcParticles.end());
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
